/*
 * Copyright (C) 2025 The Android Open Source Project
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

package androidx.ink.authoring

import android.util.Log
import androidx.ink.brush.Brush
import androidx.ink.brush.ExperimentalInkCustomBrushApi
import androidx.ink.brush.TextureAnimationProgressHelper
import androidx.ink.geometry.Box
import androidx.ink.geometry.BoxAccumulator
import androidx.ink.strokes.InProgressStroke
import androidx.ink.strokes.Stroke
import androidx.ink.strokes.StrokeInputBatch
import kotlin.random.Random

/**
 * An implementation of [InProgressShape] that simply wraps [androidx.ink.strokes.InProgressStroke].
 */
internal class InkInProgressShape : InProgressShape<Brush, Stroke> {

    internal val inProgressStroke = InProgressStroke()

    private var shapeChangesWithTime = false
    internal var textureAnimationDurationMillis: Long = -Long.MIN_VALUE
        private set

    /** Whether this shape has been canceled. Primarily tracked for defensive coding purposes. */
    internal var canceled = false
        private set

    private var updateSinceResetUpdatedRegion = false
    private var cancelSinceResetUpdatedRegion = false

    /** The most recent value passed to [update]. Acts as the current time for all calculations. */
    internal var lastUpdateSystemElapsedTimeMillis = Long.MIN_VALUE
        private set

    /** Used by [getUpdatedRegion]. */
    private val scratchUpdatedRegion = BoxAccumulator()

    /** Used by [getBoundingBox]. */
    private val scratchBoundingBox = BoxAccumulator()

    /**
     * Used on a very short-term basis to be overwritten by `populate*` functions, and its result
     * added somewhere else for accumulation.
     */
    private val scratchBoxAccumulator = BoxAccumulator()

    @OptIn(ExperimentalInkCustomBrushApi::class)
    override fun start(shapeSpec: Brush) {
        inProgressStroke.start(brush = shapeSpec, noiseSeed = Random.Default.nextInt())
        shapeChangesWithTime = inProgressStroke.changesWithTime()
        textureAnimationDurationMillis =
            TextureAnimationProgressHelper.getAnimationDurationMillis(shapeSpec.family)
    }

    override fun enqueueInputs(realInputs: StrokeInputBatch, predictedInputs: StrokeInputBatch) {
        try {
            inProgressStroke.enqueueInputs(realInputs, predictedInputs)
        } catch (t: Throwable) {
            // TODO(b/306361370): Throw here once input is more sanitized.
            Log.w(
                InkInProgressShape::class.simpleName,
                "Error during InProgressStroke.enqueueInputs",
                t,
            )
        }
    }

    override fun changesWithTime(): Boolean {
        return shapeChangesWithTime || textureAnimationDurationMillis > 0
    }

    override fun update(
        inputElapsedTimeMillis: Long,
        systemElapsedTimeMillis: Long,
        forceCompletion: Boolean,
    ) {
        // Update these values even if the underlying [InProgressStroke] doesn't need updating, so
        // that
        // texture animations can be properly rendered.
        lastUpdateSystemElapsedTimeMillis = systemElapsedTimeMillis
        updateSinceResetUpdatedRegion = true

        if (!inProgressStroke.isUpdateNeeded()) return
        runCatching {
                inProgressStroke.updateShape(
                    if (forceCompletion) Long.MAX_VALUE else inputElapsedTimeMillis
                )
            }
            .exceptionOrNull()
            ?.let {
                Log.w(
                    InkInProgressShape::class.simpleName,
                    "Error during InProgressStroke.updateShape",
                    it,
                )
            }
    }

    override fun cancel() {
        canceled = true
        cancelSinceResetUpdatedRegion = true
    }

    override fun getUpdatedRegion(): Box? {
        scratchUpdatedRegion.reset()
        if (cancelSinceResetUpdatedRegion) {
            // When a stroke has been canceled, the entire stroke is considered updated because it
            // is
            // being removed from the screen.
            scratchUpdatedRegion.add(getBoundingBox())
        }
        if (updateSinceResetUpdatedRegion) {
            scratchUpdatedRegion.add(inProgressStroke.populateUpdatedRegion(scratchBoxAccumulator))
            // When a stroke has texture animation, assume it needs to fully re-render with each
            // timestamp
            // change. This means the updated region is mostly equivalent to the stroke's bounding
            // box,
            // but also include the stroke's updated region in case there's an update that lies
            // outside
            // the current bounding box - for example, an overshot prediction being erased or the
            // disappearing end of a laser pointer style stroke.
            if (textureAnimationDurationMillis > 0) {
                scratchUpdatedRegion.add(getBoundingBox())
            }
        }
        return scratchUpdatedRegion.box
    }

    private fun getBoundingBox(): Box? {
        scratchBoundingBox.reset()
        for (coatIndex in 0 until inProgressStroke.getBrushCoatCount()) {
            scratchBoundingBox.add(
                inProgressStroke.populateMeshBounds(coatIndex, scratchBoxAccumulator)
            )
        }
        return scratchBoundingBox.box
    }

    override fun resetUpdatedRegion() {
        cancelSinceResetUpdatedRegion = false
        updateSinceResetUpdatedRegion = false
        inProgressStroke.resetUpdatedRegion()
    }

    override fun finishInput() {
        inProgressStroke.finishInput()
    }

    override fun getCompletedShape() =
        if (inProgressStroke.isInputFinished() && !inProgressStroke.isUpdateNeeded()) {
            inProgressStroke.toImmutable()
        } else {
            null
        }

    override fun clear() {
        lastUpdateSystemElapsedTimeMillis = Long.MIN_VALUE
        updateSinceResetUpdatedRegion = false
        cancelSinceResetUpdatedRegion = false
        canceled = false
        textureAnimationDurationMillis = -Long.MIN_VALUE
        shapeChangesWithTime = false
        inProgressStroke.clear()
    }
}
