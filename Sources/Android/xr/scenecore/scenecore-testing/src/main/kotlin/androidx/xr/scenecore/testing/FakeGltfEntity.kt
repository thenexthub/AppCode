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

import androidx.annotation.RestrictTo
import androidx.xr.scenecore.runtime.GltfEntity
import androidx.xr.scenecore.runtime.GltfFeature
import androidx.xr.scenecore.runtime.MaterialResource
import java.util.concurrent.Executor

/** Test-only implementation of [androidx.xr.scenecore.runtime.GltfEntity] */
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP_PREFIX)
public open class FakeGltfEntity(
    private val feature: GltfFeature? = null,
    private val executor: Executor? = null,
) : FakeEntity(), GltfEntity {
    public class Node {
        public val nodeName: String = "glTF node"
        public val materialArray: Array<FakeResource> =
            arrayOf(FakeResource(1), FakeResource(2), FakeResource(3))
    }

    public val node: Node = Node()

    @GltfEntity.AnimationStateValue
    private var _animationState: Int = GltfEntity.AnimationState.STOPPED

    /** Returns the current animation state of the glTF entity. */
    @GltfEntity.AnimationStateValue
    override val animationState: Int
        get() {
            return feature?.animationState ?: _animationState
        }

    /**
     * Indicates whether the animation is currently looping. In tests, you can
     * - call [startAnimation] with loop set to true to simulate looping the animation and verify
     *   that your code responds correctly to the animation looping.
     * - call [stopAnimation] to clear the looping state and verify that your code responds
     *   correctly to the animation stopping.
     */
    public var isLooping: Boolean = false

    override fun setMaterialOverride(
        material: MaterialResource,
        nodeName: String,
        primitiveIndex: Int,
    ) {
        feature?.setMaterialOverride(material, nodeName, primitiveIndex)
        if (nodeName == node.nodeName && primitiveIndex < node.materialArray.size) {
            node.materialArray[primitiveIndex] = material as FakeResource
        }
    }

    override fun clearMaterialOverride(nodeName: String, primitiveIndex: Int) {
        feature?.clearMaterialOverride(nodeName, primitiveIndex)
        if (nodeName == node.nodeName && primitiveIndex < node.materialArray.size) {
            node.materialArray[primitiveIndex] = FakeResource(primitiveIndex.toLong())
        }
    }

    /**
     * The name of the animation that is currently playing. In tests, you can
     * - call [startAnimation] with a supported animationName and verify that your code responds
     *   correctly to the animation starting.
     * - call [stopAnimation] to clear the value and verify that your code responds correctly to the
     *   animation stopping.
     */
    public var currentAnimationName: String? = null
        private set

    /**
     * A list of supported animation names with a default value of "animation_name" which is used in
     * the scenecore/JxrPlatformAdapterAxrTest unit test. In tests, you can call [startAnimation]
     * with a supported/unsupported animationName and verify that your code responds correctly to
     * the [isLooping] and [currentAnimationName] values.
     */
    public var supportedAnimationNames: MutableList<String> = mutableListOf("animation_name")

    /**
     * Starts the animation with a supported given name when the animation state is STOPPED.
     *
     * @param currentAnimationName The name of the animation to start. If null is supplied, will
     *   play the first animation found in the glTF.
     * @param loop Whether the animation should loop.
     */
    override fun startAnimation(loop: Boolean, animationName: String?) {
        feature?.startAnimation(loop, animationName, executor!!)
        if (
            supportedAnimationNames.contains(animationName) &&
                _animationState == GltfEntity.AnimationState.STOPPED
        ) {

            _animationState = GltfEntity.AnimationState.PLAYING

            isLooping = loop
            currentAnimationName = animationName
        }
    }

    /** Stops the animation of the glTF entity. */
    override fun stopAnimation() {
        feature?.stopAnimation()
        if (_animationState == GltfEntity.AnimationState.PLAYING) {
            _animationState = GltfEntity.AnimationState.STOPPED

            isLooping = false
            currentAnimationName = null
        }
    }
}
