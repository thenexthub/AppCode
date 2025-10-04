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

package androidx.compose.remote.creation.compose.capture

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.BlendMode
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Rect
import android.graphics.Typeface
import androidx.compose.remote.core.CoreDocument
import androidx.compose.remote.core.Operation
import androidx.compose.remote.core.Profiles
import androidx.compose.remote.core.RemoteContext
import androidx.compose.remote.core.operations.PaintData
import androidx.compose.remote.creation.compose.SCREENSHOT_GOLDEN_DIRECTORY
import androidx.compose.remote.creation.compose.state.RemoteBlendModeColorFilter
import androidx.compose.remote.creation.compose.state.RemoteBoolean
import androidx.compose.remote.creation.compose.state.RemoteColor
import androidx.compose.remote.creation.compose.state.RemoteFloat
import androidx.compose.remote.creation.compose.state.RemotePaint
import androidx.compose.remote.creation.compose.test.R
import androidx.compose.remote.creation.platform.AndroidxPlatformServices
import androidx.compose.remote.player.core.platform.AndroidRemoteContext
import androidx.compose.ui.geometry.Size
import androidx.test.core.app.ApplicationProvider
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.filters.MediumTest
import androidx.test.filters.SdkSuppress
import androidx.test.screenshot.AndroidXScreenshotTestRule
import androidx.test.screenshot.assertAgainstGolden
import com.google.common.truth.Truth.assertThat
import java.util.ArrayList
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

private const val WIDTH = 400
private const val HEIGHT = 400

/** Emulator-based screenshot test of [RecordingCanvas]. */
@SdkSuppress(minSdkVersion = 35, maxSdkVersion = 35)
@RunWith(AndroidJUnit4::class)
@MediumTest
class RecordingCanvasTest {
    private val context: Context = ApplicationProvider.getApplicationContext()
    @get:Rule val screenshotRule = AndroidXScreenshotTestRule(SCREENSHOT_GOLDEN_DIRECTORY)

    private val creationState =
        RemoteComposeCreationState(
            AndroidxPlatformServices(),
            1f,
            Size(WIDTH.toFloat(), HEIGHT.toFloat()),
            CoreDocument.DOCUMENT_API_LEVEL,
            Profiles.PROFILE_ANDROIDX,
        )

    private val recordingCanvas =
        RecordingCanvas(Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_8888))

    private val remoteContext = AndroidRemoteContext()

    @Before
    fun setUp() {
        recordingCanvas.setRemoteComposeCreationState(creationState)
    }

    @Test
    fun remotePaint() {
        val paint = RemotePaint()
        paint.remoteColorFilter =
            RemoteBlendModeColorFilter(RemoteColor(0xffffee70.toInt()), BlendMode.MULTIPLY)
        val bitmap = BitmapFactory.decodeResource(context.getResources(), R.drawable.android_image)
        recordingCanvas.drawBitmap(bitmap, 0f, 0f, paint)
        val document = constructDocument()
        assertScreenshot(document, "remotePaint")
    }

    @Test
    fun drawConditionally_true() {
        val flag = RemoteBoolean.createNamedRemoteBoolean("flag", true)
        val document = constructConditionalDocument(flag)
        assertScreenshot(document, "drawConditonally_true")
    }

    @Test
    fun drawConditionally_false() {
        val flag = RemoteBoolean.createNamedRemoteBoolean("flag", false)
        val document = constructConditionalDocument(flag)
        assertScreenshot(document, "drawConditonally_false")
    }

    private fun constructConditionalDocument(flag: RemoteBoolean): CoreDocument {
        recordingCanvas.drawConditionally(flag) {
            recordingCanvas.drawText(
                "True",
                10,
                80,
                Paint().apply {
                    color = Color.GREEN
                    textSize = 80f
                },
            )
        }
        recordingCanvas.drawConditionally(!flag) {
            recordingCanvas.drawText(
                "False",
                10,
                80,
                Paint().apply {
                    color = Color.RED
                    textSize = 80f
                },
            )
        }

        return constructDocument()
    }

    @Test
    fun remotePaintSetColor_constantColor() {
        val paint = RemotePaint()
        paint.remoteColor =
            RemoteColor.fromARGB(
                RemoteFloat(1f),
                RemoteFloat(0.8f),
                RemoteFloat(0.7f),
                RemoteFloat(0.5f),
            )

        recordingCanvas.usePaint(paint)

        val operations = inflateOperations()
        val paintOp = operations[operations.size - 1] as PaintData
        assertThat(paintOp.mPaintData.toString()).contains("Color(0xffccb380)")
    }

    @Test
    fun remotePaintColorInt() {
        val paint = RemotePaint()
        paint.color = Color.RED

        recordingCanvas.usePaint(paint)

        val operations = inflateOperations()
        val paintOp = operations[operations.size - 1] as PaintData
        assertThat(paintOp.mPaintData.toString()).contains("Color(0xffff0000)")
    }

    @Test
    fun paintColorInt() {
        val paint = Paint()
        paint.color = Color.RED

        recordingCanvas.usePaint(paint)

        val operations = inflateOperations()
        val paintOp = operations[operations.size - 1] as PaintData
        assertThat(paintOp.mPaintData.toString()).contains("Color(0xffff0000)")
    }

    @Test
    fun remotePaintSetColor_colorExpression() {
        val paint = RemotePaint()
        paint.remoteColor =
            RemoteColor.fromARGB(
                RemoteFloat(1f),
                RemoteFloat(0.8f),
                RemoteFloat(RemoteContext.FLOAT_CONTINUOUS_SEC),
                RemoteFloat(0.5f),
            )

        recordingCanvas.usePaint(paint)

        val operations = inflateOperations()
        val paintOp = operations[operations.size - 1] as PaintData
        assertThat(paintOp.mPaintData.toString()).contains("ColorId([45])")
    }

    @Test
    fun remotePaintSetRemoteColorFilter_constantColor() {
        val paint = RemotePaint()
        paint.remoteColorFilter =
            RemoteBlendModeColorFilter(
                RemoteColor.fromARGB(
                    RemoteFloat(1f),
                    RemoteFloat(0.8f),
                    RemoteFloat(0.7f),
                    RemoteFloat(0.5f),
                ),
                BlendMode.MULTIPLY,
            )

        recordingCanvas.usePaint(paint)

        val operations = inflateOperations()
        val paintOp = operations[operations.size - 1] as PaintData
        assertThat(paintOp.mPaintData.toString())
            .contains("ColorFilter(color=0xffccb380, mode=MULTIPLY)")
    }

    @Test
    fun remotePaintSetRemoteColorFilter_colorExpression() {
        val paint = RemotePaint()
        paint.remoteColorFilter =
            RemoteBlendModeColorFilter(
                RemoteColor.fromARGB(
                    RemoteFloat(1f),
                    RemoteFloat(0.8f),
                    RemoteFloat(RemoteContext.FLOAT_CONTINUOUS_SEC),
                    RemoteFloat(0.5f),
                ),
                BlendMode.MULTIPLY,
            )

        recordingCanvas.usePaint(paint)

        val operations = inflateOperations()
        val paintOp = operations[operations.size - 1] as PaintData
        assertThat(paintOp.mPaintData.toString())
            .contains("ColorFilterID(color=[45], mode=MULTIPLY)")
    }

    @Test
    fun remotePaintSetRemoteColorFilter_clearColorExpression() {
        val paint = RemotePaint()
        paint.remoteColorFilter =
            RemoteBlendModeColorFilter(
                RemoteColor.fromARGB(
                    RemoteFloat(1f),
                    RemoteFloat(0.8f),
                    RemoteFloat(RemoteContext.FLOAT_CONTINUOUS_SEC),
                    RemoteFloat(0.5f),
                ),
                BlendMode.MULTIPLY,
            )
        recordingCanvas.usePaint(paint)

        recordingCanvas.usePaint(Paint())

        val operations = inflateOperations()
        val paintOp = operations[operations.size - 1] as PaintData
        assertThat(paintOp.mPaintData.toString()).contains("clearColorFilter")
    }

    @Test
    fun remotePaintCopyConstructor() {
        val paint = RemotePaint()
        paint.remoteColor =
            RemoteColor.fromARGB(
                RemoteFloat(1f),
                RemoteFloat(0.8f),
                RemoteFloat(RemoteContext.FLOAT_CONTINUOUS_SEC),
                RemoteFloat(0.5f),
            )
        paint.remoteColorFilter =
            RemoteBlendModeColorFilter(
                RemoteColor.fromARGB(
                    RemoteFloat(1f),
                    RemoteFloat(0.8f),
                    RemoteFloat(RemoteContext.FLOAT_CONTINUOUS_SEC),
                    RemoteFloat(0.5f),
                ),
                BlendMode.MULTIPLY,
            )

        val paintCopy = RemotePaint(paint)

        assertThat(paintCopy.remoteColor).isEqualTo(paint.remoteColor)
        assertThat(paintCopy.remoteColorFilter).isEqualTo(paint.remoteColorFilter)
    }

    @Test
    fun drawToOffscreenBitmap() {
        recordingCanvas.drawRect(0, 0, WIDTH, HEIGHT, Paint().apply { color = Color.BLACK })
        recordingCanvas.drawRect(
            20,
            20,
            WIDTH - 20,
            HEIGHT - 20,
            Paint().apply { color = Color.YELLOW },
        )
        val bitmap =
            recordingCanvas.drawToOffscreenBitmap(WIDTH, HEIGHT, Color.BLACK) {
                recordingCanvas.drawOval(
                    20,
                    20,
                    WIDTH - 20,
                    HEIGHT - 20,
                    Paint().apply { color = Color.RED },
                )
                recordingCanvas.drawText(
                    "HI",
                    20,
                    HEIGHT - 50,
                    Paint().apply {
                        textSize = 380f
                        typeface = Typeface.DEFAULT_BOLD
                        blendMode = BlendMode.CLEAR
                    },
                )
            }
        val rect = Rect(0, 0, WIDTH, HEIGHT)
        recordingCanvas.drawBitmap(
            bitmap,
            rect,
            rect,
            Paint().apply { blendMode = BlendMode.SRC_OVER },
        )

        val document = constructDocument()
        assertScreenshot(document, "offscreenBitmap")
    }

    private fun constructDocument() =
        CoreDocument().apply {
            val buffer = creationState.document.buffer
            buffer.buffer.index = 0
            initFromBuffer(buffer)
        }

    private fun assertScreenshot(document: CoreDocument, filename: String) {
        val bitmap = Bitmap.createBitmap(WIDTH, HEIGHT, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(bitmap)
        remoteContext.useCanvas(canvas)
        document.paint(remoteContext, 0)
        bitmap.assertAgainstGolden(screenshotRule, "${this::class.simpleName}_$filename")
    }

    private fun inflateOperations(): ArrayList<Operation> {
        val buffer = creationState.document.buffer
        buffer.buffer.index = 0
        val operations = ArrayList<Operation>()
        buffer.inflateFromBuffer(operations)
        return operations
    }
}
