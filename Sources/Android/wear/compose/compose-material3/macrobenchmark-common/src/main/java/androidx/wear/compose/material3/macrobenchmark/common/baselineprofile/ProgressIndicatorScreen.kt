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

package androidx.wear.compose.material3.macrobenchmark.common.baselineprofile

import androidx.compose.foundation.layout.BoxScope
import androidx.compose.runtime.Composable
import androidx.wear.compose.material3.macrobenchmark.common.MacrobenchmarkScreen
import androidx.wear.compose.material3.samples.FullScreenProgressIndicatorSample
import androidx.wear.compose.material3.samples.IndeterminateProgressArcSample
import androidx.wear.compose.material3.samples.IndeterminateProgressIndicatorSample
import androidx.wear.compose.material3.samples.MediaButtonProgressIndicatorSample
import androidx.wear.compose.material3.samples.OverflowProgressIndicatorSample
import androidx.wear.compose.material3.samples.SegmentedProgressIndicatorBinarySample
import androidx.wear.compose.material3.samples.SegmentedProgressIndicatorSample
import androidx.wear.compose.material3.samples.SmallSegmentedProgressIndicatorSample
import androidx.wear.compose.material3.samples.SmallValuesProgressIndicatorSample

val ProgressIndicatorScreen =
    object : MacrobenchmarkScreen {
        override val content: @Composable BoxScope.() -> Unit
            get() = {
                FullScreenProgressIndicatorSample()
                MediaButtonProgressIndicatorSample()
                OverflowProgressIndicatorSample()
                SmallValuesProgressIndicatorSample()
                IndeterminateProgressIndicatorSample()
                IndeterminateProgressArcSample()
                SegmentedProgressIndicatorSample()
                SegmentedProgressIndicatorBinarySample()
                SmallSegmentedProgressIndicatorSample()
            }
    }
