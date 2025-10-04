/*
 * Copyright 2022 The Android Open Source Project
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

package androidx.camera.video;

import android.location.Location;

import com.google.auto.value.AutoValue;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

/** A fake implementation of {@link OutputOptions}. */
// Java is used because @AutoValue is required.
public class FakeOutputOptions extends OutputOptions {

    private FakeOutputOptions(@NonNull FakeOutputOptionsInternal fakeOutputOptionsInternal) {
        super(fakeOutputOptionsInternal);
    }

    /** The builder of the {@link FakeOutputOptions} object. */
    public static final class Builder extends OutputOptions.Builder<FakeOutputOptions, Builder> {

        /** Creates a builder of the {@link FakeOutputOptions}. */
        public Builder() {
            super(new AutoValue_FakeOutputOptions_FakeOutputOptionsInternal.Builder());
        }

        /** Builds the {@link FakeOutputOptions} instance. */
        @Override
        public @NonNull FakeOutputOptions build() {
            return new FakeOutputOptions(
                    ((FakeOutputOptionsInternal.Builder) mRootInternalBuilder).build());
        }
    }

    @AutoValue
    abstract static class FakeOutputOptionsInternal extends OutputOptions.OutputOptionsInternal {
        @Override
        abstract @Nullable Location getLocation();

        @AutoValue.Builder
        abstract static class Builder extends OutputOptions.OutputOptionsInternal.Builder<Builder> {

            @SuppressWarnings("NullableProblems") // Nullable problem in AutoValue generated class
            @Override
            abstract @NonNull FakeOutputOptionsInternal build();
        }
    }
}
