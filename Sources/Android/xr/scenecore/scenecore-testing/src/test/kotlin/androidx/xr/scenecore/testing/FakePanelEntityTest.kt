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

package androidx.xr.scenecore.testing

import android.app.Activity
import android.view.View
import androidx.xr.scenecore.runtime.Dimensions
import androidx.xr.scenecore.runtime.PerceivedResolutionResult
import androidx.xr.scenecore.runtime.PixelDimensions
import com.google.common.truth.Truth.assertThat
import kotlin.math.roundToInt
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.Robolectric
import org.robolectric.RobolectricTestRunner
import org.robolectric.shadows.ShadowLooper

@RunWith(RobolectricTestRunner::class)
class FakePanelEntityTest {
    private var view: View =
        View(Robolectric.buildActivity(Activity::class.java).create().start().get())
    private lateinit var underTest: FakePanelEntity

    @Before
    fun setUp() {
        underTest = FakePanelEntity(view)
    }

    @Test
    fun getSizeInPixels_returnsDefaultValue() {
        assertThat(underTest.sizeInPixels).isEqualTo(PixelDimensions(1152, 1152))
    }

    @Test
    fun getCornerRadius_setNegativeCornerRadius_returnLastValidCornerRadius() {
        // Default value
        check(underTest.cornerRadius == 32.0f)

        underTest.cornerRadius = -64.0f

        assertThat(underTest.cornerRadius).isEqualTo(32.0f)
    }

    @Test
    fun getSize_setNegativeSize_returnLastValidSize() {
        // Default value
        val defaultSize = Dimensions(1.0f, 1.0f, 0.0f)
        assertThat(underTest.size).isApproximatelyEqualTo(defaultSize)

        // Set a negative width value.
        underTest.size = Dimensions(-1280.0f, 720.0f, 1.0f)

        assertThat(underTest.size).isApproximatelyEqualTo(defaultSize)

        // Set a negative height value.
        underTest.size = Dimensions(1280.0f, -720.0f, 1.0f)

        assertThat(underTest.size).isApproximatelyEqualTo(defaultSize)

        // Set a negative depth value.
        underTest.size = Dimensions(1280.0f, 720.0f, -1.0f)

        assertThat(underTest.size).isApproximatelyEqualTo(defaultSize)
    }

    @Test
    fun getSize_setNonZeroPositiveDepthSize_returnsZeroDepthSize() {
        // Default value
        val defaultSize = Dimensions(1.0f, 1.0f, 0.0f)
        assertThat(underTest.size).isApproximatelyEqualTo(defaultSize)

        // Set a non-zero positive depth value.
        underTest.size = Dimensions(2.0f, 3.0f, 1.0f)

        // Value set successfully but returned with depth set to 0.
        assertThat(underTest.size).isApproximatelyEqualTo(Dimensions(2.0f, 3.0f, 0.0f))
    }

    @Test
    fun getPerceivedResolutionResult_withDifferentResults() {
        // Default value
        assertThat(underTest.getPerceivedResolution())
            .isInstanceOf(PerceivedResolutionResult.InvalidCameraView::class.java)

        // Set the EntityTooClose result.
        underTest.setPerceivedResolution(PerceivedResolutionResult.EntityTooClose())

        assertThat(underTest.getPerceivedResolution())
            .isInstanceOf(PerceivedResolutionResult.EntityTooClose::class.java)

        // Set the Success result.
        underTest.setPerceivedResolution(
            PerceivedResolutionResult.Success(PixelDimensions(640, 480))
        )
        val successResult = underTest.getPerceivedResolution() as PerceivedResolutionResult.Success

        assertThat(successResult).isInstanceOf(PerceivedResolutionResult.Success::class.java)
        assertThat(successResult.perceivedResolution).isEqualTo(PixelDimensions(640, 480))
    }

    @Test
    fun setSizeInPixels_returnsCorrectViewSize() {
        val expectedWidth = 1280
        val expectedHeight = 720
        underTest.sizeInPixels = PixelDimensions(expectedWidth, expectedHeight)

        assertThat(underTest.sizeInPixels).isEqualTo(PixelDimensions(expectedWidth, expectedHeight))
        // Robolectric's main looper should process pending layout requests from WindowManager.
        ShadowLooper.idleMainLooper()

        assertThat(view.width).isEqualTo(expectedWidth)
        assertThat(view.height).isEqualTo(expectedHeight)
    }

    @Test
    fun setSize_returnsCorrectViewSize() {
        // Assuming default dpPerMeter = 160, density = 1.0 in Robolectric
        underTest.dpPerMeter = 160f // Ensure a known value for testing
        val density = view.context.resources.displayMetrics.density

        val widthMeters = 2.0f
        val heightMeters = 1.5f
        underTest.size = Dimensions(widthMeters, heightMeters, 0.0f)

        val expectedWidthPixels = (widthMeters * underTest.dpPerMeter * density).roundToInt()
        val expectedHeightPixels = (heightMeters * underTest.dpPerMeter * density).roundToInt()

        assertThat(underTest.size.width).isEqualTo(widthMeters)
        assertThat(underTest.size.height).isEqualTo(heightMeters)
        assertThat(underTest.sizeInPixels)
            .isEqualTo(PixelDimensions(expectedWidthPixels, expectedHeightPixels))

        ShadowLooper.idleMainLooper()

        assertThat(view.width).isEqualTo(expectedWidthPixels)
        assertThat(view.height).isEqualTo(expectedHeightPixels)
    }

    private fun assertThat(actual: Dimensions): DimensionsSubject = DimensionsSubject(actual)

    private class DimensionsSubject(private val actual: Dimensions) {
        fun isApproximatelyEqualTo(expected: Dimensions, tolerance: Float = 0.001f) {
            assertThat(actual.width).isWithin(tolerance).of(expected.width)
            assertThat(actual.height).isWithin(tolerance).of(expected.height)
            assertThat(actual.depth).isWithin(tolerance).of(expected.depth)
        }
    }
}
