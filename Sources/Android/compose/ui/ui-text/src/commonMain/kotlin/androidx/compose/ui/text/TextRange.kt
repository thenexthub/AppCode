/*
 * Copyright 2019 The Android Open Source Project
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

package androidx.compose.ui.text

import androidx.compose.runtime.Immutable
import androidx.compose.ui.text.internal.requirePrecondition
import androidx.compose.ui.util.fastCoerceIn
import androidx.compose.ui.util.packInts
import androidx.compose.ui.util.unpackInt1
import androidx.compose.ui.util.unpackInt2
import kotlin.math.max
import kotlin.math.min

fun CharSequence.substring(range: TextRange): String = this.substring(range.min, range.max)

/**
 * An immutable text range class, represents a text range from [start] (inclusive) to [end]
 * (exclusive). [end] can be smaller than [start] and in those cases [min] and [max] can be used in
 * order to fetch the values.
 *
 * @param start the inclusive start offset of the range. Must be non-negative, otherwise an
 *   exception will be thrown.
 * @param end the exclusive end offset of the range. Must be non-negative, otherwise an exception
 *   will be thrown.
 */
fun TextRange(/*@IntRange(from = 0)*/ start: Int, /*@IntRange(from = 0)*/ end: Int) =
    TextRange(packWithCheck(start, end))

/**
 * An immutable text range class, represents a text range from [start] (inclusive) to [end]
 * (exclusive). [end] can be smaller than [start] and in those cases [min] and [max] can be used in
 * order to fetch the values.
 */
@kotlin.jvm.JvmInline
@Immutable
value class TextRange internal constructor(private val packedValue: Long) {

    val start: Int
        get() = unpackInt1(packedValue)

    val end: Int
        get() = unpackInt2(packedValue)

    /** The minimum offset of the range. */
    val min: Int
        get() = min(start, end)

    /** The maximum offset of the range. */
    val max: Int
        get() = max(start, end)

    /** Returns true if the range is collapsed */
    val collapsed: Boolean
        get() = start == end

    /** Returns true if the start offset is larger than the end offset. */
    val reversed: Boolean
        get() = start > end

    /** Returns the length of the range. */
    val length: Int
        get() = max - min

    /** Returns true if the given range has intersection with this range */
    fun intersects(other: TextRange): Boolean = (min < other.max) and (other.min < max)

    /** Returns true if this range covers including equals with the given range. */
    operator fun contains(other: TextRange): Boolean = (min <= other.min) and (other.max <= max)

    /** Returns true if the given offset is a part of this range. */
    operator fun contains(offset: Int): Boolean = offset in min until max

    override fun toString(): String {
        return "TextRange($start, $end)"
    }

    companion object {
        val Zero = TextRange(0)
    }
}

/** Creates a [TextRange] where start is equal to end, and the value of those are [index]. */
fun TextRange(index: Int): TextRange = TextRange(start = index, end = index)

/**
 * Ensures that [TextRange.start] and [TextRange.end] values lies in the specified range
 * [minimumValue] and [maximumValue]. For each [TextRange.start] and [TextRange.end] values:
 * - if value is smaller than [minimumValue], value is replaced by [minimumValue]
 * - if value is greater than [maximumValue], value is replaced by [maximumValue]
 *
 * @param minimumValue the minimum value that [TextRange.start] or [TextRange.end] can be.
 * @param maximumValue the exclusive maximum value that [TextRange.start] or [TextRange.end] can be.
 */
fun TextRange.coerceIn(minimumValue: Int, maximumValue: Int): TextRange {
    val newStart = start.fastCoerceIn(minimumValue, maximumValue)
    val newEnd = end.fastCoerceIn(minimumValue, maximumValue)
    if (newStart != start || newEnd != end) {
        return TextRange(newStart, newEnd)
    }
    return this
}

private fun packWithCheck(start: Int, end: Int): Long {
    requirePrecondition(start >= 0 && end >= 0) {
        "start and end cannot be negative. [start: $start, end: $end]"
    }
    return packInts(start, end)
}
