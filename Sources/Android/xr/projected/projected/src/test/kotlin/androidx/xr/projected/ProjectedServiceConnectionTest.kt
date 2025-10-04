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

import android.app.Application
import android.content.ComponentName
import android.content.IntentFilter
import android.content.pm.ApplicationInfo
import android.content.pm.PackageInfo
import android.content.pm.ServiceInfo
import android.os.IBinder
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.testing.TestLifecycleOwner
import androidx.test.core.app.ApplicationProvider
import androidx.xr.projected.IProjectedService
import androidx.xr.projected.ProjectedServiceBinding.ACTION_BIND
import androidx.xr.projected.ProjectedServiceConnection
import com.google.common.truth.Truth.assertThat
import kotlin.test.BeforeTest
import kotlin.test.assertFailsWith
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.launch
import kotlinx.coroutines.test.TestScope
import kotlinx.coroutines.test.UnconfinedTestDispatcher
import kotlinx.coroutines.test.runTest
import kotlinx.coroutines.test.setMain
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.Mockito.mock
import org.robolectric.RobolectricTestRunner
import org.robolectric.Shadows.shadowOf

@OptIn(ExperimentalCoroutinesApi::class)
@RunWith(RobolectricTestRunner::class)
class ProjectedServiceConnectionTest {

    private val dispatcher = UnconfinedTestDispatcher()

    private lateinit var context: Application
    private lateinit var testScope: TestScope
    private lateinit var lifecycleOwner: TestLifecycleOwner

    @BeforeTest
    fun setMainDispatcher() {
        Dispatchers.setMain(dispatcher)
    }

    @Before
    fun setUp() {
        context = ApplicationProvider.getApplicationContext()
        testScope = TestScope(dispatcher)
        lifecycleOwner = TestLifecycleOwner(Lifecycle.State.INITIALIZED, dispatcher)
        shadowOf(context.packageManager).apply {
            addServiceIfNotPresent(COMPONENT_NAME)
            addOrUpdateService(SERVICE_INFO)
            addIntentFilterForService(COMPONENT_NAME, IntentFilter(ACTION_BIND))
            installPackage(PACKAGE_INFO)
        }
        shadowOf(context)
            .setComponentNameAndServiceForBindService(COMPONENT_NAME, mock(IBinder::class.java))
    }

    @Test
    fun connect_bindsAndConnects_returnsInstance() =
        testScope.runTest {
            shadowOf(context).setBindServiceCallsOnServiceConnectedDirectly(true)

            val service = ProjectedServiceConnection.connect(context, lifecycleOwner)
            assertThat(service).isInstanceOf(IProjectedService::class.java)
        }

    @Test
    fun connect_doesNotBind_throwsException() =
        testScope.runTest {
            shadowOf(context).declareComponentUnbindable(COMPONENT_NAME)

            val unused =
                assertFailsWith<IllegalStateException> {
                    ProjectedServiceConnection.connect(context, lifecycleOwner)
                }
        }

    @Test
    fun onDestroy_unbinds() =
        testScope.runTest {
            lifecycleOwner.handleLifecycleEvent(Lifecycle.Event.ON_RESUME)

            backgroundScope.launch {
                val unused = ProjectedServiceConnection.connect(context, lifecycleOwner)
            }

            lifecycleOwner.handleLifecycleEvent(Lifecycle.Event.ON_DESTROY)
            assertThat(shadowOf(context).unboundServiceConnections).isNotEmpty()
        }

    companion object {
        private const val SYSTEM_PACKAGE_NAME = "com.system.service"
        private const val SYSTEM_CLASS_NAME = "com.system.service.ProjectedService"
        private val COMPONENT_NAME = ComponentName(SYSTEM_PACKAGE_NAME, SYSTEM_CLASS_NAME)
        private val SERVICE_INFO =
            ServiceInfo().apply {
                packageName = SYSTEM_PACKAGE_NAME
                name = SYSTEM_CLASS_NAME
            }
        private val PACKAGE_INFO =
            PackageInfo().apply {
                packageName = SYSTEM_PACKAGE_NAME
                services = arrayOf(SERVICE_INFO)
                applicationInfo = ApplicationInfo().apply { flags = ApplicationInfo.FLAG_SYSTEM }
            }
    }
}
