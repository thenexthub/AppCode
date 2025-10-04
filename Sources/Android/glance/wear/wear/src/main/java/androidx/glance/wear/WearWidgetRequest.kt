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

package androidx.glance.wear

import androidx.annotation.Dimension
import androidx.annotation.RestrictTo
import androidx.annotation.RestrictTo.Scope.LIBRARY
import androidx.glance.wear.parcel.WearWidgetRequestParcel
import androidx.glance.wear.proto.WearWidgetRequestProto

/**
 * Request for widget contents.
 *
 * @property instanceId The instance id of the widget for this request. The id is created by the
 *   system and is provided when [GlanceWearWidget.onActivated] is called.
 * @property widthDp The width in dp of the container for this widget.
 * @property heightDp The height in dp of the container for this widget.
 */
public class WearWidgetRequest
@RestrictTo(LIBRARY)
public constructor(
    public val instanceId: Int,
    @Dimension(unit = Dimension.DP) public val widthDp: Float,
    @Dimension(unit = Dimension.DP) public val heightDp: Float,
) {

    /** Convert this request to [WearWidgetRequestParcel]. */
    @RestrictTo(LIBRARY)
    public fun toParcel(): WearWidgetRequestParcel {
        val requestProto =
            WearWidgetRequestProto(
                instance_id = instanceId,
                width_dp = widthDp,
                height_dp = heightDp,
            )
        return WearWidgetRequestParcel().apply { payload = requestProto.encode() }
    }

    internal companion object {
        fun fromParcel(requestParcel: WearWidgetRequestParcel): WearWidgetRequest {
            val requestProto = WearWidgetRequestProto.ADAPTER.decode(requestParcel.payload)
            return WearWidgetRequest(
                instanceId = requestProto.instance_id,
                widthDp = requestProto.width_dp,
                heightDp = requestProto.height_dp,
            )
        }

        private const val TAG = "WearWidgetRequest"
    }
}
