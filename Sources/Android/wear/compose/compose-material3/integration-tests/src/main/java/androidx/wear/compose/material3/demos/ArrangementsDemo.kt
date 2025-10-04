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

package androidx.wear.compose.material3.demos

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.aspectRatio
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.text.BasicText
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.wear.compose.foundation.CurvedLayout
import androidx.wear.compose.foundation.CurvedModifier
import androidx.wear.compose.foundation.angularSizeDp
import androidx.wear.compose.foundation.curvedBox
import androidx.wear.compose.foundation.curvedComposable
import androidx.wear.compose.foundation.lazy.ScalingLazyColumn
import androidx.wear.compose.foundation.lazy.TransformingLazyColumn
import androidx.wear.compose.material3.Button
import androidx.wear.compose.material3.ButtonDefaults
import androidx.wear.compose.material3.CompactButton
import androidx.wear.compose.material3.Text
import androidx.wear.compose.material3.lazy.rememberTransformationSpec
import androidx.wear.compose.material3.lazy.transformedHeight
import kotlin.math.max

@Composable
private fun ArrangementsDemoList(
    listIx: Int,
    verticalArrangement: Arrangement.Vertical,
    itemCount: Int,
) {
    @Composable
    fun demoItem(it: Int, modifier: Modifier = Modifier) {
        BasicText(
            "Item $it",
            modifier = modifier.background(Color.DarkGray).width(80.dp).padding(5.dp),
            style = TextStyle(color = Color.White, textAlign = TextAlign.Center),
        )
    }
    if (listIx == 0) {
        LazyColumn(
            Modifier.fillMaxSize(),
            verticalArrangement = verticalArrangement,
            horizontalAlignment = Alignment.CenterHorizontally,
        ) {
            items(itemCount) { demoItem(it) }
        }
    } else if (listIx == 1) {
        ScalingLazyColumn(
            Modifier.fillMaxSize(),
            autoCentering = null,
            verticalArrangement = verticalArrangement,
        ) {
            items(itemCount) { demoItem(it) }
        }
    } else {
        val spec = rememberTransformationSpec()
        TransformingLazyColumn(Modifier.fillMaxSize(), verticalArrangement = verticalArrangement) {
            items(itemCount) {
                demoItem(
                    it,
                    Modifier.transformedHeight(this, spec).graphicsLayer {
                        with(spec) {
                            applyContainerTransformation(scrollProgress)
                            applyContentTransformation(scrollProgress)
                        }
                    },
                )
            }
        }
    }
}

@Composable
fun ArrangementsDemo() {
    val lists =
        listOf(
            "LC" to "LazyColumn",
            "SLC" to "ScalingLazyColumn",
            "TLC" to "TransformingLazyColumn",
        )
    var listsIx by remember { mutableIntStateOf(0) }
    val spacing = 10.dp
    val arrs =
        listOf(
            AlignmentDemoItem("^", "Arrangement.Top", Arrangement.Top),
            AlignmentDemoItem("=", "Arrangement.Center", Arrangement.Center),
            AlignmentDemoItem("v", "Arrangement.Bottom", Arrangement.Bottom),
            AlignmentDemoItem(
                "Sp ^",
                "Arrangement.spacedBy($spacing, Alignment.Top)",
                Arrangement.spacedBy(spacing, Alignment.Top),
            ),
            AlignmentDemoItem(
                "Sp =",
                "Arrangement.spacedBy($spacing, Alignment.CenterVertically)",
                Arrangement.spacedBy(spacing, Alignment.CenterVertically),
            ),
            AlignmentDemoItem(
                "Sp v",
                "Arrangement.spacedBy($spacing, Alignment.Bottom)",
                Arrangement.spacedBy(spacing, Alignment.Bottom),
            ),
        )
    var arrsIx by remember { mutableIntStateOf(0) }
    var itemCount by remember { mutableIntStateOf(4) }

    val screenSizeDp =
        with(LocalDensity.current) {
            with(LocalConfiguration.current) { max(screenWidthDp, screenHeightDp) }
        }
    @Composable
    fun MinusItemsButton(modifier: Modifier = Modifier) {
        CompactButton(onClick = { itemCount = (itemCount - 1).coerceAtLeast(1) }, modifier) {
            BasicText("-", style = TextStyle(color = Color.Black))
        }
    }

    @Composable
    fun ItemsTextDisplay(modifier: Modifier = Modifier) {
        Text(itemCount.toString(), modifier, color = Color.White, textAlign = TextAlign.Center)
    }

    @Composable
    fun PlusItemsButton(modifier: Modifier = Modifier) {
        CompactButton(
            onClick = {
                itemCount++ // The sky is the limit!
            },
            modifier,
        ) {
            BasicText("+", style = TextStyle(color = Color.Black))
        }
    }

    if (screenSizeDp > 400) {
        // Phone
        Column(Modifier.fillMaxSize(), verticalArrangement = Arrangement.spacedBy(10.dp)) {
            Box(
                Modifier.aspectRatio(1f)
                    .fillMaxSize()
                    .clip(shape = CircleShape)
                    .border(2.dp, Color.White, shape = CircleShape)
            ) {
                ArrangementsDemoList(listsIx, arrs[arrsIx].arrangement, itemCount)
            }

            Row(
                Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(10.dp, Alignment.Start),
            ) {
                arrs.forEachIndexed { ix, it ->
                    Button(
                        onClick = { arrsIx = ix },
                        modifier =
                            if (ix == arrsIx)
                                Modifier.border(2.dp, Color.Green, shape = ButtonDefaults.shape)
                            else Modifier,
                    ) {
                        BasicText(it.buttonText, style = TextStyle(color = Color.Black))
                    }
                }
            }
            BasicText(arrs[arrsIx].description, style = TextStyle(color = Color.White))
            Spacer(Modifier.size(5.dp))
            Row(
                Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(10.dp, Alignment.Start),
            ) {
                lists.forEachIndexed { ix, it ->
                    Button(
                        onClick = { listsIx = ix },
                        modifier =
                            if (ix == listsIx)
                                Modifier.border(2.dp, Color.Green, shape = ButtonDefaults.shape)
                            else Modifier,
                    ) {
                        BasicText(it.first, style = TextStyle(color = Color.Black))
                    }
                }
            }
            BasicText(lists[listsIx].second, style = TextStyle(color = Color.White))
            Spacer(Modifier.size(5.dp))
            Row(Modifier.fillMaxWidth(), verticalAlignment = Alignment.CenterVertically) {
                MinusItemsButton()
                ItemsTextDisplay(Modifier.weight(1f))
                PlusItemsButton()
            }
        }
    } else {
        // Watch
        Box(Modifier.fillMaxSize()) {
            ArrangementsDemoList(listsIx, arrs[arrsIx].arrangement, itemCount)

            val buttonSize = 60.dp
            CurvedLayout(anchor = 180f) {
                curvedComposable(
                    rotationLocked = true,
                    modifier = CurvedModifier.angularSizeDp(buttonSize),
                ) {
                    Button(onClick = { listsIx = (listsIx + 1) % lists.size }) {
                        BasicText(lists[listsIx].first, style = TextStyle(color = Color.Black))
                    }
                }
                curvedBox(CurvedModifier.angularSizeDp(10.dp)) {}
                curvedComposable(
                    rotationLocked = true,
                    modifier = CurvedModifier.angularSizeDp(buttonSize),
                ) {
                    Button(onClick = { arrsIx = (arrsIx + 1) % arrs.size }) {
                        BasicText(arrs[arrsIx].buttonText, style = TextStyle(color = Color.Black))
                    }
                }
            }
            Column(
                Modifier.align(Alignment.CenterEnd).padding(end = (screenSizeDp * 8 / 100).dp),
                verticalArrangement = Arrangement.spacedBy(5.dp),
                horizontalAlignment = Alignment.CenterHorizontally,
            ) {
                PlusItemsButton()
                ItemsTextDisplay()
                MinusItemsButton()
            }
        }
    }
}

private data class AlignmentDemoItem(
    val buttonText: String,
    val description: String,
    val arrangement: Arrangement.Vertical,
)
