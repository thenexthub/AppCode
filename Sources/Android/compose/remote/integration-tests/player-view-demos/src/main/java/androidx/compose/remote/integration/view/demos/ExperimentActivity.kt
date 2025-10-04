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

package androidx.compose.remote.integration.view.demos

import android.annotation.SuppressLint
import android.appwidget.AppWidgetManager
import android.content.Context
import android.content.ContextWrapper
import android.content.Intent
import android.content.res.Configuration
import android.os.Build
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.annotation.RequiresApi
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Checkbox
import androidx.compose.material3.SecondaryTabRow
import androidx.compose.material3.Surface
import androidx.compose.material3.Tab
import androidx.compose.material3.Text
import androidx.compose.remote.core.CoreDocument
import androidx.compose.remote.core.Profiles.PROFILE_WIDGETS
import androidx.compose.remote.core.RemoteComposeBuffer
import androidx.compose.remote.core.operations.Theme
import androidx.compose.remote.creation.RemoteComposeContext
import androidx.compose.remote.creation.RemoteComposeWriter
import androidx.compose.remote.creation.compose.capture.CreationDisplayInfo
import androidx.compose.remote.creation.compose.capture.RemoteComposeCapture
import androidx.compose.remote.integration.view.demos.examples.RcSimpleClock1
import androidx.compose.remote.integration.view.demos.examples.SimplePath
import androidx.compose.remote.integration.view.demos.examples.WeatherDemo
import androidx.compose.remote.integration.view.demos.examples.countDown
import androidx.compose.remote.integration.view.demos.examples.cube3d
import androidx.compose.remote.integration.view.demos.utils.RCDoc
import androidx.compose.remote.player.core.RemoteComposeDocument
import androidx.compose.remote.player.view.RemoteComposePlayer
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.ComposeView
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import java.io.ByteArrayInputStream
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.IOException
import java.util.zip.DeflaterOutputStream
import kotlin.math.roundToInt
import kotlin.random.Random
import kotlinx.coroutines.delay

const val DEFAULT_SHOW_REMOTE = true
const val DEFAULT_SHOW_COMPOSE = false
const val DEFAULT_SHOW_COMPOSE_PLAYER = true
const val DEFAULT_DEBUG_REMOTE_COMPOSE = false
const val DELAY_IN_MS = 2000L

@Suppress("RestrictedApiAndroidX") var launcher: RemoteComposeBuffer = countDown().buffer

@Suppress("RestrictedApiAndroidX")
fun launcherDoc(): RemoteComposeBuffer {
    return launcher
}

var remoteIDs: IntArray = IntArray(0)

fun getRemoteComposables(context: Context, list: ArrayList<RCDoc>) {

    list.addAll(arrayListOf())
}

@Suppress("RestrictedApiAndroidX")
fun getComposeDoc(
    context: Context,
    name: String,
    color: Color = toRcColor(name),
    cRun: @Composable () -> Unit,
): RCDoc {
    return object : RCDoc {
        var remoteComposeDocument: RemoteComposeDocument? = null
        var document: CoreDocument? = null
        var created = false

        override fun run() {}

        override fun getDoc(): RemoteComposeDocument? {
            if (remoteComposeDocument != null) {
                return remoteComposeDocument!!
            }
            if (!created) {
                created = true
                rememberRemoteDocument(context, cRun)
            }
            if (document != null && document is CoreDocument) {
                remoteComposeDocument = RemoteComposeDocument(document!!)
            }
            return remoteComposeDocument
        }

        override fun getColor(): Int {
            return color.toArgb()
        }

        override fun size(): Int {
            val rcd = remoteComposeDocument
            if (rcd == null) {
                return 0
            }
            return rcd.document.buffer.buffer.size
        }

        override fun toString(): String {
            return name
        }

        fun rememberRemoteDocument(baseContext: Context, content: @Composable () -> Unit) {
            rememberRemoteDocument(baseContext, 6, PROFILE_WIDGETS, content)
        }

        // @Composable
        fun rememberRemoteDocument(
            baseContext: Context,
            apiLevel: Int,
            profiles: Int,
            content: @Composable () -> Unit,
        ) {
            //        val doc: MutableState<CoreDocument?> = remember { mutableStateOf(null) }
            //        val density = with(LocalDensity.current) { 1.dp.toPx() * 160 }
            val connection = CreationDisplayInfo(1000, 1000, 440)
            //        val done = remember { mutableStateOf(false) }
            RemoteComposeCapture(
                baseContext,
                connection,
                true,
                { view, writer ->
                    if (document == null) {
                        val buffer = writer.buffer()
                        val bufferSize = writer.bufferSize()
                        val inputStream = ByteArrayInputStream(buffer, 0, bufferSize)
                        val coreDocument = CoreDocument()
                        val rcBuffer = RemoteComposeBuffer.fromInputStream(inputStream)
                        coreDocument.initFromBuffer(rcBuffer)
                        document = coreDocument
                    }
                    true
                },
                @Composable {},
                apiLevel,
                profiles,
                @Composable { content() },
            )
        }
    }
}

/** Display a list of samples and run them */
@Suppress("RestrictedApiAndroidX")
class ExperimentActivity : ComponentActivity() {
    val composeKey = "USE_COMPOSE"
    val showComposeKey = "SHOW_COMPOSE"
    val showComposePlayerKey = "SHOW_COMPOSE_PLAYER"
    val showOrigamiKey = "SHOW_ORIGAMI"
    val debugComposeKey = "DEBUG_ORIGAMI"

    fun setLauncherDoc(doc: RemoteComposeBuffer) {
        val appWidgetManager = AppWidgetManager.getInstance(applicationContext)
        val buffer = doc.buffer
        val bufferSize = doc.buffer.size
        val bytes = ByteArray(bufferSize)
        ByteArrayInputStream(buffer.buffer, 0, bufferSize).read(bytes)
    }

    var cmap = listOf(get("Frontend...") {}, get("Procedural...") {})

    var subMenus =
        mapOf<String, List<RemoteComposeFunc>>(
            "Frontend..." to
                listOf(
                    get("SimplePath") { SimplePath() },
                    get("WeatherDemo") { WeatherDemo() },
                    get("Simple Clock") { RcSimpleClock1() },
                ),
            "Procedural..." to
                listOf(getpc("CountDown") { countDown() }, getpc("Cube 3D") { cube3d() }),
        )

    fun getpc(
        name: String,
        color: Color = toRcColor(name, 0.1f),
        gen: () -> RemoteComposeContext,
    ): RemoteComposeFunc {
        return getp(name, color) { gen().mRemoteWriter }
    }

    fun getp(
        name: String,
        color: Color = toRcColor(name, 0.1f),
        gen: () -> RemoteComposeWriter,
    ): RemoteComposeFunc {
        return object : RemoteComposeFunc {
            @Composable
            override fun Run() {
                println()
            }

            @Composable
            override fun getDoc(): MutableState<CoreDocument?> {
                val doc =
                    RemoteComposeDocument(
                        ByteArrayInputStream(gen().buffer(), 0, gen().bufferSize())
                    )
                val doc2: MutableState<CoreDocument?> = remember { mutableStateOf(doc.document) }
                return doc2
            }

            override fun getColor(): Color {
                return color
            }

            override fun toString(): String {
                return name
            }
        }
    }

    fun get(
        name: String,
        color: Color = toRcColor(name),
        cRun: @Composable () -> Unit,
    ): RemoteComposeFunc {
        return object : RemoteComposeFunc {
            @Composable
            override fun Run() {
                cRun()
            }

            @Composable
            override fun getDoc(): MutableState<CoreDocument?> {
                return rememberRemoteDocument(cRun)
            }

            override fun getColor(): Color {
                return color
            }

            override fun toString(): String {
                return name
            }

            @Composable
            fun rememberRemoteDocument(
                content: @Composable () -> Unit
            ): MutableState<CoreDocument?> {
                return rememberRemoteDocument(CoreDocument.DOCUMENT_API_LEVEL, 0, content)
            }

            @Composable
            fun rememberRemoteDocument(
                apiLevel: Int,
                profiles: Int,
                content: @Composable () -> Unit,
            ): MutableState<CoreDocument?> {
                val doc: MutableState<CoreDocument?> = remember { mutableStateOf(null) }
                val density = with(LocalDensity.current) { 1.dp.toPx() * 160 }
                val connection = CreationDisplayInfo(1000, 1000, density.toInt())
                val done = remember { mutableStateOf(false) }
                RemoteComposeCapture(
                    LocalContext.current,
                    connection,
                    true,
                    { view, writer ->
                        if (!done.value) {
                            val buffer = writer.buffer()
                            val bufferSize = writer.bufferSize()
                            val inputStream = ByteArrayInputStream(buffer, 0, bufferSize)
                            val document = CoreDocument()
                            val rcBuffer = RemoteComposeBuffer.fromInputStream(inputStream)
                            document.initFromBuffer(rcBuffer)
                            doc.value = document
                            done.value = true
                        }
                        done.value
                    },
                    @Composable {},
                    apiLevel,
                    profiles,
                    @Composable { content() },
                )
                return doc
            }
        }
    }

    /** Runs the menu if no Bundle containing what to run */
    @RequiresApi(Build.VERSION_CODES.TIRAMISU)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val fullList = cmap.toMutableList()
        fullList.addAll(subMenus.values.flatten())

        val extra = intent.extras
        var cfunc: RemoteComposeFunc? = null
        var showCompose = false
        var showComposePlayer = true
        var showOrigami = true

        var menu = cmap
        if (extra != null) {
            val composeName = extra.getString(composeKey)
            val showComposeStr = extra.getString(showComposeKey)
            val showComposePlayerStr = extra.getString(showComposePlayerKey)
            val showOrigamiStr = extra.getString(showOrigamiKey)
            showCompose = "true".equals(showComposeStr)
            showComposePlayer = "true".equals(showComposePlayerStr)
            showOrigami = "true".equals(showOrigamiStr)
            setTitle(composeName)
            if (composeName?.endsWith("...") == true) {
                menu = subMenus.get(composeName) ?: menu
            } else {
                for (remoteComposeFunc in fullList) {
                    if (remoteComposeFunc.toString() == composeName) {
                        cfunc = remoteComposeFunc
                        break
                    }
                }
            }
        }

        val com = ComposeView(this)
        setContentView(com)
        com.setContent {
            Surface(modifier = Modifier.fillMaxSize(), color = Color(0xFFFFFFFF)) {
                if (cfunc != null) {
                    Log.v("MAIN", " running $cfunc")
                    rcRun(cfunc, showOrigami, showCompose, showComposePlayer)
                } else {
                    RemoteComposableMenu(map = menu) {
                        act,
                        showOrigami,
                        showCompose,
                        showComposePlayer,
                        debugCompose ->
                        launch(act, showOrigami, showCompose, showComposePlayer, debugCompose)
                    }
                }
            }
        }
    }

    private fun launch(
        to_run: RemoteComposeFunc,
        showOrigami: Boolean,
        showCompose: Boolean,
        showComposePlayer: Boolean,
        debugCompose: Boolean,
    ) {
        Log.v("MAIN", " launch $to_run")
        val intent = Intent(this, ExperimentActivity::class.java)
        intent.putExtra(composeKey, to_run.toString())
        intent.putExtra(showComposeKey, "$showCompose")
        intent.putExtra(showComposePlayerKey, "$showComposePlayer")
        intent.putExtra(showOrigamiKey, "$showOrigami")
        intent.putExtra(debugComposeKey, "$debugCompose")
        startActivity(intent)
    }

    private fun rcRun(
        func: RemoteComposeFunc,
        showOrigami: Boolean,
        showCompose: Boolean,
        showComposePlayer: Boolean,
    ) {
        val file = File(applicationContext.filesDir, "$func.rc")

        // Delete the file so that we can wait for it to be generated and then load and display it
        file.delete()

        setContent {
            var fileReady by remember { mutableStateOf(true) }
            Column(
                modifier =
                    Modifier.padding(16.dp)
                        // .verticalScroll(rememberScrollState()) // TODO investigate why this
                        // breaks
                        .background(Color.White)
            ) {
                var tabIndex by remember { mutableIntStateOf(0) }
                val tabs = listOf("UI", "Stats", "Doc", "misc")

                Text(text = "$func", textAlign = TextAlign.Center)

                SecondaryTabRow(
                    tabIndex,
                    tabs = {
                        tabs.forEachIndexed { index, title ->
                            Tab(
                                text = { Text(title) },
                                selected = tabIndex == index,
                                onClick = { tabIndex = index },
                            )
                        }
                    },
                )
                when (tabIndex) {
                    0 -> DisplayMain(fileReady, showOrigami, showCompose, showComposePlayer, func)
                    1 -> DisplayStats(fileReady, func)
                    2 -> DisplayDoc(fileReady, func)
                    3 ->
                        DisplayControls(fileReady, name = func.toString(), func, applicationContext)
                }
            }
        }
    }
}

@Suppress("RestrictedApiAndroidX")
@Composable
fun DisplayControls(fileReady: Boolean, name: String, func: RemoteComposeFunc, context: Context) {
    val orientation = LocalConfiguration.current.orientation

    if (fileReady) {
        var playbackTheme by remember { mutableIntStateOf(Theme.UNSPECIFIED) }
        var debugMode by remember { mutableIntStateOf(0) }
        var documentWidth = remember { mutableIntStateOf(300) }
        var documentHeight = remember { mutableIntStateOf(300) }
        val currentDocument = func.getDoc()

        if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
            Row {
                DocumentView(
                    documentWidth,
                    documentHeight,
                    currentDocument,
                    playbackTheme,
                    debugMode,
                )
            }
        } else {
            Column {
                DocumentView(
                    documentWidth,
                    documentHeight,
                    currentDocument,
                    playbackTheme,
                    debugMode,
                )
            }
        }
    }
}

inline fun <reified Activity : ComponentActivity> Context.getActivity(): Activity? {
    return when (this) {
        is Activity -> this
        else -> {
            var context = this
            while (context is ContextWrapper) {
                context = context.baseContext
                if (context is Activity) return context
            }
            null
        }
    }
}

@Suppress("RestrictedApiAndroidX")
@Composable
private fun DocumentView(
    documentWidth: MutableState<Int>,
    documentHeight: MutableState<Int>,
    currentDocument: MutableState<CoreDocument?>,
    playbackTheme: Int,
    debugMode: Int,
) {
    Box {
        AndroidView(
            modifier = Modifier.size(documentWidth.value.dp, documentHeight.value.dp),
            factory = {
                val player = RemoteComposePlayer(it)
                if (currentDocument.value != null) {
                    player.setDocument(RemoteComposeDocument(currentDocument.value!!))
                }
                player.addIdActionListener { _id, _metadata -> println("click $_id $_metadata") }
                player
            },
            update = {
                it.setTheme(playbackTheme)
                if (currentDocument.value != null) {
                    it.setDocument(RemoteComposeDocument(currentDocument.value!!))
                }
                it.setDebug(debugMode)
            },
        )

        Box(
            Modifier.offset {
                    IntOffset(
                        documentWidth.value.dp.toPx().toInt(),
                        documentHeight.value.dp.toPx().toInt(),
                    )
                }
                .background(Color.Green)
                .size(30.dp)
                .pointerInput(Unit) {
                    detectDragGestures { change, dragAmount ->
                        change.consume()
                        documentWidth.value += dragAmount.x.toDp().value.toInt()
                        documentHeight.value += dragAmount.y.toDp().value.toInt()
                    }
                }
        )
    }
}

@SuppressLint("AutoboxingStateValueProperty")
@Suppress("RestrictedApiAndroidX")
@Composable
fun DisplayStats(fileReady: Boolean, func: RemoteComposeFunc) {
    if (fileReady) {
        var playbackTheme by remember { mutableIntStateOf(Theme.UNSPECIFIED) }
        var debugMode by remember { mutableIntStateOf(0) }
        var documentWidth by remember { mutableIntStateOf(300) }
        var documentHeight by remember { mutableIntStateOf(300) }
        val currentDocument = func.getDoc() // remember(func) {  }
        val stats = currentDocument.value?.stats
        val docSize = currentDocument.value?.let { docSize(it) }
        val zipSize = currentDocument.value?.let { compressDocSize(it) }
        val playerRef = remember { mutableStateOf<RemoteComposePlayer?>(null) }
        val evalTime = remember { mutableFloatStateOf(0f) }

        LaunchedEffect(Unit) {
            while (true) {
                delay(DELAY_IN_MS) // Delay for 2 seconds
                evalTime.value = playerRef.value?.evalTime ?: -1f
            }
        }

        Column {
            AndroidView(
                modifier = Modifier.size(documentWidth.dp, documentHeight.dp),
                factory = {
                    val player = RemoteComposePlayer(it)
                    if (currentDocument.value != null) {
                        player.setDocument(RemoteComposeDocument(currentDocument.value!!))
                    }
                    playerRef.value = player
                    player.addIdActionListener { _id, _metadata ->
                        println("click $_id $_metadata")
                    }
                    player
                },
                update = {
                    it.setTheme(playbackTheme)
                    if (currentDocument.value != null) {
                        it.setDocument(RemoteComposeDocument(currentDocument.value!!))
                    }
                    it.setDebug(debugMode)
                },
            )

            if (stats != null) {
                LazyColumn {
                    item {
                        Row() {
                            Text(
                                text = "Size ",
                                modifier = Modifier.fillMaxWidth(0.6f),
                                textAlign = TextAlign.Right,
                            )
                            Text(text = " $docSize")
                            Text(text = "  Eval: ", textAlign = TextAlign.Right)
                            Text(text = "%.2f".format(evalTime.value) + "ms")
                        }
                    }
                    item {
                        Row() {
                            Text(
                                text = "ZipSize ",
                                modifier = Modifier.fillMaxWidth(0.6f),
                                textAlign = TextAlign.Right,
                            )
                            Text(text = " $zipSize")
                        }
                    }
                    itemsIndexed(stats) { index, stat ->
                        val sp = stat.split(":")
                        println(sp)
                        Row(
                            modifier =
                                Modifier.background(
                                    if (index % 2 == 0) Color.LightGray else Color.White
                                )
                        ) {
                            Text(
                                text = sp[0],
                                modifier = Modifier.fillMaxWidth(0.6f),
                                textAlign = TextAlign.Right,
                            )
                            Text(
                                text = sp[1],
                                modifier = Modifier.fillMaxWidth(0.2f),
                                textAlign = TextAlign.Right,
                            )
                            if (sp.size > 2) {
                                val count = sp[1].trim().toFloat()
                                val psize = ((sp[2].trim().toFloat() - count) / count).roundToInt()
                                Text(
                                    text = "   " + sp[2] + " ",
                                    textAlign = TextAlign.Right,
                                    modifier = Modifier.fillMaxWidth(.4f),
                                )
                                Text(
                                    text = " (" + count.toInt() + "x" + psize + ") ",
                                    textAlign = TextAlign.Right,
                                    modifier = Modifier.fillMaxWidth(),
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}

@Suppress("RestrictedApiAndroidX")
@Composable
fun DisplayDoc(fileReady: Boolean, func: RemoteComposeFunc) {
    if (fileReady) {
        var playbackTheme by remember { mutableIntStateOf(Theme.UNSPECIFIED) }
        var debugMode by remember { mutableIntStateOf(0) }
        var documentWidth by remember { mutableIntStateOf(300) }
        var documentHeight by remember { mutableIntStateOf(300) }
        val currentDocument = func.getDoc() // remember(func) {  }
        val textDoc = currentDocument.value?.toNestedString()

        Column {
            AndroidView(
                modifier = Modifier.size(documentWidth.dp, documentHeight.dp),
                factory = {
                    val player = RemoteComposePlayer(it)
                    if (currentDocument.value != null) {
                        player.setDocument(RemoteComposeDocument(currentDocument.value!!))
                    }
                    player.addIdActionListener { _id, _metadata ->
                        println("click $_id $_metadata")
                    }
                    player
                },
                update = {
                    it.setTheme(playbackTheme)
                    if (currentDocument.value != null) {
                        it.setDocument(RemoteComposeDocument(currentDocument.value!!))
                    }
                    it.setDebug(debugMode)
                },
            )

            if (textDoc != null) {
                Text(
                    text = textDoc,
                    Modifier.fillMaxWidth()
                        .fillMaxHeight()
                        .verticalScroll(rememberScrollState())
                        .horizontalScroll(rememberScrollState()),
                )
            }
        }
    }
}

@Suppress("RestrictedApiAndroidX")
@Composable
fun DisplayMain(
    fileReady: Boolean,
    showOrigami: Boolean,
    showCompose: Boolean,
    showComposePlayer: Boolean,
    func: RemoteComposeFunc,
) {
    Column(modifier = Modifier.background(Color.LightGray).padding(10.dp)) {
        var id by remember { mutableIntStateOf(0) }
        var metadata by remember { mutableStateOf("empty") }
        var playbackTheme by remember { mutableIntStateOf(Theme.UNSPECIFIED) }
        var debugMode by remember { mutableIntStateOf(0) }
        var documentWidth by remember { mutableIntStateOf(300) }
        var documentHeight by remember { mutableIntStateOf(300) }
        Row {
            Spacer(modifier = Modifier.width(2.dp))

            Text("RemoteComposePlayer:", Modifier.clickable { println("click RmoteComposePlayer") })
        }
        println("launching $func")
        if (fileReady && showOrigami) {
            Box(modifier = Modifier.background(Color(0xFFFFFFFF)).fillMaxHeight(0.5f)) {
                // Button(onClick = { /*TODO*/}) { Text(text = "hello world") }
                val currentDocument = func.getDoc() // remember(func) {  }

                AndroidView(
                    modifier = Modifier.size(documentWidth.dp, documentHeight.dp),
                    factory = {
                        val player = RemoteComposePlayer(it)
                        if (currentDocument.value != null) {
                            player.setDocument(RemoteComposeDocument(currentDocument.value!!))
                        }
                        player.addIdActionListener { _id, _metadata ->
                            id = _id
                            metadata = _metadata ?: "empty"
                        }
                        player
                    },
                    update = {
                        it.setTheme(playbackTheme)
                        if (currentDocument.value != null) {
                            it.setDocument(RemoteComposeDocument(currentDocument.value!!))
                        }
                        it.setDebug(debugMode)
                    },
                )
            }
        } else {
            Text(if (showOrigami) "Waiting for file..." else "RC not shown")
        }
        if (showComposePlayer) {
            //            Text("Compose player:")
            //            Box(modifier = Modifier
            //                .background(Color(0xFFFFFFFF))
            //            ) {
            //                val currentDocument = func.getDoc()
            //                currentDocument.value?.let {
            //                    RemoteDocumentPlayer(
            //                        it,
            //                        documentWidth = documentWidth,
            //                        documentHeight = documentHeight,
            //                    ) { _, _, _ -> }
            //                }
            //            }
        }
        if (showCompose) {
            Text("Direct Compose:")
            Box(modifier = Modifier.background(Color.LightGray)) { func.Run() }
        }
    }
}

@Suppress("RestrictedApiAndroidX")
interface RemoteComposeFunc {
    @Composable fun Run()

    override fun toString(): String

    fun getColor(): Color

    @Composable fun getDoc(): MutableState<CoreDocument?>
}

@Suppress("RestrictedApiAndroidX")
@Composable
fun RemoteComposableMenu(
    map: List<RemoteComposeFunc>,
    act:
        (
            act: RemoteComposeFunc,
            showOrigami: Boolean,
            showCompose: Boolean,
            showComposePlayer: Boolean,
            debugCompose: Boolean,
        ) -> Unit,
) {
    Column(modifier = Modifier.fillMaxWidth().padding(10.dp)) {
        var showOrigami by remember { mutableStateOf(DEFAULT_SHOW_REMOTE) }
        var showCompose by remember { mutableStateOf(DEFAULT_SHOW_COMPOSE) }
        var showComposePlayer by remember { mutableStateOf(DEFAULT_SHOW_COMPOSE_PLAYER) }
        var debugCompose by remember { mutableStateOf(DEFAULT_DEBUG_REMOTE_COMPOSE) }
        LazyVerticalGrid(columns = GridCells.Adaptive(minSize = 170.dp)) {
            items(map.size) { i ->
                var c = ButtonDefaults.buttonColors() // backgroundColor = map[i].getColor())
                Button(
                    modifier = Modifier.padding(2.dp),
                    colors = c,
                    onClick = {
                        act(map[i], showOrigami, showCompose, showComposePlayer, debugCompose)
                    },
                ) {
                    Text(map[i].toString(), modifier = Modifier.padding(2.dp))
                }
            }
        }
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text("RemoteCompose:")
            Checkbox(checked = showOrigami, onCheckedChange = { showOrigami = it })
            Text("Compose:")
            Checkbox(checked = showCompose, onCheckedChange = { showCompose = it })
        }
        Row(verticalAlignment = Alignment.CenterVertically) {
            Text("Compose player:")
            Checkbox(checked = showComposePlayer, onCheckedChange = { showComposePlayer = it })
            Text("Debug:")
            Checkbox(checked = debugCompose, onCheckedChange = { debugCompose = it })
        }
    }
}

fun toRcColor(str: String, sat: Float = .5f): Color {
    if (str.endsWith("...")) {
        return Color.LightGray
    }
    val h = Random(str.hashCode()).nextFloat()
    val c = Color.hsl(h * 360, sat, .7f)
    return c
}

@Suppress("RestrictedApiAndroidX")
fun docSize(doc: CoreDocument): Int {
    val wb = doc.buffer.buffer
    val len = wb.size
    return len
}

@Suppress("RestrictedApiAndroidX")
fun compressDocSize(doc: CoreDocument): Int {
    val wb = doc.buffer.buffer
    val len = wb.size
    val buff = wb.buffer
    return compress(buff, len)
}

fun compress(data: ByteArray?, len: Int): Int {
    val byteArrayOutputStream = ByteArrayOutputStream()

    try {
        DeflaterOutputStream(byteArrayOutputStream).use { deflaterOutputStream ->
            deflaterOutputStream.write(data, 0, len)
            deflaterOutputStream.finish()
        }
    } catch (e: IOException) {
        e.printStackTrace()
    }

    return byteArrayOutputStream.size()
}
