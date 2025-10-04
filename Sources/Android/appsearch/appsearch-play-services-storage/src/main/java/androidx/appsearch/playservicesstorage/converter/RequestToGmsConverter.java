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

package androidx.appsearch.playservicesstorage.converter;

import androidx.annotation.RestrictTo;
import androidx.appsearch.app.GenericDocument;
import androidx.appsearch.app.GetByDocumentIdRequest;
import androidx.appsearch.app.PutDocumentsRequest;
import androidx.appsearch.app.RemoveByDocumentIdRequest;
import androidx.appsearch.app.ReportUsageRequest;
import androidx.core.util.Preconditions;

import org.jspecify.annotations.NonNull;

import java.util.List;
import java.util.Map;

/**
 * Translates between Gms and Jetpack versions of requests.

 */
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
public final class RequestToGmsConverter {
    private RequestToGmsConverter() {
    }

    /**
     * Translates a jetpack {@link PutDocumentsRequest} into a Gms
     * {@link com.google.android.gms.appsearch.PutDocumentsRequest}.
     */
    public static com.google.android.gms.appsearch.@NonNull PutDocumentsRequest
            toGmsPutDocumentsRequest(@NonNull PutDocumentsRequest jetpackRequest) {
        Preconditions.checkNotNull(jetpackRequest);
        com.google.android.gms.appsearch.PutDocumentsRequest.Builder gmsBuilder =
                new com.google.android.gms.appsearch.PutDocumentsRequest.Builder();
        // Convert normal generic documents.
        for (GenericDocument jetpackDocument : jetpackRequest.getGenericDocuments()) {
            gmsBuilder.addGenericDocuments(
                    GenericDocumentToGmsConverter.toGmsGenericDocument(jetpackDocument));
        }
        // Convert taken action generic documents.
        for (GenericDocument jetpackTakenActionGenericDocument :
                jetpackRequest.getTakenActionGenericDocuments()) {
            gmsBuilder.addTakenActionGenericDocuments(
                    GenericDocumentToGmsConverter.toGmsGenericDocument(
                            jetpackTakenActionGenericDocument)
            );
        }
        return gmsBuilder.build();
    }

    /**
     * Translates a jetpack {@link GetByDocumentIdRequest} into a Gms
     * {@link com.google.android.gms.appsearch.GetByDocumentIdRequest}.
     */
    public static com.google.android.gms.appsearch.@NonNull GetByDocumentIdRequest
            toGmsGetByDocumentIdRequest(@NonNull GetByDocumentIdRequest jetpackRequest) {
        Preconditions.checkNotNull(jetpackRequest);
        com.google.android.gms.appsearch.GetByDocumentIdRequest.Builder gmsBuilder =
                new com.google.android.gms.appsearch.GetByDocumentIdRequest.Builder(
                        jetpackRequest.getNamespace())
                        .addIds(jetpackRequest.getIds());
        for (Map.Entry<String, List<String>> projection :
                jetpackRequest.getProjections().entrySet()) {
            gmsBuilder.addProjection(projection.getKey(), projection.getValue());
        }
        return gmsBuilder.build();
    }

    /**
     * Translates a jetpack {@link RemoveByDocumentIdRequest} into a Gms
     * {@link com.google.android.gms.appsearch.RemoveByDocumentIdRequest}.
     */
    public static com.google.android.gms.appsearch.@NonNull RemoveByDocumentIdRequest
            toGmsRemoveByDocumentIdRequest(@NonNull RemoveByDocumentIdRequest jetpackRequest) {
        Preconditions.checkNotNull(jetpackRequest);
        return new com.google.android.gms.appsearch.RemoveByDocumentIdRequest.Builder(
                jetpackRequest.getNamespace())
                .addIds(jetpackRequest.getIds())
                .build();
    }

    /**
     * Translates a jetpack {@link androidx.appsearch.app.ReportUsageRequest} into a
     * Gms
     * {@link com.google.android.gms.appsearch.ReportUsageRequest}.
     */
    public static com.google.android.gms.appsearch.@NonNull ReportUsageRequest
            toGmsReportUsageRequest(@NonNull ReportUsageRequest jetpackRequest) {
        Preconditions.checkNotNull(jetpackRequest);
        return new com.google.android.gms.appsearch.ReportUsageRequest.Builder(
                jetpackRequest.getNamespace(), jetpackRequest.getDocumentId())
                .setUsageTimestampMillis(jetpackRequest.getUsageTimestampMillis())
                .build();
    }
}
