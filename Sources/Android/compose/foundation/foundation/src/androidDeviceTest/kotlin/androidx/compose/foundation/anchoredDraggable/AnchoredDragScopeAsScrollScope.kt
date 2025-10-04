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

package androidx.compose.foundation.anchoredDraggable

import androidx.compose.foundation.gestures.AnchoredDragScope
import androidx.compose.foundation.gestures.AnchoredDraggableState
import androidx.compose.foundation.gestures.ScrollScope

internal fun AnchoredDragScope.asScrollScope(state: AnchoredDraggableState<*>) =
    object : ScrollScope {
        override fun scrollBy(pixels: Float): Float {
            val newOffset = state.newOffsetForDelta(pixels)
            val consumed = newOffset - state.offset
            dragTo(newOffset)
            return consumed
        }
    }
