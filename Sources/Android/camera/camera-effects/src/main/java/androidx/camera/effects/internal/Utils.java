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

package androidx.camera.effects.internal;

import android.graphics.Canvas;
import android.view.Surface;

import androidx.annotation.RequiresApi;

import org.jspecify.annotations.NonNull;

/**
 * Utility methods for overlay processing.
 */
public class Utils {

    private Utils() {
    }

    /**
     * Locks the Canvas on the given Surface.
     *
     * <p>This method calls {@link Surface#lockCanvas}.
     */
    public static @NonNull Canvas lockCanvas(@NonNull Surface surface) {
        // TODO(b/186120366): Investigate how widespread the lockHardwareCanvas is and re-enable
        //  it when possible.
        return surface.lockCanvas(null);
    }

    @RequiresApi(23)
    static class Api23Impl {
        private Api23Impl() {
            // This class is not instantiable.
        }

        static Canvas lockHardwareCanvas(Surface surface) {
            return surface.lockHardwareCanvas();
        }
    }
}
