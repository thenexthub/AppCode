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

package androidx.xr.compose.subspace.layout

import android.content.pm.PackageManager
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.unit.Density
import androidx.core.content.ContextCompat
import androidx.xr.compose.platform.LocalSession
import androidx.xr.compose.subspace.node.CompositionLocalConsumerSubspaceModifierNode
import androidx.xr.compose.subspace.node.LayoutCoordinatesAwareModifierNode
import androidx.xr.compose.subspace.node.SubspaceLayoutModifierNode
import androidx.xr.compose.subspace.node.SubspaceModifierNodeElement
import androidx.xr.compose.subspace.node.currentValueOf
import androidx.xr.compose.unit.IntVolumeSize
import androidx.xr.compose.unit.VolumeConstraints
import androidx.xr.compose.unit.toDimensionsInMeters
import androidx.xr.compose.unit.toIntVolumeSize
import androidx.xr.runtime.Config
import androidx.xr.runtime.Session
import androidx.xr.runtime.manifest.SCENE_UNDERSTANDING_COARSE
import androidx.xr.runtime.math.Pose
import androidx.xr.runtime.math.Ray
import androidx.xr.scenecore.AnchorPlacement
import androidx.xr.scenecore.Entity
import androidx.xr.scenecore.EntityMoveListener
import androidx.xr.scenecore.MovableComponent
import androidx.xr.scenecore.PanelEntity
import androidx.xr.scenecore.PlaneOrientation as SceneCorePlaneOrientation
import androidx.xr.scenecore.PlaneSemanticType as SceneCorePlaneSemantic
import java.util.concurrent.Executor
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.asExecutor

/**
 * When the movable modifier is present and enabled, draggable UI controls will be shown that allow
 * the user to move the element in 3D space. This feature is only available for
 * [SpatialPanels][androidx.xr.compose.subspace.SpatialPanel] at the moment. This overload of the
 * modifier allows the element to be anchored to a plane in the real world.
 *
 * This modifier requires requires [androidx.xr.runtime.Session.configure] to be called with
 * [androidx.xr.runtime.Config.PlaneTrackingMode.HORIZONTAL_AND_VERTICAL]. This configuration
 * requires that the `SCENE_UNDERSTANDING_COARSE` Android permission is granted. If not granted, the
 * `anchorable` functionality will be disabled, and the element will behave as if the anchorable
 * modifier was not applied.
 *
 * @param enabled true if this composable should be movable.
 * @param anchorPlaneOrientations when supplied, this movable entity can be anchored to Horizontal
 *   or Vertical planes or both (ANY). Can be used without anchorPlaneSemantics being supplied.
 * @param anchorPlaneSemantics when supplied, this movable entity can be anchored to planes which
 *   match one of the supplied list of semantic interpretations, such as a "table" or "floor". Can
 *   be used without anchorPlaneOrientations being supplied.
 */
internal fun SubspaceModifier.anchorable(
    enabled: Boolean = true,
    onMoveStart: ((SpatialMoveStartEvent) -> Unit)? = null,
    onMoveEnd: ((SpatialMoveEndEvent) -> Unit)? = null,
    onMove: ((SpatialMoveEvent) -> Boolean)? = null,
    @Suppress("PrimitiveInCollection") anchorPlaneOrientations: Set<PlaneOrientation> = emptySet(),
    @Suppress("PrimitiveInCollection") anchorPlaneSemantics: Set<PlaneSemantic> = emptySet(),
): SubspaceModifier =
    this.then(
        AnchorableElement(
            enabled,
            onMoveStart = onMoveStart,
            onMoveEnd = onMoveEnd,
            onMove = onMove,
            stickyPose = true,
            scaleWithDistance = true,
            anchorPlaneOrientations,
            anchorPlaneSemantics,
        )
    )

private class AnchorableElement(
    private val enabled: Boolean,
    private val onMoveStart: ((SpatialMoveStartEvent) -> Unit)?,
    private val onMoveEnd: ((SpatialMoveEndEvent) -> Unit)?,
    private val onMove: ((SpatialMoveEvent) -> Boolean)?,
    private val stickyPose: Boolean,
    private val scaleWithDistance: Boolean,
    @Suppress("PrimitiveInCollection")
    private val anchorPlaneOrientations: Set<PlaneOrientation> = emptySet(),
    @Suppress("PrimitiveInCollection")
    private val anchorPlaneSemantics: Set<PlaneSemantic> = emptySet(),
) : SubspaceModifierNodeElement<AnchorableNode>() {
    override fun create(): AnchorableNode =
        AnchorableNode(
            enabled = enabled,
            stickyPose = stickyPose,
            onMoveStart = onMoveStart,
            onMoveEnd = onMoveEnd,
            onMove = onMove,
            scaleWithDistance = scaleWithDistance,
            anchorPlaneOrientations = anchorPlaneOrientations,
            anchorPlaneSemantics = anchorPlaneSemantics,
        )

    override fun update(node: AnchorableNode) {
        node.enabled = enabled
        node.onMoveStart = onMoveStart
        node.onMoveEnd = onMoveEnd
        node.onMove = onMove
        node.stickyPose = stickyPose
        node.scaleWithDistance = scaleWithDistance
        node.anchorPlaneOrientations = anchorPlaneOrientations
        node.anchorPlaneSemantics = anchorPlaneSemantics
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is AnchorableElement) return false
        if (enabled != other.enabled) return false
        if (onMoveStart !== other.onMoveStart) return false
        if (onMoveEnd !== other.onMoveEnd) return false
        if (onMove !== other.onMove) return false
        if (stickyPose != other.stickyPose) return false
        if (scaleWithDistance != other.scaleWithDistance) return false
        if (anchorPlaneOrientations != other.anchorPlaneOrientations) return false
        if (anchorPlaneSemantics != other.anchorPlaneSemantics) return false
        return true
    }

    override fun hashCode(): Int {
        var result = enabled.hashCode()
        result = 31 * result + onMoveStart.hashCode()
        result = 31 * result + onMoveEnd.hashCode()
        result = 31 * result + onMove.hashCode()
        result = 31 * result + stickyPose.hashCode()
        result = 31 * result + scaleWithDistance.hashCode()
        result = 31 * result + anchorPlaneOrientations.hashCode()
        result = 31 * result + anchorPlaneSemantics.hashCode()
        return result
    }
}

internal class AnchorableNode(
    var enabled: Boolean,
    var stickyPose: Boolean,
    var scaleWithDistance: Boolean,
    var onMoveStart: ((SpatialMoveStartEvent) -> Unit)?,
    var onMoveEnd: ((SpatialMoveEndEvent) -> Unit)?,
    var onMove: ((SpatialMoveEvent) -> Boolean)?,
    @Suppress("PrimitiveInCollection")
    var anchorPlaneOrientations: Set<PlaneOrientation> = emptySet(),
    @Suppress("PrimitiveInCollection") var anchorPlaneSemantics: Set<PlaneSemantic> = emptySet(),
) :
    SubspaceModifier.Node(),
    CompositionLocalConsumerSubspaceModifierNode,
    CoreEntityNode,
    LayoutCoordinatesAwareModifierNode,
    EntityMoveListener,
    SubspaceLayoutModifierNode {
    private inline val density: Density
        get() = currentValueOf(LocalDensity)

    private inline val session: Session
        get() = checkNotNull(currentValueOf(LocalSession)) { "Movable requires a Session." }

    /** Pose based on user adjustments from MoveEvents from SceneCore. */
    private var userPose: Pose = Pose.Identity
    /** The scale of this entity when it is moved. */
    private var scaleFromMovement: Float = 1.0F
    private var component: MovableComponent? = null

    override fun CoreEntityScope.modifyCoreEntity() {
        setOrAppendScale(scaleFromMovement)
    }

    override fun onDetach() {
        if (component != null) {
            disableAnchorableComponent()
        }
    }

    override fun SubspaceMeasureScope.measure(
        measurable: SubspaceMeasurable,
        constraints: VolumeConstraints,
    ): SubspaceMeasureResult {
        // modifyCoreEntity happens during placement, so we need to update the component state here
        // before measurement.
        updateAnchorableState()
        val placeable = measurable.measure(constraints)
        return layout(placeable.measuredWidth, placeable.measuredHeight, placeable.measuredDepth) {
            placeable.place(userPose)
        }
    }

    override fun onLayoutCoordinates(coordinates: SubspaceLayoutCoordinates) {
        // Update the size of the component to match the final size of the layout.
        component?.size = coordinates.size.toDimensionsInMeters(density)
    }

    /** Updates the anchorable state of this CoreEntity. */
    private fun updateAnchorableState() {
        if (coreEntity !is MovableCoreEntity) {
            return
        }
        // Enabled is on the Node. It means "should be enabled" for the Component.
        if (enabled && component == null) {
            enableAnchorableComponent()
        } else if (!enabled && component != null) {
            disableAnchorableComponent()
        }
    }

    /** Enables the MovableComponent and anchorPlacement for this CoreEntity. */
    private fun enableAnchorableComponent() {
        check(component == null) { "MovableComponent already enabled." }

        if (session.config.planeTracking == Config.PlaneTrackingMode.DISABLED) {
            return
        }

        val anchorPlacement = convertToAnchorPlacement()

        if (!anchorPlacement.isEmpty()) {
            if (
                ContextCompat.checkSelfPermission(
                    currentValueOf(LocalContext),
                    SCENE_UNDERSTANDING_COARSE,
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                return
            }
        }

        // The developer could have used the movable overload which allows them to supply their own
        // onMove logic, or they could have used the overload which provides the anchoring ability
        // (where the movement is managed by the system).
        if (anchorPlacement.isEmpty()) {
            return
        } else {
            component = MovableComponent.createAnchorable(session, anchorPlacement)
            component?.addMoveListener(this)
        }
        check(component?.let { coreEntity.addComponent(it) } == true) {
            "Could not add MovableComponent to Core Entity."
        }
    }

    /**
     * Disables the MovableComponent for this CoreEntity. Takes care of life cycle tasks for the
     * underlying component in SceneCore.
     */
    private fun disableAnchorableComponent() {
        check(component != null) { "MovableComponent already disabled." }
        component?.removeMoveListener(this)
        component?.let { coreEntity.removeComponent(it) }
        component = null
        if (!stickyPose) {
            userPose = Pose.Identity
        }
    }

    /**
     * Takes the param values PlaneOrientation and PlaneSemantic, and returns the SceneCore
     * equivalent, which is bundled in a structure called AnchorPlacement.
     *
     * The lint error was suppressed because the function being called requires a set of ints.
     */
    @Suppress("PrimitiveInCollection")
    private fun convertToAnchorPlacement(): Set<AnchorPlacement> {
        if (anchorPlaneOrientations.isEmpty() && anchorPlaneSemantics.isEmpty())
            return mutableSetOf()

        val planeTypeFilter: MutableSet<Int> = mutableSetOf()
        anchorPlaneOrientations.forEach { planeTypeFilter.add(it.value) }
        if (planeTypeFilter.isEmpty()) planeTypeFilter.add(SceneCorePlaneOrientation.ANY)

        val planeSemanticFilter: MutableSet<Int> = mutableSetOf()
        anchorPlaneSemantics.forEach { planeSemanticFilter.add(it.value) }
        if (planeSemanticFilter.isEmpty()) planeSemanticFilter.add(SceneCorePlaneSemantic.ANY)

        return mutableSetOf(AnchorPlacement.createForPlanes(planeTypeFilter, planeSemanticFilter))
    }

    override fun onMoveStart(
        entity: Entity,
        initialInputRay: Ray,
        initialPose: Pose,
        initialScale: Float,
        initialParent: Entity,
    ) {
        val initialSize: IntVolumeSize =
            when (entity) {
                is PanelEntity -> entity.size.to3d().toIntVolumeSize(density)
                else -> IntVolumeSize.Zero
            }
        val event =
            SpatialMoveStartEvent(
                initialPose.convertMetersToPixels(density),
                initialScale,
                initialSize,
            )
        onMoveStart?.invoke(event)
    }

    override fun onMoveUpdate(
        entity: Entity,
        currentInputRay: Ray,
        currentPose: Pose,
        currentScale: Float,
    ) {

        userPose = currentPose.convertMetersToPixels(density)
        val initialSize: IntVolumeSize =
            when (entity) {
                is PanelEntity -> entity.size.to3d().toIntVolumeSize(density)
                else -> IntVolumeSize.Zero
            }
        val event =
            SpatialMoveEvent(currentPose.convertMetersToPixels(density), currentScale, initialSize)
        onMove?.invoke(event)
    }

    override fun onMoveEnd(
        entity: Entity,
        finalInputRay: Ray,
        finalPose: Pose,
        finalScale: Float,
        updatedParent: Entity?,
    ) {
        userPose = finalPose.convertMetersToPixels(density)
        val finalSize: IntVolumeSize =
            when (entity) {
                is PanelEntity -> entity.size.to3d().toIntVolumeSize(density)
                else -> IntVolumeSize.Zero
            }
        val event =
            SpatialMoveEndEvent(finalPose.convertMetersToPixels(density), finalScale, finalSize)
        onMoveEnd?.invoke(event)
    }

    companion object {
        private val MainExecutor: Executor = Dispatchers.Main.asExecutor()
    }
}

/** Type of plane based on orientation i.e. Horizontal or Vertical. */
@JvmInline
public value class PlaneOrientation private constructor(internal val value: Int) {
    public companion object {
        public val Horizontal: PlaneOrientation =
            PlaneOrientation(SceneCorePlaneOrientation.HORIZONTAL)
        public val Vertical: PlaneOrientation = PlaneOrientation(SceneCorePlaneOrientation.VERTICAL)
        public val Any: PlaneOrientation = PlaneOrientation(SceneCorePlaneOrientation.ANY)
    }

    override fun toString(): String {
        return when (this) {
            Horizontal -> "PlaneOrientation.Horizontal"
            Vertical -> "PlaneOrientation.Vertical"
            Any -> "PlaneOrientation.Any"
            else -> super.toString()
        }
    }
}

/** Semantic plane types. */
@JvmInline
public value class PlaneSemantic private constructor(internal val value: Int) {
    public companion object {
        public val Wall: PlaneSemantic = PlaneSemantic(SceneCorePlaneSemantic.WALL)
        public val Floor: PlaneSemantic = PlaneSemantic(SceneCorePlaneSemantic.FLOOR)
        public val Ceiling: PlaneSemantic = PlaneSemantic(SceneCorePlaneSemantic.CEILING)
        public val Table: PlaneSemantic = PlaneSemantic(SceneCorePlaneSemantic.TABLE)
        public val Any: PlaneSemantic = PlaneSemantic(SceneCorePlaneSemantic.ANY)
    }

    override fun toString(): String {
        return when (this) {
            Wall -> "PlaneSemantic.Wall"
            Floor -> "PlaneSemantic.Floor"
            Ceiling -> "PlaneSemantic.Ceiling"
            Table -> "PlaneSemantic.Table"
            Any -> "PlaneSemantic.Any"
            else -> super.toString()
        }
    }
}
