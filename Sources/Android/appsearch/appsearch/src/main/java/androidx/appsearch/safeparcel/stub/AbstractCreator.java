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

package androidx.appsearch.safeparcel.stub;

import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.RestrictTo;
import androidx.appsearch.safeparcel.SafeParcelable;

import org.jspecify.annotations.NonNull;

/**
 * An abstract class providing a default {@link #writeToParcel(SafeParcelable, Parcel, int)} to
 * throw {@link UnsupportedOperationException}.
 *
 * <p>All the stub Creator classes in the package can extend this as the parent class, so they
 * don't need to implement {@code writeToParcel} individually.
 */
// @exportToFramework:skipFile()
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
abstract class AbstractCreator<T> implements Parcelable.Creator<T> {
    @Override
    public T createFromParcel(Parcel var1) {
        // This is here only for code sync purpose.
        throw new UnsupportedOperationException("createFromParcel is not implemented and should "
                + "not be used.");
    }

    @Override
    public T[] newArray(int var1) {
        // This is here only for code sync purpose.
        throw new UnsupportedOperationException("newArray is not implemented and should "
                + "not be used.");
    }

    public static void writeToParcel(
            @NonNull SafeParcelable safeParcelable,
            @NonNull Parcel parcel,
            int flags) {
        // This is here only for code sync purpose.
        throw new UnsupportedOperationException(
                "writeToParcel is not implemented and should not be used.");
    }
}
