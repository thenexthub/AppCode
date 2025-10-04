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

package androidx.xr.arcore.openxr

import androidx.annotation.RestrictTo
import androidx.annotation.VisibleForTesting
import androidx.xr.arcore.runtime.PerceptionRuntime
import androidx.xr.runtime.Config
import androidx.xr.runtime.Config.ConfigMode
import kotlin.time.ComparableTimeMark

/**
 * Implementation of the [androidx.xr.arcore.runtime.PerceptionRuntime] interface using OpenXR.
 *
 * @property lifecycleManager that manages the lifecycle of the OpenXR session.
 * @property perceptionManager that manages the perception capabilities of a runtime using OpenXR.
 */
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP_PREFIX)
public class OpenXrRuntime
internal constructor(
    override val lifecycleManager: OpenXrManager,
    override val perceptionManager: OpenXrPerceptionManager,
) : PerceptionRuntime {
    override fun initialize() {
        lifecycleManager.create()
    }

    override fun resume() {
        lifecycleManager.resume()
    }

    override fun pause() {
        lifecycleManager.pause()
    }

    override suspend fun update(): ComparableTimeMark? {
        return lifecycleManager.update()
    }

    override fun configure(config: Config) {
        lifecycleManager.configure(config)
    }

    override fun isSupported(configMode: ConfigMode): Boolean {
        return SUPPORTED_CONFIG_MODES.contains(configMode)
    }

    override fun destroy() {
        lifecycleManager.stop()
    }

    internal companion object {
        @VisibleForTesting
        internal val SUPPORTED_CONFIG_MODES: Set<ConfigMode> =
            setOf(
                Config.PlaneTrackingMode.DISABLED,
                Config.PlaneTrackingMode.HORIZONTAL_AND_VERTICAL,
                Config.HandTrackingMode.DISABLED,
                Config.HandTrackingMode.BOTH,
                Config.DeviceTrackingMode.DISABLED,
                Config.DeviceTrackingMode.LAST_KNOWN,
                Config.HeadTrackingMode.DISABLED,
                Config.HeadTrackingMode.LAST_KNOWN,
                Config.DepthEstimationMode.DISABLED,
                Config.DepthEstimationMode.RAW_ONLY,
                Config.DepthEstimationMode.SMOOTH_ONLY,
                Config.AnchorPersistenceMode.DISABLED,
                Config.AnchorPersistenceMode.LOCAL,
                Config.FaceTrackingMode.DISABLED,
                Config.FaceTrackingMode.USER,
                Config.GeospatialMode.DISABLED,
                Config.EyeTrackingMode.DISABLED,
                Config.EyeTrackingMode.COARSE_TRACKING,
                Config.EyeTrackingMode.FINE_TRACKING,
                Config.EyeTrackingMode.COARSE_AND_FINE_TRACKING,
            )
    }
}
