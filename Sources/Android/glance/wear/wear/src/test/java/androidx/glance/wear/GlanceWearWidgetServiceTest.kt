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

package androidx.glance.wear

import android.content.Context
import android.content.Intent
import android.os.IBinder
import androidx.compose.remote.creation.compose.layout.RemoteText
import androidx.glance.wear.parcel.IWearWidgetCallback
import androidx.glance.wear.parcel.IWearWidgetProvider
import androidx.glance.wear.parcel.legacy.TileProvider
import com.google.common.truth.Truth.assertThat
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.kotlin.mock
import org.robolectric.Robolectric
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class GlanceWearWidgetServiceTest {

    private val mockWidgetCallback = mock<IWearWidgetCallback>()

    @Test
    fun onBind_withWidgetIntent_returnsWidgetProvider() {
        val service: TestService = Robolectric.setupService(TestService::class.java)

        val binder: IBinder? =
            service.onBind(Intent(GlanceWearWidgetService.ACTION_BIND_WIDGET_PROVIDER))

        assertThat(binder).isInstanceOf(IWearWidgetProvider::class.java)
    }

    @Test
    fun onBind_withTileIntentAndSupportsWidgetProvider_returnsWidgetProvider() {
        val service: TestService = Robolectric.setupService(TestService::class.java)
        val bindIntent =
            Intent(GlanceWearWidgetService.ACTION_BIND_TILE_PROVIDER).apply {
                putExtra(GlanceWearWidgetService.EXTRA_KEY_WEAR_WIDGET_PROVIDER_SUPPORTED, true)
            }

        val binder: IBinder? = service.onBind(bindIntent)

        assertThat(binder).isInstanceOf(IWearWidgetProvider::class.java)
    }

    @Test
    fun onBind_withTileIntentAndDoesNotSupportWidgetProvider_returnsLegacyProvider() {
        val service: TestService = Robolectric.setupService(TestService::class.java)
        val bindIntent =
            Intent(GlanceWearWidgetService.ACTION_BIND_TILE_PROVIDER).apply {
                putExtra(GlanceWearWidgetService.EXTRA_KEY_WEAR_WIDGET_PROVIDER_SUPPORTED, false)
            }

        val binder: IBinder? = service.onBind(bindIntent)

        assertThat(binder).isInstanceOf(TileProvider::class.java)
    }

    @Test
    fun onBind_withTileIntentAndNoExtras_returnsLegacyProvider() {
        val service: TestService = Robolectric.setupService(TestService::class.java)
        val bindIntent = Intent(GlanceWearWidgetService.ACTION_BIND_TILE_PROVIDER)

        val binder: IBinder? = service.onBind(bindIntent)

        assertThat(binder).isInstanceOf(TileProvider::class.java)
    }

    @Test
    fun onBind_withWrongIntent_returnsNull() {
        val service: TestService = Robolectric.setupService(TestService::class.java)

        val binder: IBinder? = service.onBind(Intent())

        assertThat(binder).isNull()
    }

    @Suppress("RestrictedApiAndroidX")
    class TestWidget : GlanceWearWidget() {
        var instanceId: Int? = null

        override suspend fun provideWidgetContent(
            context: Context,
            request: WearWidgetRequest,
        ): WearWidgetContent {
            instanceId = request.instanceId
            return WearWidgetContent { RemoteText("Testing...") }
        }
    }

    private class TestService() : GlanceWearWidgetService() {
        override val widget = TestWidget()
    }
}
