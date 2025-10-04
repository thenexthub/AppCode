/*
 * Copyright 2023 The Android Open Source Project
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

package androidx.credentials.provider;

import static com.google.common.truth.Truth.assertThat;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import android.content.Context;
import android.content.Intent;
import android.content.pm.SigningInfo;
import android.credentials.CredentialOption;
import android.os.Build;
import android.os.Bundle;
import android.service.credentials.CallingAppInfo;

import androidx.credentials.CreatePasswordResponse;
import androidx.credentials.GetCredentialResponse;
import androidx.credentials.PasswordCredential;
import androidx.credentials.TestUtilsKt;
import androidx.credentials.exceptions.CreateCredentialInterruptedException;
import androidx.credentials.exceptions.GetCredentialInterruptedException;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.SdkSuppress;
import androidx.test.filters.SmallTest;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.ArrayList;
import java.util.Collections;

@RunWith(AndroidJUnit4.class)
@SmallTest
@SdkSuppress(minSdkVersion = 34, codeName = "UpsideDownCake")
public class PendingIntentHandlerApi34JavaTest {
    private static final Intent BLANK_INTENT = new Intent();

    private static final android.credentials.CredentialOption
            GET_CREDENTIAL_OPTION = new CredentialOption.Builder(
            "type", new Bundle(), new Bundle())
            .build();

    private static final android.service.credentials.GetCredentialRequest
            GET_CREDENTIAL_REQUEST = new android.service.credentials.GetCredentialRequest(
                    new CallingAppInfo(
                            "package_name", new SigningInfo()), new ArrayList<>(
                                    Collections.singleton(GET_CREDENTIAL_OPTION)));

    private static final int BIOMETRIC_AUTHENTICATOR_TYPE = 1;

    private static final int BIOMETRIC_AUTHENTICATOR_ERROR_CODE = 5;

    private static final String BIOMETRIC_AUTHENTICATOR_ERROR_MSG = "error";

    private final Context mContext = InstrumentationRegistry.getInstrumentation().getContext();

    @Test
    public void test_retrieveProviderCreateCredReqWithSuccessBpAuthJetpack_retrieveJetpackResult() {
        for (int jetpackResult :
                AuthenticationResult.Companion
                        .getBiometricFrameworkToJetpackResultMap$credentials_release().values()) {
            BiometricPromptResult biometricPromptResult =
                    new BiometricPromptResult(new AuthenticationResult(jetpackResult));
            android.service.credentials.CreateCredentialRequest request =
                    TestUtilsKt.setUpCreatePasswordRequest();
            Intent intent = prepareIntentWithCreateRequest(
                    request,
                    biometricPromptResult);

            ProviderCreateCredentialRequest retrievedRequest =
                    PendingIntentHandler.retrieveProviderCreateCredentialRequest(intent);

            assertNotNull(request);
            TestUtilsKt.equals(request, retrievedRequest);
            assertNotNull(biometricPromptResult.getAuthenticationResult());
            assertEquals(retrievedRequest.getBiometricPromptResult().getAuthenticationResult()
                    .getAuthenticationType(), jetpackResult);
        }
    }

    @Test
    public void test_retrieveProviderGetCredReqWithSuccessBpAuthJetpack_retrieveJetpackResult() {
        for (int jetpackResult :
                AuthenticationResult.Companion
                        .getBiometricFrameworkToJetpackResultMap$credentials_release().values()) {
            BiometricPromptResult biometricPromptResult =
                    new BiometricPromptResult(new AuthenticationResult(jetpackResult));
            Intent intent = prepareIntentWithGetRequest(GET_CREDENTIAL_REQUEST,
                    biometricPromptResult);

            ProviderGetCredentialRequest retrievedRequest =
                    PendingIntentHandler.retrieveProviderGetCredentialRequest(intent);

            assertNotNull(retrievedRequest);
            TestUtilsKt.equals(GET_CREDENTIAL_REQUEST, retrievedRequest);
            assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
            assertEquals(retrievedRequest.getBiometricPromptResult().getAuthenticationResult()
                    .getAuthenticationType(), jetpackResult);
        }
    }

    @Test
    public void test_retrieveProviderCreateCredReqWithSuccessBpAuthFramework_resultConverted() {
        for (int frameworkResult :
                AuthenticationResult.Companion
                        .getBiometricFrameworkToJetpackResultMap$credentials_release().keySet()) {
            BiometricPromptResult biometricPromptResult =
                    new BiometricPromptResult(
                            AuthenticationResult.Companion.createFrom$credentials_release(
                                    frameworkResult,
                                    /*isFrameworkBiometricPrompt=*/true
                            ));
            android.service.credentials.CreateCredentialRequest request =
                    TestUtilsKt.setUpCreatePasswordRequest();
            int expectedResult =
                    AuthenticationResult.Companion
                            .getBiometricFrameworkToJetpackResultMap$credentials_release()
                            .get(frameworkResult);
            Intent intent = prepareIntentWithCreateRequest(
                    request,
                    biometricPromptResult);

            ProviderCreateCredentialRequest retrievedRequest =
                    PendingIntentHandler.retrieveProviderCreateCredentialRequest(intent);

            assertNotNull(request);
            TestUtilsKt.equals(request, retrievedRequest);
            assertNotNull(biometricPromptResult.getAuthenticationResult());
            assertEquals(retrievedRequest.getBiometricPromptResult().getAuthenticationResult()
                    .getAuthenticationType(), expectedResult);
        }
    }

    @Test
    public void test_retrieveProviderGetCredReqWithSuccessBpAuthFramework_resultConverted() {
        for (int frameworkResult :
                AuthenticationResult.Companion
                        .getBiometricFrameworkToJetpackResultMap$credentials_release().keySet()) {
            BiometricPromptResult biometricPromptResult =
                    new BiometricPromptResult(
                            AuthenticationResult.Companion.createFrom$credentials_release(
                                    frameworkResult,
                                    /*isFrameworkBiometricPrompt=*/true
                            ));
            int expectedResult =
                    AuthenticationResult.Companion
                            .getBiometricFrameworkToJetpackResultMap$credentials_release()
                            .get(frameworkResult);
            Intent intent = prepareIntentWithGetRequest(GET_CREDENTIAL_REQUEST,
                    biometricPromptResult);

            ProviderGetCredentialRequest retrievedRequest =
                    PendingIntentHandler.retrieveProviderGetCredentialRequest(intent);

            assertNotNull(retrievedRequest);
            TestUtilsKt.equals(GET_CREDENTIAL_REQUEST, retrievedRequest);
            assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
            assertEquals(retrievedRequest.getBiometricPromptResult().getAuthenticationResult()
                    .getAuthenticationType(), expectedResult);
        }
    }


    @Test
    public void test_retrieveProviderCreateCredReqWithFailureBpAuthJetpack_retrieveJetpackError() {
        for (int jetpackError :
                AuthenticationError.Companion
                        .getBiometricFrameworkToJetpackErrorMap$credentials_release().values()) {
            BiometricPromptResult biometricPromptResult =
                    new BiometricPromptResult(
                            new AuthenticationError(
                                    jetpackError,
                                    BIOMETRIC_AUTHENTICATOR_ERROR_MSG));
            android.service.credentials.CreateCredentialRequest request =
                    TestUtilsKt.setUpCreatePasswordRequest();
            Intent intent = prepareIntentWithCreateRequest(
                    request, biometricPromptResult);

            ProviderCreateCredentialRequest retrievedRequest = PendingIntentHandler
                    .retrieveProviderCreateCredentialRequest(intent);

            assertNotNull(retrievedRequest);
            TestUtilsKt.equals(request, retrievedRequest);
            assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
            assertNotNull(retrievedRequest.getBiometricPromptResult().getAuthenticationError());
            assertEquals(retrievedRequest.getBiometricPromptResult().getAuthenticationError()
                    .getErrorCode(), jetpackError);
        }
    }

    @Test
    public void test_retrieveProviderGetCredReqWithFailureBpAuthJetpack_retrieveJetpackError() {
        for (int jetpackError :
                AuthenticationError.Companion
                        .getBiometricFrameworkToJetpackErrorMap$credentials_release().values()) {
            BiometricPromptResult biometricPromptResult = new BiometricPromptResult(
                    new AuthenticationError(
                            jetpackError,
                            BIOMETRIC_AUTHENTICATOR_ERROR_MSG));
            Intent intent = prepareIntentWithGetRequest(GET_CREDENTIAL_REQUEST,
                    biometricPromptResult);

            ProviderGetCredentialRequest retrievedRequest = PendingIntentHandler
                    .retrieveProviderGetCredentialRequest(intent);

            assertNotNull(retrievedRequest);
            TestUtilsKt.equals(GET_CREDENTIAL_REQUEST, retrievedRequest);
            assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
            assertNotNull(retrievedRequest.getBiometricPromptResult().getAuthenticationError());
            assertEquals(
                    retrievedRequest.getBiometricPromptResult().getAuthenticationError()
                            .getErrorCode(), jetpackError);
        }
    }

    @Test
    public void test_retrieveProviderCreateCredReqWithFailureBpAuthFramework_errorConverted() {
        for (int frameworkError :
                AuthenticationError.Companion
                        .getBiometricFrameworkToJetpackErrorMap$credentials_release().keySet()) {
            BiometricPromptResult biometricPromptResult =
                    new BiometricPromptResult(
                            AuthenticationError.Companion.createFrom$credentials_release(
                                    frameworkError, BIOMETRIC_AUTHENTICATOR_ERROR_MSG,
                                    /*isFrameworkBiometricPrompt=*/true
                            ));
            android.service.credentials.CreateCredentialRequest request =
                    TestUtilsKt.setUpCreatePasswordRequest();
            int expectedErrorCode =
                    AuthenticationError.Companion
                            .getBiometricFrameworkToJetpackErrorMap$credentials_release()
                            .get(frameworkError);
            Intent intent = prepareIntentWithCreateRequest(
                    request, biometricPromptResult);

            ProviderCreateCredentialRequest retrievedRequest = PendingIntentHandler
                    .retrieveProviderCreateCredentialRequest(intent);

            assertNotNull(retrievedRequest);
            TestUtilsKt.equals(request, retrievedRequest);
            assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
            assertNotNull(retrievedRequest.getBiometricPromptResult().getAuthenticationError());
            assertEquals(retrievedRequest.getBiometricPromptResult().getAuthenticationError()
                    .getErrorCode(), expectedErrorCode);
        }
    }

    @Test
    public void test_retrieveProviderGetCredReqWithFailureBpAuthFramework_correctlyConvertedErr() {
        for (int frameworkError :
                AuthenticationError.Companion
                        .getBiometricFrameworkToJetpackErrorMap$credentials_release().keySet()) {
            BiometricPromptResult biometricPromptResult = new BiometricPromptResult(
                    AuthenticationError.Companion.createFrom$credentials_release(
                            frameworkError, BIOMETRIC_AUTHENTICATOR_ERROR_MSG,
                            /*isFrameworkBiometricPrompt=*/true
                    ));
            Intent intent = prepareIntentWithGetRequest(GET_CREDENTIAL_REQUEST,
                    biometricPromptResult);
            int expectedErrorCode =
                    AuthenticationError.Companion
                            .getBiometricFrameworkToJetpackErrorMap$credentials_release()
                            .get(frameworkError);

            ProviderGetCredentialRequest retrievedRequest = PendingIntentHandler
                    .retrieveProviderGetCredentialRequest(intent);

            assertNotNull(retrievedRequest);
            TestUtilsKt.equals(GET_CREDENTIAL_REQUEST, retrievedRequest);
            assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
            assertNotNull(retrievedRequest.getBiometricPromptResult().getAuthenticationError());
            assertEquals(
                    retrievedRequest.getBiometricPromptResult().getAuthenticationError()
                            .getErrorCode(), expectedErrorCode);
        }
    }

    @Test
    public void test_setGetCreateCredentialException() {
        Intent intent = new Intent();
        CreateCredentialInterruptedException initialException =
                new CreateCredentialInterruptedException("message");

        PendingIntentHandler.setCreateCredentialException(intent, initialException);

        android.credentials.CreateCredentialException finalException =
                IntentHandlerConverters.getCreateCredentialException(intent);
        assertThat(finalException).isNotNull();
        assertThat(finalException.getMessage()).isEqualTo(initialException.getMessage());
        assertThat(finalException.getType()).isEqualTo(initialException.getType());
    }

    @Test
    public void test_setGetCreateCredentialException_nullWhenEmptyIntent() {
        assertThat(IntentHandlerConverters.getCreateCredentialException(BLANK_INTENT)).isNull();
    }

    @Test
    public void test_credentialException() {
        Intent intent = new Intent();
        GetCredentialInterruptedException initialException =
                new GetCredentialInterruptedException("message");

        PendingIntentHandler.setGetCredentialException(intent, initialException);

        android.credentials.GetCredentialException finalException =
                IntentHandlerConverters.getGetCredentialException(intent);
        assertThat(finalException).isNotNull();
        assertThat(finalException.getMessage()).isEqualTo(initialException.getMessage());
        assertThat(finalException.getType()).isEqualTo(initialException.getType());
    }

    @Test
    public void test_credentialException_nullWhenEmptyIntent() {
        assertThat(IntentHandlerConverters.getGetCredentialException(BLANK_INTENT)).isNull();
    }

    @Test
    public void test_beginGetResponse() {
        Intent intent = new Intent();
        BeginGetCredentialResponse initialResponse =
                new BeginGetCredentialResponse.Builder().build();

        PendingIntentHandler.setBeginGetCredentialResponse(intent, initialResponse);

        BeginGetCredentialResponse finalResponse =
                IntentHandlerConverters.getBeginGetResponse(intent);
        assertThat(finalResponse).isNotNull();
        TestUtilsKt.assertEquals(mContext, finalResponse, initialResponse);
    }

    @Test
    public void test_beginGetResponse_throwsWhenEmptyIntent() {
        assertThat(IntentHandlerConverters.getBeginGetResponse(BLANK_INTENT))
                .isNull();
    }

    @Test
    public void test_credentialResponse() {
        Intent intent = new Intent();
        PasswordCredential credential = new PasswordCredential("a", "b");
        GetCredentialResponse initialResponse = new GetCredentialResponse(credential);

        PendingIntentHandler.setGetCredentialResponse(intent, initialResponse);

        android.credentials.GetCredentialResponse finalResponse =
                IntentHandlerConverters.getGetCredentialResponse(intent);
        assertThat(finalResponse).isNotNull();
        TestUtilsKt.assertEquals(finalResponse, initialResponse);
    }

    @Test
    public void test_credentialResponse_nullWhenEmptyIntent() {
        assertThat(IntentHandlerConverters.getGetCredentialResponse(BLANK_INTENT))
                .isNull();
    }

    @Test
    public void test_retrieveProviderCreateCredReqWithSuccessfulBpAuth() {
        BiometricPromptResult biometricPromptResult = new BiometricPromptResult(
                new AuthenticationResult(BIOMETRIC_AUTHENTICATOR_TYPE));

        android.service.credentials.CreateCredentialRequest request =
                TestUtilsKt.setUpCreatePasswordRequest();

        Intent intent = prepareIntentWithCreateRequest(request,
                biometricPromptResult);

        ProviderCreateCredentialRequest retrievedRequest = PendingIntentHandler
                .retrieveProviderCreateCredentialRequest(intent);

        assertNotNull(retrievedRequest);
        TestUtilsKt.equals(request, retrievedRequest);
        assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
    }

    @Test
    public void test_retrieveProviderCreateCredReqWithFailureBpAuth() {
        BiometricPromptResult biometricPromptResult =
                new BiometricPromptResult(
                        new AuthenticationError(
                                BIOMETRIC_AUTHENTICATOR_ERROR_CODE,
                                BIOMETRIC_AUTHENTICATOR_ERROR_MSG));
        android.service.credentials.CreateCredentialRequest request =
                TestUtilsKt.setUpCreatePasswordRequest();
        Intent intent = prepareIntentWithCreateRequest(
                request, biometricPromptResult);

        ProviderCreateCredentialRequest retrievedRequest = PendingIntentHandler
                .retrieveProviderCreateCredentialRequest(intent);

        assertNotNull(retrievedRequest);
        TestUtilsKt.equals(request, retrievedRequest);
        assertEquals(biometricPromptResult, retrievedRequest.getBiometricPromptResult());
    }

    @Test
    public void test_retrieveProviderGetCredReqWithSuccessfulBpAuth() {
        BiometricPromptResult biometricPromptResult = new BiometricPromptResult(
                new AuthenticationResult(
                BIOMETRIC_AUTHENTICATOR_TYPE));
        Intent intent = prepareIntentWithGetRequest(GET_CREDENTIAL_REQUEST,
                biometricPromptResult);

        ProviderGetCredentialRequest request = PendingIntentHandler
                .retrieveProviderGetCredentialRequest(intent);

        assertNotNull(request);
        TestUtilsKt.equals(GET_CREDENTIAL_REQUEST, request);
        assertEquals(biometricPromptResult, request.getBiometricPromptResult());
    }

    @Test
    public void test_retrieveProviderGetCredReqWithFailingBpAuth() {
        BiometricPromptResult biometricPromptResult = new BiometricPromptResult(
                new AuthenticationError(
                        BIOMETRIC_AUTHENTICATOR_ERROR_CODE,
                        BIOMETRIC_AUTHENTICATOR_ERROR_MSG));
        Intent intent = prepareIntentWithGetRequest(GET_CREDENTIAL_REQUEST,
                biometricPromptResult);

        ProviderGetCredentialRequest request = PendingIntentHandler
                .retrieveProviderGetCredentialRequest(intent);

        assertNotNull(request);
        TestUtilsKt.equals(GET_CREDENTIAL_REQUEST, request);
        assertEquals(biometricPromptResult, request.getBiometricPromptResult());
    }

    private Intent prepareIntentWithGetRequest(
            android.service.credentials.GetCredentialRequest request,
            BiometricPromptResult biometricPromptResult
    ) {
        Intent intent = new Intent();
        intent.putExtra(CredentialProviderService
                        .EXTRA_GET_CREDENTIAL_REQUEST, request);
        prepareIntentWithBiometricResult(intent, biometricPromptResult);
        return intent;
    }

    private Intent prepareIntentWithCreateRequest(
            android.service.credentials.CreateCredentialRequest request,
            BiometricPromptResult biometricPromptResult) {
        Intent intent = new Intent();
        intent.putExtra(CredentialProviderService.EXTRA_CREATE_CREDENTIAL_REQUEST,
                request);
        prepareIntentWithBiometricResult(intent, biometricPromptResult);
        return intent;
    }

    private void prepareIntentWithBiometricResult(Intent intent,
            BiometricPromptResult biometricPromptResult) {
        String buildId = Build.ID;
        if (biometricPromptResult.isSuccessful()) {
            assertNotNull(biometricPromptResult.getAuthenticationResult());
            String extraResultKey = AuthenticationResult.EXTRA_BIOMETRIC_AUTH_RESULT_TYPE;
            intent.putExtra(extraResultKey,
                    biometricPromptResult.getAuthenticationResult().getAuthenticationType());
        } else {
            assertNotNull(biometricPromptResult.getAuthenticationError());
            String extraErrorKey = AuthenticationError.EXTRA_BIOMETRIC_AUTH_ERROR;
            String extraErrorMessageKey = AuthenticationError.EXTRA_BIOMETRIC_AUTH_ERROR_MESSAGE;
            intent.putExtra(extraErrorKey,
                    biometricPromptResult.getAuthenticationError().getErrorCode());
            intent.putExtra(extraErrorMessageKey,
                    biometricPromptResult.getAuthenticationError().getErrorMsg());
        }
    }

    @Test
    public void test_createCredentialCredentialResponse() {
        Intent intent = new Intent();
        CreatePasswordResponse initialResponse = new CreatePasswordResponse();

        PendingIntentHandler.setCreateCredentialResponse(intent, initialResponse);

        android.credentials.CreateCredentialResponse finalResponse =
                IntentHandlerConverters.getCreateCredentialCredentialResponse(
                        intent);
        assertThat(finalResponse).isNotNull();
        TestUtilsKt.assertEquals(finalResponse, initialResponse);
    }

    @Test
    public void test_createCredentialCredentialResponse_nullWhenEmptyIntent() {
        assertThat(IntentHandlerConverters.getCreateCredentialCredentialResponse(BLANK_INTENT))
                .isNull();
    }
}
