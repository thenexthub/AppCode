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

package androidx.glance.wear.parcel

import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.os.Bundle
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.text.BasicText
import androidx.compose.remote.creation.compose.action.pendingIntentAction
import androidx.compose.remote.creation.compose.capture.CreationDisplayInfo
import androidx.compose.remote.creation.compose.capture.RemoteComposeCapture
import androidx.compose.remote.creation.compose.layout.RemoteBox
import androidx.compose.remote.creation.compose.layout.RemoteColumn
import androidx.compose.remote.creation.compose.layout.RemoteText
import androidx.compose.remote.creation.compose.modifier.RemoteModifier
import androidx.compose.remote.creation.compose.modifier.fillMaxSize
import androidx.compose.remote.creation.compose.modifier.onClick
import androidx.compose.remote.creation.compose.modifier.size
import androidx.compose.remote.player.core.RemoteComposeDocument
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.semantics.SemanticsProperties
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.getOrNull
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.test.ExperimentalTestApi
import androidx.compose.ui.test.SemanticsNodeInteraction
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.compose.ui.test.onNodeWithContentDescription
import androidx.compose.ui.unit.dp
import androidx.core.os.BundleCompat
import androidx.test.core.app.ApplicationProvider
import com.google.common.truth.Truth.assertThat
import kotlinx.coroutines.test.StandardTestDispatcher
import org.junit.Rule
import org.junit.Test

class WearWidgetCaptureTest {

    @OptIn(ExperimentalTestApi::class)
    @get:Rule
    val composeTestRule = createComposeRule(StandardTestDispatcher())

    companion object {
        val context: Context = ApplicationProvider.getApplicationContext()
        val testPendingIntent0 =
            PendingIntent.getActivity(context, 1234, Intent(), PendingIntent.FLAG_IMMUTABLE)
        val testPendingIntent1 =
            PendingIntent.getActivity(context, 5678, Intent(), PendingIntent.FLAG_IMMUTABLE)

        @Composable
        fun TestLayout() {
            RemoteColumn(modifier = RemoteModifier.fillMaxSize()) {
                RemoteBox(modifier = RemoteModifier.size(100.dp))
                RemoteText(
                    text = "text-0",
                    modifier = RemoteModifier.onClick(pendingIntentAction(testPendingIntent0)),
                )
                RemoteText(
                    text = "text-1",
                    modifier = RemoteModifier.onClick(pendingIntentAction(testPendingIntent1)),
                )
            }
        }

        @Composable
        internal fun CollectPendingIntent(
            widgetPendingIntents: WidgetPendingIntents,
            content: @Composable () -> Unit,
        ) {
            val creationDisplayInfo =
                CreationDisplayInfo(400, 400, LocalConfiguration.current.densityDpi)
            RemoteComposeCapture(
                LocalContext.current,
                creationDisplayInfo,
                true,
                { view, writer -> true },
                @Composable {},
                WearWidgetProfile(widgetPendingIntents),
                @Composable { content() },
            )
        }

        @Composable
        internal fun CaptureWidgetContentData(content: @Composable () -> Unit) {
            val creationDisplayInfo = CreationDisplayInfo(400, 400, 320)
            val context = LocalContext.current

            val data = remember { mutableStateOf<WearWidgetRawContent?>(null) }
            LaunchedEffect(Unit) {
                data.value =
                    WearWidgetCapture.capture(
                        context,
                        creationDisplayInfo,
                        @Composable { content() },
                    )
            }

            Column {
                BasicText(
                    modifier =
                        androidx.compose.ui.Modifier.semantics { contentDescription = "Document" },
                    text =
                        data.value?.rcDocument?.let { rcDoc ->
                            RemoteComposeDocument(rcDoc).document.displayHierarchy()
                        } ?: "",
                )
                BasicText(
                    modifier =
                        androidx.compose.ui.Modifier.semantics {
                            contentDescription = "PendingIntent 0"
                        },
                    text = data.value?.extras?.getPendingIntent(0)?.toString() ?: "",
                )
                BasicText(
                    modifier =
                        androidx.compose.ui.Modifier.semantics {
                            contentDescription = "PendingIntent 1"
                        },
                    text = data.value?.extras?.getPendingIntent(1)?.toString() ?: "",
                )
            }
        }

        private fun SemanticsNodeInteraction.assertTextMatches(expected: String) {
            val text =
                fetchSemanticsNode()
                    .config
                    .getOrNull(SemanticsProperties.Text)
                    ?.firstOrNull()
                    ?.toString()
            assertThat(text!!.normalizeWhiteSpace()).isEqualTo(expected.normalizeWhiteSpace())
        }

        // Replace all sequences of whitespace (including newlines, tabs) with a single space. Then
        // trim leading/trailing spaces from the whole string
        private fun String.normalizeWhiteSpace() = this.replace(Regex("``s+"), " ").trim()

        private fun Bundle.getPendingIntent(index: Int): PendingIntent? {
            val pendingIntents =
                BundleCompat.getParcelable(
                    this,
                    WearWidgetCapture.PENDING_INTENT_KEY,
                    Bundle::class.java,
                )
            return BundleCompat.getParcelable(
                pendingIntents!!,
                index.toString(),
                PendingIntent::class.java,
            )
        }
    }

    @Test
    fun pendingIntentCollection() {
        var pendingIntents = WidgetPendingIntents()
        composeTestRule.setContent { CollectPendingIntent(pendingIntents) { TestLayout() } }

        assertThat(pendingIntents.size()).isEqualTo(2)
        assertThat(pendingIntents.get(0)).isEqualTo(testPendingIntent0)
        assertThat(pendingIntents.get(1)).isEqualTo(testPendingIntent1)
    }

    @Test
    fun pendingIntentCollection_addToBundle() {
        val result =
            """
DATA_TEXT<42> = "text-0"
DATA_TEXT<44> = "text-1"
ROOT [-2:-1] = [0.0, 0.0, 0.0, 0.0] VISIBLE
  COLUMN [-3:-1] = [0.0, 0.0, 0.0, 0.0] VISIBLE
    MODIFIERS
    BOX [-5:-1] = [0.0, 0.0, 200.0, 200.0] VISIBLE
      MODIFIERS
        WIDTH = 200.0
        HEIGHT = 200.0
    TEXT_LAYOUT [-7:-1] = [0.0, 0.0, 0.0, 0.0] VISIBLE (42:"null")
      MODIFIERS
        CLICK_MODIFIER
          HOST_NAMED_ACTION = 46 : 43
        SEMANTICS = SEMANTICS BUTTON
    TEXT_LAYOUT [-9:-1] = [0.0, 0.0, 0.0, 0.0] VISIBLE (44:"null")
      MODIFIERS
        CLICK_MODIFIER
          HOST_NAMED_ACTION = 46 : 45
        SEMANTICS = SEMANTICS BUTTON
"""

        composeTestRule.setContent { CaptureWidgetContentData { TestLayout() } }

        composeTestRule.onNodeWithContentDescription("Document").assertTextMatches(result)
        composeTestRule
            .onNodeWithContentDescription("PendingIntent 0")
            .assertTextMatches(testPendingIntent0.toString())
        composeTestRule
            .onNodeWithContentDescription("PendingIntent 1")
            .assertTextMatches(testPendingIntent1.toString())
    }
}
