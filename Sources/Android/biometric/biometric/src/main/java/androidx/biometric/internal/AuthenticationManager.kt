/*
 * Copyright 2025 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package androidx.biometric.internal

import android.content.Context
import android.os.Build
import android.os.Handler
import android.os.Looper
import androidx.biometric.BiometricManager
import androidx.biometric.BiometricPrompt
import androidx.biometric.BiometricPrompt.AuthenticationCallback
import androidx.biometric.R
import androidx.biometric.internal.viewmodel.AuthenticationViewModel
import androidx.biometric.utils.AuthenticatorUtils
import androidx.biometric.utils.DeviceUtils
import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import androidx.lifecycle.LifecycleOwner
import androidx.lifecycle.lifecycleScope
import java.util.concurrent.Executor
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch

/**
 * A central coordinator for biometric authentication sessions.
 *
 * This class provides shared logic for different concrete implementations of the
 * [AuthenticationHandler] interface. It allows for better separation of concerns and reusability by
 * composing common authentication logic, such as managing state, dispatching results to the client,
 * and handling lifecycle events.
 *
 * This manager observes a [AuthenticationViewModel] for authentication results and errors,
 * orchestrating the flow of an authentication session from start to finish.
 *
 * @param context The application context.
 * @param lifecycleOwner The [LifecycleOwner] associated with the authentication session.
 * @param viewModel The [AuthenticationViewModel] to manage the state of the authentication prompt.
 * @param confirmCredentialActivityLauncher A [Runnable] to launch the device credential activity.
 * @param clientExecutor The [Executor] on which to run the client's callback methods.
 * @param clientAuthenticationCallback The client's original [AuthenticationCallback] to be invoked.
 */
internal class AuthenticationManager(
    val context: Context,
    val lifecycleOwner: LifecycleOwner,
    val viewModel: AuthenticationViewModel,
    val confirmCredentialActivityLauncher: Runnable,
    clientExecutor: Executor,
    clientAuthenticationCallback: AuthenticationCallback,
) {
    /** The dispatcher responsible for sending authentication results to the client's callback. */
    var resultDispatcher: AuthenticationResultDispatcher =
        object :
            AuthenticationResultDispatcher(
                context,
                viewModel,
                clientExecutor,
                clientAuthenticationCallback,
                confirmCredentialActivityLauncher,
                { dismiss() },
            ) {}
        private set

    /** An optional observer strategy for handling authentication UI state. */
    var uiStateObserver: AuthenticationUiStateObserver? = null
        private set

    /** A container to manage lifecycle event observers, ensuring they are properly cleaned up. */
    val lifecycleContainer = BiometricPrompt.LifecycleContainer(lifecycleOwner.lifecycle)

    /**
     * A [Handler] associated with the main application looper, used for UI-related tasks or for
     * delaying operations.
     */
    val mainHandler = Handler(Looper.getMainLooper())

    /** A flag to ensure the class is initialized only once. */
    private var isInitialized: Boolean = false
    /** A flag indicating whether the authentication is already prepared. */
    private var isAuthenticationPrepared: Boolean = false

    private var callbackObserverJob: Job? = null

    /**
     * Observes a negative button press, which is typically used for either canceling the
     * authentication or falling back to the device's credential screen.
     */
    val isNegativeButtonPressPendingObserver = {
        if (viewModel.isPromptShowing) {
            if (context.isManagingDeviceCredentialButton(viewModel.allowedAuthenticators)) {
                resultDispatcher.showKMAsFallback()
            } else {
                onCancelButtonPressed()
            }
        }
    }

    /**
     * Sets up the dispatchers and observers.
     *
     * This function optionally updates the [resultDispatcher] and [uiStateObserver] properties. If
     * a parameter is provided as non-null, the corresponding property will be updated; otherwise,
     * the current value is retained.
     *
     * @param resultDispatcher An optional [AuthenticationResultDispatcher] to be used.
     * @param uiStateObserver An optional [AuthenticationUiStateObserver] to be used.
     */
    fun initialize(
        resultDispatcher: AuthenticationResultDispatcher? = this.resultDispatcher,
        uiStateObserver: AuthenticationUiStateObserver? = this.uiStateObserver,
    ) {
        if (isInitialized) {
            return
        }
        isInitialized = true
        resultDispatcher?.let { this.resultDispatcher = it }
        uiStateObserver?.let { this.uiStateObserver = it }

        // When activity/fragment restarts, we need to check |viewModel.isPromptShowing| for
        // reconnecting view models.
        val observer = LifecycleEventObserver { owner, event ->
            when (event) {
                Lifecycle.Event.ON_START ->
                    if (viewModel.isPromptShowing) {
                        prepareAuth()
                    }

                Lifecycle.Event.ON_DESTROY -> {
                    destroy()
                    lifecycleContainer.clearObservers()
                }

                else -> {}
            }
        }
        lifecycleContainer.addObserver(observer)
    }

    /**
     * Shows the prompt UI and begins an authentication session.
     *
     * @param info The [BiometricPrompt.PromptInfo] to be used for the prompt.
     * @param crypto An optional [BiometricPrompt.CryptoObject] for cryptographically-secure
     *   authentication.
     * @param showAuthentication A lambda function that encapsulates the action of displaying the
     *   authentication prompt. This is where the actual framework call to show the authentication
     *   UI should be called.
     */
    fun authenticate(
        info: BiometricPrompt.PromptInfo,
        crypto: BiometricPrompt.CryptoObject?,
        showAuthentication: () -> Unit,
    ) {
        prepareAuth()
        // PromptInfo has to be set prior to others.
        viewModel.setPromptInfo(info)

        viewModel.isIdentityCheckAvailable =
            BiometricManager.from(context).isIdentityCheckAvailable()

        viewModel.cryptoObject = crypto

        viewModel.setNegativeButtonTextOverride(
            if (context.isManagingDeviceCredentialButton(viewModel.allowedAuthenticators)) {
                context.getString(R.string.confirm_device_credential_password)
            } else {
                // Don't override the negative button text from the client.
                null
            }
        )

        // Fall back to device credential immediately if no known biometrics are available.
        if (context.isKeyguardManagerNeededForNoBiometric(viewModel.allowedAuthenticators)) {
            viewModel.isDelayingPrompt = false
        }

        // Check if we should delay showing the authentication prompt.
        if (viewModel.isDelayingPrompt) {
            mainHandler.postDelayed(
                { showPromptForAuthentication(showAuthentication) },
                SHOW_PROMPT_DELAY_MS.toLong(),
            )
        } else {
            showPromptForAuthentication(showAuthentication)
        }
    }

    /**
     * Cancels the ongoing authentication session with the [canceledFrom] info and sends an error to
     * the client callback.
     */
    fun cancelAuthentication(canceledFrom: CanceledFrom) {
        if (canceledFrom != CanceledFrom.CLIENT && viewModel.isIgnoringCancel) {
            return
        }
        viewModel.canceledFrom = canceledFrom
        viewModel.cancellationSignalProvider.cancel()
        destroy()
    }

    /** Removes any associated UI from the client activity/fragment. */
    fun dismiss() {
        viewModel.isPromptShowing = false
        viewModel.isConfirmingDeviceCredential = false

        // Wait before showing again to work around a dismissal logic issue on API 29 (b/157783075).
        if (DeviceUtils.shouldDelayShowingPrompt(context, Build.MODEL)) {
            viewModel.isDelayingPrompt = true
            viewModel.setDelayedDelayingPrompt(false, SHOW_PROMPT_DELAY_MS.toLong())
        }
        destroy()
    }

    /** Prepares the authentication, setting up view model observers. */
    private fun prepareAuth() {
        if (isAuthenticationPrepared) {
            return
        }
        isAuthenticationPrepared = true

        connectCallbackObservers()
        uiStateObserver?.connectObservers()

        // Some device credential implementations in API 29 cause the prompt to receive a cancel
        // signal immediately after it's shown (b/162022588).
        // TODO(b/162022588): mViewModel.info hasn't been set. So isDeviceCredentialAllowed()
        //  check will always be false. Reproduce the bug and fix it.
        if (
            Build.VERSION.SDK_INT == Build.VERSION_CODES.Q &&
                AuthenticatorUtils.isDeviceCredentialAllowed(viewModel.allowedAuthenticators)
        ) {
            viewModel.isIgnoringCancel = true
            viewModel.setDelayedIgnoringCancel(false, 250L)
        }

        // TODO(b/263800618): Add lifecycle observer to cancel authentication when the app enters
        // the background. ProcessLifecycleOwner.get().lifecycle.addObserver(processObserver), or
        // find a better alternative to handle backgrounded cancelling authentication, e.g. combine
        // LifecycleEventObserver and fragment.isRemoving()/!activity.isChangingConfigurations().
    }

    /**
     * Cleans up resources and unregisters any view model observers associated with the
     * authentication session.
     */
    private fun destroy() {
        isAuthenticationPrepared = false
        disconnectCallbackObservers()
        uiStateObserver?.disconnectObservers()

        lifecycleContainer.clearObservers()

        // TODO(b/263800618): Remove lifecycle observer
        // ProcessLifecycleOwner.get().lifecycle.removeObserver(processObserver)
    }

    /**
     * Connects the [AuthenticationViewModel] for the ongoing authentication session. This method is
     * paired with [disconnectCallbackObservers].
     */
    private fun connectCallbackObservers() {
        callbackObserverJob =
            lifecycleOwner.lifecycleScope.launch {
                launch {
                    viewModel.authenticationResult.collect { authenticationResult ->
                        if (viewModel.isPromptShowing) {
                            resultDispatcher.onAuthenticationSucceeded(authenticationResult)
                        }
                    }
                }
                launch {
                    viewModel.authenticationError.collect { authenticationError ->
                        if (viewModel.isPromptShowing) {
                            resultDispatcher.onAuthenticationError(
                                authenticationError.errorCode,
                                authenticationError.errorMessage,
                            )
                        }
                    }
                }
                launch {
                    viewModel.isAuthenticationFailurePending.collect {
                        if (viewModel.isPromptShowing) {
                            resultDispatcher.onAuthenticationFailed()
                        }
                    }
                }
            }
    }

    /** Disconnects all observers from the [AuthenticationViewModel]. */
    private fun disconnectCallbackObservers() {
        callbackObserverJob?.cancel()
        callbackObserverJob = null
    }

    /**
     * Callback that is run when the view model reports that the cancel button has been pressed on
     * the prompt.
     */
    private fun onCancelButtonPressed() {
        val negativeButtonText: CharSequence? = viewModel.negativeButtonText

        resultDispatcher.sendErrorAndDismiss(
            BiometricPrompt.ERROR_NEGATIVE_BUTTON,
            negativeButtonText ?: context.getString(R.string.default_error_msg),
        )

        cancelAuthentication(CanceledFrom.NEGATIVE_BUTTON)
    }

    /**
     * Shows any of the framework biometric prompt, or framework credential view, or AndroidX
     * fingerprint UI dialog to the user and begins authentication.
     */
    @Suppress("WeakerAccess")
    private fun showPromptForAuthentication(showAuthentication: () -> Unit) {
        showAuthentication()

        viewModel.isPromptShowing = true
        viewModel.isAwaitingResult = true
    }

    private companion object {
        /**
         * The amount of time (in milliseconds) to wait before showing the authentication UI if
         * [AuthenticationViewModel.isDelayingPrompt] is `true`.
         */
        private const val SHOW_PROMPT_DELAY_MS = 600
    }
}

/** Represents the source or reason why an authentication operation was canceled. */
internal enum class CanceledFrom {
    INTERNAL,
    USER,
    NEGATIVE_BUTTON,
    CLIENT,
    MORE_OPTIONS_BUTTON,
}

private class AppLifecycleListener(val onBackgrounded: () -> Unit = {}) : DefaultLifecycleObserver {
    override fun onStop(owner: LifecycleOwner) {
        // app moved to background
        onBackgrounded()
    }
}
