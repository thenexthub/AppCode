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

package androidx.car.app.features;

import static androidx.annotation.RestrictTo.Scope.LIBRARY;

import android.content.Context;
import android.content.pm.PackageManager;

import androidx.annotation.RestrictTo;
import androidx.annotation.StringDef;

import org.jspecify.annotations.NonNull;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Class for checking for whether car related features are available on the system.
 */
public final class CarFeatures {

    /**
     * Flag value to check whether or not audio is allowed to play while driving.
     */
    public static final String FEATURE_BACKGROUND_AUDIO_WHILE_DRIVING =
            "com.android.car.background_audio_while_driving";

    /**
     * Flag value to check whether the system supports apps that use the
     * CarAppLibrary Media category
     */
    public static final String FEATURE_CAR_APP_LIBRARY_MEDIA =
            "android.software.car.templates_host.media";

    @RestrictTo(LIBRARY)
    @Retention(RetentionPolicy.SOURCE)
    @StringDef({
            FEATURE_BACKGROUND_AUDIO_WHILE_DRIVING,
            FEATURE_CAR_APP_LIBRARY_MEDIA,
    })
    public @interface CarFeature {}

    // Do not instantiate
    private CarFeatures() {}

    // Create an open api surface to support future system features that may need to be
    // backported without needing to create a new api
    /**
     * Checks whether the system enables a given CarFeature.
     *
     * @param context The context with which to query for the CarFeature.
     * @param feature The CarFeature to query for.
     * @return  Whether the system enables a given CarFeature.
     */
    public static boolean isFeatureEnabled(@NonNull Context context,
            @CarFeature @NonNull String feature) {
        PackageManager packageManager = context.getPackageManager();

        return packageManager.hasSystemFeature(feature);
    }
}
