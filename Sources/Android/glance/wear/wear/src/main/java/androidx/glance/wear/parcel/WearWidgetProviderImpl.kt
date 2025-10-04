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

package androidx.glance.wear.parcel

import android.content.ComponentName
import android.content.Context
import android.util.Log
import androidx.glance.wear.ActiveWearWidgetHandle
import androidx.glance.wear.GlanceWearWidget
import androidx.glance.wear.WearWidgetRequest
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch

/**
 * Implementation of the [IWearWidgetProvider] Stub.
 *
 * @property context The Context of the service that communicates using this Stub
 * @property providerName The [ComponentName] of the provider service.
 * @property mainScope A main-thread scope
 * @property widget The widget used to receive the calls to this stub
 */
internal class WearWidgetProviderImpl(
    private val context: Context,
    private val providerName: ComponentName,
    private val mainScope: CoroutineScope,
    private val widget: GlanceWearWidget,
) : IWearWidgetProvider.Stub() {

    override fun getApiVersion(): Int = API_VERSION

    override fun onWidgetRequest(
        requestParcel: WearWidgetRequestParcel?,
        callback: IWearWidgetCallback?,
    ) {
        requireNotNull(requestParcel) { "Invalid widget request." }
        requireNotNull(callback) { "Invalid widget callback." }
        mainScope.launch {
            // TODO: Report errors in the callback if any of the following steps fail.
            val request = WearWidgetRequest.fromParcel(requestParcel)
            val widgetContent = widget.provideWidgetContent(context, request)
            val rawContent =
                widgetContent.captureRawContent(context, request.widthDp, request.heightDp)
            callback.updateWidgetContent(rawContent.toParcel())
        }
    }

    override fun onActivated(
        handleParcel: ActiveWearWidgetHandleParcel?,
        callback: IExecutionCallback?,
    ) {
        requireNotNull(handleParcel) { "Invalid widget handle parcel." }
        mainScope.launch {
            val handle =
                try {
                    ActiveWearWidgetHandle.fromParcel(handleParcel, providerName)
                } catch (e: IllegalArgumentException) {
                    Log.e(TAG, "Error deserializing ActiveWearWidgetHandleParcel", e)
                    callback?.onError()
                    return@launch
                }

            try {
                widget.onActivated(context, handle)
            } catch (e: Exception) {
                callback?.onError()
                throw e
            }
            callback?.onSuccess()
        }
    }

    override fun onDeactivated(
        handleParcel: ActiveWearWidgetHandleParcel?,
        callback: IExecutionCallback?,
    ) {
        requireNotNull(handleParcel) { "Invalid widget handle parcel." }
        mainScope.launch {
            val handle =
                try {
                    ActiveWearWidgetHandle.fromParcel(handleParcel, providerName)
                } catch (e: IllegalArgumentException) {
                    Log.e(TAG, "Error deserializing ActiveWearWidgetHandleParcel", e)
                    callback?.onError()
                    return@launch
                }

            try {
                widget.onDeactivated(context, handle)
            } catch (e: Exception) {
                callback?.onError()
                throw e
            }
            callback?.onSuccess()
        }
    }

    private companion object {
        private const val TAG = "WearWidgetProviderImpl"
    }
}
