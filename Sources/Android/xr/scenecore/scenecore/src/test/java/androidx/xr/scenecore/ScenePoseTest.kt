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

package androidx.xr.scenecore

import android.app.Activity
import androidx.xr.runtime.FieldOfView
import androidx.xr.runtime.math.Pose
import androidx.xr.runtime.math.Vector3
import androidx.xr.scenecore.ScenePose.HitTestFilter
import androidx.xr.scenecore.runtime.CameraViewActivityPose as RtCameraViewActivityPose
import androidx.xr.scenecore.runtime.HitTestResult as RtHitTestResult
import androidx.xr.scenecore.runtime.SceneRuntime
import androidx.xr.scenecore.testing.FakeCameraViewActivityPose
import androidx.xr.scenecore.testing.FakeHeadActivityPose
import androidx.xr.scenecore.testing.FakePerceptionSpaceActivityPose
import androidx.xr.scenecore.testing.FakeSceneRuntimeFactory
import com.google.common.truth.Truth.assertThat
import kotlinx.coroutines.runBlocking
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.Robolectric
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class ScenePoseTest {
    private val entityManager = EntityManager()

    private var head: Head? = null
    private lateinit var activitySpace: ActivitySpace
    private var camera: CameraView? = null
    private lateinit var perceptionSpace: PerceptionSpace

    private val activity = Robolectric.buildActivity(Activity::class.java).create().start().get()
    private lateinit var fakeRuntime: SceneRuntime

    @Before
    fun setUp() {
        val fakeRuntimeFactory = FakeSceneRuntimeFactory()
        fakeRuntime = fakeRuntimeFactory.create(activity)

        head = Head.create(fakeRuntime)
        camera = CameraView.createLeft(fakeRuntime)
        activitySpace = ActivitySpace.create(fakeRuntime, entityManager)
        perceptionSpace = PerceptionSpace.create(fakeRuntime)
    }

    @Test
    fun allScenePoseTransformPoseTo_callsRuntimeActivityPoseImplTransformPoseTo() {
        val pose = Pose.Identity

        assertThat(head?.transformPoseTo(pose, head!!)).isEqualTo(pose)
        assertThat(camera!!.transformPoseTo(pose, camera!!)).isEqualTo(pose)
        assertThat(perceptionSpace.transformPoseTo(pose, perceptionSpace)).isEqualTo(pose)
    }

    @Test
    fun allScenePoseTransformPoseToEntity_callsRuntimeActivityPoseImplTransformPoseTo() {
        val pose = Pose.Identity

        assertThat(head!!.transformPoseTo(pose, activitySpace)).isEqualTo(pose)
        assertThat(camera!!.transformPoseTo(pose, activitySpace)).isEqualTo(pose)
        assertThat(perceptionSpace.transformPoseTo(pose, activitySpace)).isEqualTo(pose)
    }

    @Test
    fun allScenePoseTransformPoseFromEntity_callsRuntimeActivityPoseImplTransformPoseTo() {
        val pose = Pose.Identity

        assertThat(activitySpace.transformPoseTo(pose, head!!)).isEqualTo(pose)
        assertThat(activitySpace.transformPoseTo(pose, camera!!)).isEqualTo(pose)
        assertThat(activitySpace.transformPoseTo(pose, perceptionSpace)).isEqualTo(pose)
    }

    @Test
    fun allScenePoseGetActivitySpacePose_callsRuntimeActivityPoseImplGetActivitySpacePose() {
        val pose = Pose.Identity

        assertThat(head!!.activitySpacePose).isEqualTo(pose)
        assertThat(camera!!.activitySpacePose).isEqualTo(pose)
        assertThat(perceptionSpace.activitySpacePose).isEqualTo(pose)
    }

    @Test
    fun hitTest_callsRuntimeHitTest() {
        val origin = Vector3(1f, 2f, 3f)
        val direction = Vector3(4f, 5f, 6f)
        val hitTestFilter = HitTestFilter.SELF_SCENE
        val hitPosition = Vector3(7f, 8f, 9f)
        val surfaceNormal = Vector3(10f, 11f, 12f)
        val distance = 7f
        val surfaceType = RtHitTestResult.HitTestSurfaceType.HIT_TEST_RESULT_SURFACE_TYPE_PLANE
        val expectedHitTestResult =
            HitTestResult(hitPosition, surfaceNormal, surfaceType.toHitTestSurfaceType(), distance)

        // Set the hit test results.
        val rtHitTestResult = RtHitTestResult(hitPosition, surfaceNormal, surfaceType, distance)
        (fakeRuntime.headActivityPose as? FakeHeadActivityPose)?.hitTestResult = rtHitTestResult
        (fakeRuntime.getCameraViewActivityPose(
                RtCameraViewActivityPose.CameraType.CAMERA_TYPE_LEFT_EYE
            ) as? FakeCameraViewActivityPose)
            ?.hitTestResult = rtHitTestResult
        (fakeRuntime.perceptionSpaceActivityPose as FakePerceptionSpaceActivityPose).hitTestResult =
            rtHitTestResult

        runBlocking {
            assertThat(head!!.hitTest(origin, direction, hitTestFilter))
                .isEqualTo(expectedHitTestResult)
            assertThat(camera!!.hitTest(origin, direction, hitTestFilter))
                .isEqualTo(expectedHitTestResult)
            assertThat(perceptionSpace.hitTest(origin, direction, hitTestFilter))
                .isEqualTo(expectedHitTestResult)
        }
    }

    @Test
    fun hitTest_withDefaultHitTestFilter_callsRuntimeHitTest() {
        val origin = Vector3(1f, 2f, 3f)
        val direction = Vector3(4f, 5f, 6f)
        val hitPosition = Vector3(7f, 8f, 9f)
        val surfaceNormal = Vector3(10f, 11f, 12f)
        val distance = 7f
        val surfaceType = RtHitTestResult.HitTestSurfaceType.HIT_TEST_RESULT_SURFACE_TYPE_PLANE
        val expectedHitTestResult =
            HitTestResult(hitPosition, surfaceNormal, surfaceType.toHitTestSurfaceType(), distance)

        // Set the hit test results.
        val rtHitTestResult = RtHitTestResult(hitPosition, surfaceNormal, surfaceType, distance)
        (fakeRuntime.headActivityPose as? FakeHeadActivityPose)?.hitTestResult = rtHitTestResult
        (fakeRuntime.getCameraViewActivityPose(
                RtCameraViewActivityPose.CameraType.CAMERA_TYPE_LEFT_EYE
            ) as? FakeCameraViewActivityPose)
            ?.hitTestResult = rtHitTestResult
        (fakeRuntime.perceptionSpaceActivityPose as FakePerceptionSpaceActivityPose).hitTestResult =
            rtHitTestResult

        runBlocking {
            assertThat(head!!.hitTest(origin, direction)).isEqualTo(expectedHitTestResult)
            assertThat(camera!!.hitTest(origin, direction)).isEqualTo(expectedHitTestResult)
            assertThat(perceptionSpace.hitTest(origin, direction)).isEqualTo(expectedHitTestResult)
        }
    }

    @Test
    fun cameraView_getFov_returnsFov() {
        val rtFov = RtCameraViewActivityPose.Fov(1f, 2f, 3f, 4f)

        // Set the fov in the camera view.
        (fakeRuntime.getCameraViewActivityPose(
                RtCameraViewActivityPose.CameraType.CAMERA_TYPE_LEFT_EYE
            ) as? FakeCameraViewActivityPose)
            ?.fov = rtFov

        assertThat(camera!!.fov).isEqualTo(FieldOfView(1f, 2f, 3f, 4f))
    }

    @Test
    fun cameraView_getFovTwice_returnsUpdatedFov() {
        val rtFov = RtCameraViewActivityPose.Fov(1f, 2f, 3f, 4f)
        // Set the fov in the camera view.
        (fakeRuntime.getCameraViewActivityPose(
                RtCameraViewActivityPose.CameraType.CAMERA_TYPE_LEFT_EYE
            ) as? FakeCameraViewActivityPose)
            ?.fov = rtFov

        assertThat(camera!!.fov).isEqualTo(FieldOfView(1f, 2f, 3f, 4f))

        val rtFov2 = RtCameraViewActivityPose.Fov(5f, 6f, 7f, 8f)
        // Set the fov in the camera view.
        (fakeRuntime.getCameraViewActivityPose(
                RtCameraViewActivityPose.CameraType.CAMERA_TYPE_LEFT_EYE
            ) as? FakeCameraViewActivityPose)
            ?.fov = rtFov2

        assertThat(camera!!.fov).isEqualTo(FieldOfView(5f, 6f, 7f, 8f))
    }
}
