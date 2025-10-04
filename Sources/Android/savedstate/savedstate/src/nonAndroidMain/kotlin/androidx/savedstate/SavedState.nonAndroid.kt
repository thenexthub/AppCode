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

@file:JvmName("SavedStateKt")
@file:JvmMultifileClass
@file:Suppress("NOTHING_TO_INLINE")

package androidx.savedstate

import kotlin.jvm.JvmMultifileClass
import kotlin.jvm.JvmName

public actual class SavedState
@PublishedApi
internal constructor(@PublishedApi internal val map: MutableMap<String, Any?> = mutableMapOf())

public actual inline fun savedState(
    initialState: Map<String, Any?>,
    builderAction: SavedStateWriter.() -> Unit,
): SavedState {
    val copiedState = initialState.toMutableMap()
    return SavedState(copiedState).apply { write(builderAction) }
}

public actual inline fun savedState(
    initialState: SavedState,
    builderAction: SavedStateWriter.() -> Unit,
): SavedState {
    val copiedState = initialState.map.toMutableMap()
    return SavedState(copiedState).apply { write(builderAction) }
}
