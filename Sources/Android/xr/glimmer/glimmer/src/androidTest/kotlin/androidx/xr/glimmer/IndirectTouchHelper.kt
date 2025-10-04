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

package androidx.xr.glimmer

import android.os.SystemClock
import android.view.MotionEvent
import androidx.compose.ui.ExperimentalIndirectTouchTypeApi
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.input.indirect.IndirectTouchEvent
import androidx.compose.ui.input.indirect.IndirectTouchEventPrimaryDirectionalMotionAxis
import androidx.compose.ui.test.SemanticsNodeInteraction
import androidx.compose.ui.test.junit4.ComposeTestRule
import androidx.core.view.InputDeviceCompat.SOURCE_TOUCH_NAVIGATION

/** Synthetic indirect swipe. */
@OptIn(ExperimentalIndirectTouchTypeApi::class)
internal fun SemanticsNodeInteraction.performIndirectSwipe(
    rule: ComposeTestRule,
    distance: Float,
    moveDuration: Long = 200L,
) {
    val currentTime = SystemClock.uptimeMillis()

    val down =
        MotionEvent.obtain(
            currentTime, // downTime,
            currentTime, // eventTime,
            MotionEvent.ACTION_DOWN,
            0f,
            Offset.Zero.y,
            0,
        )
    down.source = SOURCE_TOUCH_NAVIGATION
    performIndirectTouchEvent(
        rule,
        IndirectTouchEvent(
            motionEvent = down,
            primaryDirectionalMotionAxis = IndirectTouchEventPrimaryDirectionalMotionAxis.X,
        ),
    )

    val move =
        MotionEvent.obtain(
            currentTime,
            currentTime + moveDuration,
            MotionEvent.ACTION_MOVE,
            distance,
            Offset.Zero.y,
            0,
        )
    move.source = SOURCE_TOUCH_NAVIGATION
    performIndirectTouchEvent(
        rule,
        IndirectTouchEvent(
            motionEvent = move,
            primaryDirectionalMotionAxis = IndirectTouchEventPrimaryDirectionalMotionAxis.X,
            previousMotionEvent = down,
        ),
    )

    val up =
        MotionEvent.obtain(
            currentTime,
            currentTime + moveDuration + 20,
            MotionEvent.ACTION_UP,
            distance,
            Offset.Zero.y,
            0,
        )
    up.source = SOURCE_TOUCH_NAVIGATION
    performIndirectTouchEvent(
        rule,
        IndirectTouchEvent(
            motionEvent = up,
            primaryDirectionalMotionAxis = IndirectTouchEventPrimaryDirectionalMotionAxis.X,
            previousMotionEvent = move,
        ),
    )
}

@OptIn(ExperimentalIndirectTouchTypeApi::class)
internal fun SemanticsNodeInteraction.performIndirectClick(
    rule: ComposeTestRule,
    durationMillis: Long = 40L,
) {
    val currentTime = SystemClock.uptimeMillis()

    val down = MotionEvent.obtain(currentTime, currentTime, MotionEvent.ACTION_DOWN, 0f, 0f, 0)
    down.source = SOURCE_TOUCH_NAVIGATION

    performIndirectTouchEvent(
        rule,
        IndirectTouchEvent(
            motionEvent = down,
            primaryDirectionalMotionAxis = IndirectTouchEventPrimaryDirectionalMotionAxis.X,
        ),
    )

    val up =
        MotionEvent.obtain(
            currentTime,
            currentTime + durationMillis,
            MotionEvent.ACTION_UP,
            0f,
            0f,
            0,
        )

    performIndirectTouchEvent(
        rule,
        IndirectTouchEvent(
            motionEvent = up,
            primaryDirectionalMotionAxis = IndirectTouchEventPrimaryDirectionalMotionAxis.X,
            previousMotionEvent = down,
        ),
    )
}

/**
 * Send the specified [IndirectTouchEvent] to the focused component.
 *
 * @return true if the event was consumed. False otherwise.
 */
@OptIn(ExperimentalIndirectTouchTypeApi::class)
internal fun SemanticsNodeInteraction.performIndirectTouchEvent(
    rule: ComposeTestRule,
    indirectTouchEvent: IndirectTouchEvent,
): Boolean {
    val semanticsNode =
        fetchSemanticsNode("Failed to send indirect touch event ($indirectTouchEvent)")
    val root = semanticsNode.root
    requireNotNull(root) { "Failed to find owner" }
    return rule.runOnUiThread { root.sendIndirectTouchEvent(indirectTouchEvent) }
}
