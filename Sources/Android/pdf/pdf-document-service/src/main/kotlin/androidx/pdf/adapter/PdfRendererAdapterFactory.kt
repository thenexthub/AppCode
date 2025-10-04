/*
 * Copyright 2024 The Android Open Source Project
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

package androidx.pdf.adapter

import android.os.Build
import android.os.ParcelFileDescriptor
import android.os.ext.SdkExtensions
import androidx.annotation.RestrictTo

@RestrictTo(RestrictTo.Scope.LIBRARY)
public interface PdfDocumentRendererFactory {
    public fun create(pfd: ParcelFileDescriptor, password: String?): PdfDocumentRenderer
}

@Suppress("ObsoleteSdkInt")
@RestrictTo(RestrictTo.Scope.LIBRARY)
internal class PdfDocumentRendererFactoryImpl : PdfDocumentRendererFactory {
    override fun create(pfd: ParcelFileDescriptor, password: String?): PdfDocumentRenderer {
        return if (Build.VERSION.SDK_INT >= 35) {
            PdfDocumentRendererAdapter(pfd, password.orEmpty())
        } else if (SdkExtensions.getExtensionVersion(Build.VERSION_CODES.S) >= 13) {
            PdfDocumentRendererPreVAdapter(pfd, password.orEmpty())
        } else {
            throw UnsupportedOperationException("Operation supported above S")
        }
    }
}
