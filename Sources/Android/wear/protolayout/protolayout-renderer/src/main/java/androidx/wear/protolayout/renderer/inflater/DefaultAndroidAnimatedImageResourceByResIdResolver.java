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

package androidx.wear.protolayout.renderer.inflater;

import android.content.res.Resources;
import android.graphics.drawable.Drawable;

import androidx.wear.protolayout.proto.ResourceProto.AndroidAnimatedImageResourceByResId;
import androidx.wear.protolayout.proto.ResourceProto.AnimatedImageFormat;
import androidx.wear.protolayout.renderer.inflater.ResourceResolvers.AndroidAnimatedImageResourceByResIdResolver;
import androidx.wear.protolayout.renderer.inflater.ResourceResolvers.ResourceAccessException;

import org.jspecify.annotations.NonNull;

/** Resource resolver for Android animated resources. */
public class DefaultAndroidAnimatedImageResourceByResIdResolver
        implements AndroidAnimatedImageResourceByResIdResolver {

    private final @NonNull Resources mAndroidResources;

    /**
     * Constructor.
     *
     * @param androidResources An Android Resources instance for the tile service's package. This is
     *     normally obtained from {@code PackageManager#getResourcesForApplication}.
     */
    public DefaultAndroidAnimatedImageResourceByResIdResolver(@NonNull Resources androidResources) {
        this.mAndroidResources = androidResources;
    }

    @Override
    public @NonNull Drawable getDrawableOrThrow(
            @NonNull AndroidAnimatedImageResourceByResId resource) throws ResourceAccessException {
        if (resource.getAnimatedImageFormat() == AnimatedImageFormat.ANIMATED_IMAGE_FORMAT_AVD) {
            return mAndroidResources.getDrawable(resource.getResourceId(), /* theme= */ null);
        }

        throw new ResourceAccessException("Unsupported animated image format");
    }
}
