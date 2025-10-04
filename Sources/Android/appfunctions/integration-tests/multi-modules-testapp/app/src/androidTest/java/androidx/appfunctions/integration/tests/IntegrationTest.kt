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

package androidx.appfunctions.integration.tests

import android.Manifest
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageInstaller
import android.net.Uri
import androidx.appfunctions.AppFunctionData
import androidx.appfunctions.AppFunctionFunctionNotFoundException
import androidx.appfunctions.AppFunctionInvalidArgumentException
import androidx.appfunctions.AppFunctionManagerCompat
import androidx.appfunctions.AppFunctionSearchSpec
import androidx.appfunctions.ExecuteAppFunctionRequest
import androidx.appfunctions.ExecuteAppFunctionResponse
import androidx.appfunctions.integration.tests.AppSearchMetadataHelper.isDynamicIndexerAvailable
import androidx.appfunctions.integration.tests.TestUtil.assertReadAccessible
import androidx.appfunctions.integration.tests.TestUtil.assertReadInaccessible
import androidx.appfunctions.integration.tests.TestUtil.assertWriteAccessible
import androidx.appfunctions.integration.tests.TestUtil.assertWriteInaccessible
import androidx.appfunctions.integration.tests.TestUtil.doBlocking
import androidx.appfunctions.integration.tests.TestUtil.retryAssert
import androidx.appfunctions.metadata.AppFunctionComponentsMetadata
import androidx.appfunctions.metadata.AppFunctionDataTypeMetadata
import androidx.appfunctions.metadata.AppFunctionIntTypeMetadata
import androidx.appfunctions.metadata.AppFunctionMetadata
import androidx.appfunctions.metadata.AppFunctionObjectTypeMetadata
import androidx.appfunctions.metadata.AppFunctionParameterMetadata
import androidx.appfunctions.metadata.AppFunctionReferenceTypeMetadata
import androidx.appfunctions.metadata.AppFunctionStringTypeMetadata
import androidx.test.filters.LargeTest
import androidx.test.platform.app.InstrumentationRegistry
import com.google.common.truth.Truth.assertThat
import java.io.InputStream
import java.time.LocalDateTime
import kotlin.coroutines.resumeWithException
import kotlin.test.assertFailsWith
import kotlin.test.assertIs
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.async
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.suspendCancellableCoroutine
import org.junit.After
import org.junit.Assert.assertThrows
import org.junit.Assume.assumeFalse
import org.junit.Assume.assumeNotNull
import org.junit.Assume.assumeTrue
import org.junit.Before
import org.junit.Test

@LargeTest
class IntegrationTest {
    private val context = InstrumentationRegistry.getInstrumentation().context
    private val targetContext = InstrumentationRegistry.getInstrumentation().targetContext
    private lateinit var appFunctionManager: AppFunctionManagerCompat
    private val uiAutomation = InstrumentationRegistry.getInstrumentation().uiAutomation

    @Before
    fun setup() = doBlocking {
        val appFunctionManagerCompatOrNull = AppFunctionManagerCompat.getInstance(targetContext)
        assumeNotNull(appFunctionManagerCompatOrNull)
        appFunctionManager = checkNotNull(appFunctionManagerCompatOrNull)

        uiAutomation.apply {
            // This is needed because the test is running under the UID of
            // "androidx.appfunctions.integration.testapp",
            // while the app functions are defined under
            // "androidx.appfunctions.integration.testapp.test"
            adoptShellPermissionIdentity(
                Manifest.permission.INSTALL_PACKAGES,
                "android.permission.EXECUTE_APP_FUNCTIONS",
            )
            executeShellCommand(
                "device_config put appsearch max_allowed_app_function_doc_size_in_bytes $TEST_APP_FUNCTION_DOC_SIZE_LIMIT"
            )
        }
        context.awaitAppFunctionsIndexed(context.packageName, FUNCTION_IDS)
    }

    @After
    fun tearDown() {
        uiAutomation.executeShellCommand("pm uninstall $ADDITIONAL_APP_PACKAGE")
        uiAutomation.dropShellPermissionIdentity()
    }

    @Test
    fun executeAppFunction_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata("androidx.appfunctions.integration.tests.TestFunctions#add")

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setLong("num1", 1)
                            .setLong("num2", 2)
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        assertThat(
                successResponse.returnValue.getLong(
                    ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                )
            )
            .isEqualTo(3)
    }

    @Test
    fun searchAllAppFunctions_returnsAllAppFunction_withDynamicIndexer() = doBlocking {
        assumeTrue(isDynamicIndexerAvailable(context))
        val searchFunctionSpec = AppFunctionSearchSpec(packageNames = setOf(context.packageName))

        val appFunctions: List<AppFunctionMetadata> =
            appFunctionManager.observeAppFunctions(searchFunctionSpec).first().flatMap {
                it.appFunctions
            }

        assertThat(appFunctions).hasSize(20)
    }

    @Test
    fun searchAllAppFunctions_returnEnumValues_withDynamicIndexer() = doBlocking {
        assumeTrue(isDynamicIndexerAvailable(context))
        val searchFunctionSpec = AppFunctionSearchSpec(packageNames = setOf(context.packageName))

        val enumFunctionMetadata =
            appFunctionManager
                .observeAppFunctions(searchFunctionSpec)
                .first()
                .flatMap { it.appFunctions }
                .single {
                    it.id ==
                        "androidx.appfunctions.integration.tests.TestFunctions#enumValueFunction"
                }

        val intEnumParamMetadata =
            assertIs<AppFunctionIntTypeMetadata>(
                enumFunctionMetadata.parameters.associateBy { it.name }["intEnum"]?.dataType
            )
        assertThat(intEnumParamMetadata.enumValues).containsExactly(0, 1)
        val stringEnumParamMetadata =
            assertIs<AppFunctionStringTypeMetadata>(
                enumFunctionMetadata.parameters.associateBy { it.name }["stringEnum"]?.dataType
            )
        assertThat(stringEnumParamMetadata.enumValues).containsExactly("A", "B")
    }

    @Test
    fun searchAllAppFunctions_returnEnumValuesFromLibraryModule_withDynamicIndexer() = doBlocking {
        assumeTrue(isDynamicIndexerAvailable(context))
        val searchFunctionSpec = AppFunctionSearchSpec(packageNames = setOf(context.packageName))

        val enumFunctionMetadata =
            appFunctionManager
                .observeAppFunctions(searchFunctionSpec)
                .first()
                .flatMap { it.appFunctions }
                .single {
                    it.id ==
                        "androidx.appfunctions.integration.testapp.library.TestFunctions2#enumValueFunction"
                }

        val intEnumParamMetadata =
            assertIs<AppFunctionIntTypeMetadata>(
                enumFunctionMetadata.parameters.associateBy { it.name }["intEnum"]?.dataType
            )
        assertThat(intEnumParamMetadata.enumValues).containsExactly(0, 1)
        val stringEnumParamMetadata =
            assertIs<AppFunctionStringTypeMetadata>(
                enumFunctionMetadata.parameters.associateBy { it.name }["stringEnum"]?.dataType
            )
        assertThat(stringEnumParamMetadata.enumValues).containsExactly("A", "B")
        val intEnumReturnMetadata =
            assertIs<AppFunctionIntTypeMetadata>(enumFunctionMetadata.response.valueType)
        assertThat(intEnumReturnMetadata.enumValues).containsExactly(10, 20)
    }

    @Test
    fun searchAllAppFunctions_populatesFunctionDescriptions_withDynamicIndexer() = doBlocking {
        val expectedAppFunctionDescriptions =
            mapOf(
                "androidx.appfunctions.integration.tests.TestFunctions#add" to
                    "Returns the sum of the given two numbers.",
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#concat" to
                    "Concatenates the two given strings.",
            )
        val expectedParamDescriptions =
            mapOf(
                "androidx.appfunctions.integration.tests.TestFunctions#add" to
                    listOf("The first number.", "The second number."),
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#concat" to
                    listOf("The first string.", "The second string."),
            )
        val expectedResponseDescriptions =
            mapOf(
                "androidx.appfunctions.integration.tests.TestFunctions#add" to
                    "The sum of the two numbers.",
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#concat" to
                    "The result of concatenating the two strings.",
            )
        assumeTrue(isDynamicIndexerAvailable(context))
        val searchFunctionSpec = AppFunctionSearchSpec(packageNames = setOf(context.packageName))

        val appFunctions: List<AppFunctionMetadata> =
            appFunctionManager
                .observeAppFunctions(searchFunctionSpec)
                .first()
                .flatMap { it.appFunctions }
                .filter { it -> it.id in expectedAppFunctionDescriptions.keys }

        assertThat(expectedAppFunctionDescriptions.keys)
            .containsExactlyElementsIn(appFunctions.map { it -> it.id })

        for (appFunction in appFunctions) {
            assertThat(expectedAppFunctionDescriptions[appFunction.id])
                .isEqualTo(appFunction.description)
            assertThat(expectedParamDescriptions[appFunction.id])
                .containsExactlyElementsIn(appFunction.parameters.map { it.description })
            assertThat(expectedResponseDescriptions[appFunction.id])
                .isEqualTo(appFunction.response.description)
        }
    }

    @Test
    fun searchAllAppFunctions_populatesSerializableDescriptions_withDynamicIndexer() = doBlocking {
        val expectedSerializableDescriptions =
            mapOf(
                "androidx.appfunctions.integration.tests.Note" to
                    "Represents a note in the notes app.",
                "androidx.appfunctions.integration.tests.SetField<kotlin.String>" to
                    "Example parameterized AppFunctionSerializable.",
                "androidx.appfunctions.integration.testapp.library.ExampleSerializable" to
                    "AppFunctionSerializable in non-root library.",
                "androidx.appfunctions.integration.testapp.library.GenericSerializable<kotlin.Int>" to
                    "Example parameterized AppFunctionSerializable in another package.",
            )
        val expectedPropertyDescriptions =
            mapOf(
                "androidx.appfunctions.integration.tests.Note" to
                    mapOf(
                        "title" to "The note's title.",
                        "content" to "The note's content.",
                        "owner" to "The note's [Owner].",
                        "attachments" to "The note's attachments.",
                        "modifiedTime" to "The note's last modified time.",
                    ),
                "androidx.appfunctions.integration.tests.SetField<kotlin.String>" to
                    mapOf("value" to "Value property of SetField."),
                "androidx.appfunctions.integration.testapp.library.ExampleSerializable" to
                    mapOf("intProperty" to "Int property of ExampleSerializable."),
                "androidx.appfunctions.integration.testapp.library.GenericSerializable<kotlin.Int>" to
                    mapOf("value" to "Value property of GenericSerializable."),
            )
        assumeTrue(isDynamicIndexerAvailable(context))
        val searchFunctionSpec = AppFunctionSearchSpec(packageNames = setOf(context.packageName))

        val dataTypeMetadata: Map<String, AppFunctionDataTypeMetadata> =
            appFunctionManager
                .observeAppFunctions(searchFunctionSpec)
                .first()
                .flatMap { it -> it.appFunctions }
                .map { it.components.dataTypes }
                .fold(emptyMap()) { acc, map -> acc + map }
        val filteredMetadata =
            dataTypeMetadata.filter { it -> it.key in expectedSerializableDescriptions.keys }
        assertThat(expectedSerializableDescriptions.keys)
            .containsExactlyElementsIn(filteredMetadata.keys)

        for ((id, dataType) in filteredMetadata) {
            assertThat(expectedSerializableDescriptions[id]).isEqualTo(dataType.description)
            assertThat(expectedPropertyDescriptions[id])
                .containsExactlyEntriesIn(
                    (dataType as AppFunctionObjectTypeMetadata).properties.entries.associate { it ->
                        it.key to it.value.description
                    }
                )
        }
    }

    @Test
    fun searchAllAppFunctions_returnsAllSchemaAppFunction_withLegacyIndexer() = doBlocking {
        assumeFalse(isDynamicIndexerAvailable(context))
        val searchFunctionSpec = AppFunctionSearchSpec(packageNames = setOf(context.packageName))

        val appFunctions: List<AppFunctionMetadata> =
            appFunctionManager.observeAppFunctions(searchFunctionSpec).first().flatMap {
                it.appFunctions
            }

        assertThat(appFunctions).hasSize(1)
    }

    @Test
    fun executeAppFunction_voidReturnType_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#voidFunction"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components).build(),
                    )
            )

        assertThat(response).isInstanceOf(ExecuteAppFunctionResponse.Success::class.java)
    }

    @Test
    fun executeAppFunction_setFactory_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFactory#isCreatedByFactory"
            )
        // A factory is set to create the enclosing class of the function.
        // See [TestApplication.appFunctionConfiguration].
        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components).build(),
                    )
            )

        // If the enclosing class was created by the provided factory, the secondary constructor
        // should be called and so the return value would be `true`.
        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        assertThat(
                successResponse.returnValue.getBoolean(
                    ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                )
            )
            .isEqualTo(true)
    }

    @Test
    fun executeAppFunction_functionInLibraryModule_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#concat"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setString("str1", "log")
                            .setString("str2", "cat")
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        assertThat(
                successResponse.returnValue.getString(
                    ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                )
            )
            .isEqualTo("logcat")
    }

    @Test
    fun executeAppFunction_functionNotFound_fail() = doBlocking {
        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        context.packageName,
                        "androidx.appfunctions.integration.tests.TestFunctions#notExist",
                        AppFunctionData.EMPTY,
                    )
            )

        val errorResponse = assertIs<ExecuteAppFunctionResponse.Error>(response)
        assertThat(errorResponse.error)
            .isInstanceOf(AppFunctionFunctionNotFoundException::class.java)
    }

    @Test
    fun executeAppFunction_appThrows_fail() = doBlocking {
        val metadata =
            findAppFunctionMetadata("androidx.appfunctions.integration.tests.TestFunctions#doThrow")

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components).build(),
                    )
            )

        assertThat(response).isInstanceOf(ExecuteAppFunctionResponse.Error::class.java)
        val errorResponse = response as ExecuteAppFunctionResponse.Error
        assertThat(errorResponse.error)
            .isInstanceOf(AppFunctionInvalidArgumentException::class.java)
    }

    @Test
    fun executeAppFunction_createNote() = doBlocking {
        val createNoteMetadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#createNote"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        createNoteMetadata.packageName,
                        createNoteMetadata.id,
                        AppFunctionData.Builder(
                                createNoteMetadata.parameters,
                                createNoteMetadata.components,
                            )
                            .setAppFunctionData(
                                "createNoteParams",
                                AppFunctionData.serialize(
                                    CreateNoteParams(
                                        title = "Test Title",
                                        content = listOf("1", "2"),
                                        owner = Owner("test"),
                                        attachments =
                                            listOf(Attachment("Uri1", Attachment("nested"))),
                                    ),
                                    CreateNoteParams::class.java,
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val expectedNote =
            Note(
                title = "Test Title",
                content = listOf("1", "2"),
                owner = Owner("test"),
                attachments = listOf(Attachment("Uri1", Attachment("nested"))),
            )
        assertThat(
                successResponse.returnValue
                    .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                    ?.deserialize(Note::class.java)
            )
            .isEqualTo(expectedNote)
    }

    @Test
    fun executeAppFunction_createNote_withOpenableCapability_returnsNote() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#getOpenableNote"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setAppFunctionData(
                                "createNoteParams",
                                AppFunctionData.serialize(
                                    CreateNoteParams(
                                        title = "Test Title",
                                        content = listOf("1", "2"),
                                        owner = Owner("test"),
                                        attachments =
                                            listOf(Attachment("Uri1", Attachment("nested"))),
                                    ),
                                    CreateNoteParams::class.java,
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val expectedNote =
            Note(
                title = "Test Title",
                content = listOf("1", "2"),
                owner = Owner("test"),
                attachments = listOf(Attachment("Uri1", Attachment("nested"))),
            )
        assertThat(
                successResponse.returnValue
                    .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                    ?.deserialize(Note::class.java)
            )
            .isEqualTo(expectedNote)
    }

    @Test
    fun executeAppFunction_createNote_withOpenableCapability_returnsOpenableNote() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#getOpenableNote"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setAppFunctionData(
                                "createNoteParams",
                                AppFunctionData.serialize(
                                    CreateNoteParams(
                                        title = "Test Title",
                                        content = listOf("1", "2"),
                                        owner = Owner("test"),
                                        attachments =
                                            listOf(Attachment("Uri1", Attachment("nested"))),
                                    ),
                                    CreateNoteParams::class.java,
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val expectedNote =
            Note(
                title = "Test Title",
                content = listOf("1", "2"),
                owner = Owner("test"),
                attachments = listOf(Attachment("Uri1", Attachment("nested"))),
            )
        val openableNoteResult =
            assertIs<OpenableNote>(
                successResponse.returnValue
                    .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                    ?.deserialize(OpenableNote::class.java)
            )

        assertThat(openableNoteResult.title).isEqualTo(expectedNote.title)
        assertThat(openableNoteResult.content).isEqualTo(expectedNote.content)
        assertThat(openableNoteResult.owner).isEqualTo(expectedNote.owner)
        assertThat(openableNoteResult.attachments).isEqualTo(expectedNote.attachments)
        assertThat(openableNoteResult.intentToOpen).isNotNull()
    }

    @Test
    fun executeAppFunction_serializableProxyParam_dateTime_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#logLocalDateTime"
            )
        val localDateTimeClass = DateTime(LocalDateTime.now())

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        targetPackageName = metadata.packageName,
                        functionIdentifier = metadata.id,
                        functionParameters =
                            AppFunctionData.Builder(metadata.parameters, metadata.components)
                                .setAppFunctionData(
                                    "dateTime",
                                    AppFunctionData.serialize(
                                        localDateTimeClass,
                                        DateTime::class.java,
                                    ),
                                )
                                .build(),
                    )
            )
        assertIs<ExecuteAppFunctionResponse.Success>(response)
    }

    @Test
    fun executeAppFunction_serializableProxyParam_androidUri_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#logUri"
            )

        val androidUri = Uri.parse("https://www.google.com/")
        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        targetPackageName = metadata.packageName,
                        functionIdentifier = metadata.id,
                        functionParameters =
                            AppFunctionData.Builder(metadata.parameters, metadata.components)
                                .setAppFunctionData(
                                    "androidUri",
                                    AppFunctionData.serialize(androidUri, Uri::class.java),
                                )
                                .build(),
                    )
            )

        assertIs<ExecuteAppFunctionResponse.Success>(response)
    }

    @Test
    fun executeAppFunction_serializableProxyResponse_dateTime_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#getLocalDate"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        targetPackageName = metadata.packageName,
                        functionIdentifier = metadata.id,
                        functionParameters =
                            AppFunctionData.Builder(metadata.parameters, metadata.components)
                                .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)

        assertIs<LocalDateTime>(
            successResponse.returnValue
                .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                ?.deserialize(DateTime::class.java)
                ?.localDateTime
        )
    }

    @Test
    fun executeAppFunction_serializableProxyResponse_androidUri_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#getUri"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        targetPackageName = metadata.packageName,
                        functionIdentifier = metadata.id,
                        functionParameters =
                            AppFunctionData.Builder(metadata.parameters, metadata.components)
                                .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val androidUriResult =
            assertIs<Uri>(
                successResponse.returnValue
                    .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                    ?.deserialize(Uri::class.java)
            )
        assertThat(androidUriResult.toString()).isEqualTo("https://www.google.com/")
    }

    @Test
    fun executeAppFunction_updateNote_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#updateNote"
            )
        val attachment = Attachment(uri = "uri", nested = null)
        val dateTime = LocalDateTime.of(1, 1, 1, 1, 1)

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setAppFunctionData(
                                "updateNoteParams",
                                AppFunctionData.serialize(
                                    UpdateNoteParams(
                                        title = SetField("NewTitle1"),
                                        nullableTitle = SetField("NewTitle2"),
                                        content = SetField(listOf("NewContent1")),
                                        nullableContent = SetField(listOf("NewContent2")),
                                        attachments = SetField(listOf(attachment)),
                                        modifiedTime = SetField(dateTime),
                                    ),
                                    UpdateNoteParams::class.java,
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val expectedNote =
            Note(
                title = "NewTitle1_NewTitle2",
                content = listOf("NewContent1", "NewContent2"),
                owner = Owner("test"),
                attachments = listOf(attachment),
                modifiedTime = dateTime,
            )
        assertThat(
                successResponse.returnValue
                    .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                    ?.deserialize(Note::class.java)
            )
            .isEqualTo(expectedNote)
    }

    @Test
    fun executeAppFunction_updateNoteSetFieldNullContent_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#updateNote"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setAppFunctionData(
                                "updateNoteParams",
                                AppFunctionData.serialize(
                                    UpdateNoteParams(
                                        title = SetField("NewTitle1"),
                                        nullableTitle = SetField(null),
                                        content = SetField(listOf("NewContent1")),
                                        nullableContent = SetField(null),
                                    ),
                                    UpdateNoteParams::class.java,
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val expectedNote =
            Note(
                title = "NewTitle1_DefaultTitle",
                content = listOf("NewContent1", "DefaultContent"),
                owner = Owner("test"),
                attachments = listOf(),
            )
        assertThat(
                successResponse.returnValue
                    .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                    ?.deserialize(Note::class.java)
            )
            .isEqualTo(expectedNote)
    }

    @Test
    fun executeAppFunction_updateNoteNullSetFields_success() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#updateNote"
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setAppFunctionData(
                                "updateNoteParams",
                                AppFunctionData.serialize(
                                    UpdateNoteParams(),
                                    UpdateNoteParams::class.java,
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val expectedNote =
            Note(
                title = "DefaultTitle_DefaultTitle",
                content = listOf("DefaultContent", "DefaultContent"),
                owner = Owner("test"),
                attachments = listOf(),
            )
        assertThat(
                successResponse.returnValue
                    .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                    ?.deserialize(Note::class.java)
            )
            .isEqualTo(expectedNote)
    }

    @Test
    fun executeAppFunction_legacySchemaCreateNote_success() = doBlocking {
        targetContext.installApk(ADDITIONAL_APK_FILE)
        targetContext.awaitAppFunctionsIndexed(
            ADDITIONAL_APP_PACKAGE,
            setOf(ADDITIONAL_LEGACY_CREATE_NOTE),
        )
        val createNoteMetadata =
            appFunctionManager
                .observeAppFunctions(
                    AppFunctionSearchSpec(packageNames = setOf(ADDITIONAL_APP_PACKAGE))
                )
                .first()
                .flatMap { it.appFunctions }
                .single()

        val response =
            appFunctionManager.executeAppFunction(
                ExecuteAppFunctionRequest(
                    functionIdentifier = createNoteMetadata.id,
                    targetPackageName = createNoteMetadata.packageName,
                    functionParameters =
                        AppFunctionData.Builder(
                                createNoteMetadata.parameters,
                                createNoteMetadata.components,
                            )
                            .setAppFunctionData(
                                "createNoteParams",
                                AppFunctionData.Builder(
                                        requireTargetObjectTypeMetadata(
                                            "createNoteParams",
                                            createNoteMetadata.parameters,
                                            createNoteMetadata.components,
                                        ),
                                        createNoteMetadata.components,
                                    )
                                    .setString("title", "Test Title")
                                    .setString("content", "Test Content")
                                    .build(),
                            )
                            .build(),
                )
            )

        assertIs<ExecuteAppFunctionResponse.Success>(response)
        val returnValue =
            response.returnValue.getAppFunctionData(
                ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
            )
        assertThat(returnValue).isNotNull()
        assertThat(checkNotNull(returnValue).getString("title")).isEqualTo("Test Title")
        assertThat(returnValue.getString("content")).isEqualTo("Test Content")
    }

    @Test
    fun executeAppFunction_legacySchemaCreateNoteSerialization_success() = doBlocking {
        targetContext.installApk(ADDITIONAL_APK_FILE)
        targetContext.awaitAppFunctionsIndexed(
            ADDITIONAL_APP_PACKAGE,
            setOf(ADDITIONAL_LEGACY_CREATE_NOTE),
        )
        val createNoteMetadata =
            appFunctionManager
                .observeAppFunctions(
                    AppFunctionSearchSpec(packageNames = setOf(ADDITIONAL_APP_PACKAGE))
                )
                .first()
                .flatMap { it.appFunctions }
                .single()
        val createNoteParams =
            LegacyCreateNoteParams(title = "Test Title", content = "Test Content")

        val response =
            appFunctionManager.executeAppFunction(
                ExecuteAppFunctionRequest(
                    functionIdentifier = createNoteMetadata.id,
                    targetPackageName = createNoteMetadata.packageName,
                    functionParameters =
                        AppFunctionData.Builder(
                                createNoteMetadata.parameters,
                                createNoteMetadata.components,
                            )
                            .setAppFunctionData(
                                "createNoteParams",
                                AppFunctionData.serialize(
                                    createNoteParams,
                                    LegacyCreateNoteParams::class.java,
                                ),
                            )
                            .build(),
                )
            )

        assertIs<ExecuteAppFunctionResponse.Success>(response)
        val returnValue =
            response.returnValue.getAppFunctionData(
                ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
            )
        assertThat(returnValue).isNotNull()
        val note = checkNotNull(returnValue).deserialize(LegacyNote::class.java)
        assertThat(note.title).isEqualTo("Test Title")
        assertThat(note.content).isEqualTo("Test Content")
    }

    @Test
    fun executeAppFunction_schemaCreateNote_success() = doBlocking {
        val createNoteMetadata =
            appFunctionManager
                .observeAppFunctions(
                    AppFunctionSearchSpec(
                        packageNames = setOf(context.packageName),
                        schemaCategory = "myNotes",
                        schemaName = "createNote",
                        minSchemaVersion = 2,
                    )
                )
                .first()
                .flatMap { it.appFunctions }
                .single()
        val request =
            ExecuteAppFunctionRequest(
                functionIdentifier = createNoteMetadata.id,
                targetPackageName = createNoteMetadata.packageName,
                functionParameters =
                    AppFunctionData.Builder(
                            createNoteMetadata.parameters,
                            createNoteMetadata.components,
                        )
                        .setAppFunctionData(
                            "parameters",
                            AppFunctionData.Builder(
                                    requireTargetObjectTypeMetadata(
                                        "parameters",
                                        createNoteMetadata.parameters,
                                        createNoteMetadata.components,
                                    ),
                                    createNoteMetadata.components,
                                )
                                .setString("title", "Test Title")
                                .setString("content", "Some valid content")
                                .setAppFunctionDataList("attachments", emptyList())
                                .setString("groupId", "testGroupId")
                                .setString("externalUuid", "testExternalUuid")
                                .build(),
                        )
                        .build(),
            )

        val response = appFunctionManager.executeAppFunction(request)

        assertIs<ExecuteAppFunctionResponse.Success>(response)
        val resultNote =
            response.returnValue
                .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                ?.getAppFunctionData("createdNote")
        assertThat(resultNote?.getString("id")).isEqualTo("testId")
        assertThat(resultNote?.getString("title")).isEqualTo("Test Title")
    }

    @Test
    fun executeAppFunction_schemaCreateNoteSerialization_success() = doBlocking {
        val createNoteMetadata =
            appFunctionManager
                .observeAppFunctions(
                    AppFunctionSearchSpec(
                        packageNames = setOf(context.packageName),
                        schemaCategory = "myNotes",
                        schemaName = "createNote",
                        minSchemaVersion = 2,
                    )
                )
                .first()
                .flatMap { it.appFunctions }
                .single()
        val parameters =
            CreateNoteAppFunction.Parameters(
                title = "Test Title",
                content = "Some valid content",
                attachments = emptyList(),
                groupId = "testGroupId",
                externalUuid = "testExternalUuid",
            )
        val request =
            ExecuteAppFunctionRequest(
                functionIdentifier = createNoteMetadata.id,
                targetPackageName = createNoteMetadata.packageName,
                functionParameters =
                    AppFunctionData.Builder(
                            createNoteMetadata.parameters,
                            createNoteMetadata.components,
                        )
                        .setAppFunctionData(
                            "parameters",
                            AppFunctionData.serialize(
                                parameters,
                                CreateNoteAppFunction.Parameters::class.java,
                            ),
                        )
                        .build(),
            )

        val response = appFunctionManager.executeAppFunction(request)

        assertIs<ExecuteAppFunctionResponse.Success>(response)
        val resultNote =
            response.returnValue
                .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                ?.getAppFunctionData("createdNote")
        assertThat(resultNote?.getString("id")).isEqualTo("testId")
        assertThat(resultNote?.getString("title")).isEqualTo("Test Title")
    }

    @Test
    fun executeAppFunction_schemaCreateNote_readInvalidFieldFail() = doBlocking {
        val createNoteMetadata =
            appFunctionManager
                .observeAppFunctions(
                    AppFunctionSearchSpec(
                        packageNames = setOf(context.packageName),
                        schemaCategory = "myNotes",
                        schemaName = "createNote",
                        minSchemaVersion = 2,
                    )
                )
                .first()
                .flatMap { it.appFunctions }
                .single()
        val request =
            ExecuteAppFunctionRequest(
                functionIdentifier = createNoteMetadata.id,
                targetPackageName = createNoteMetadata.packageName,
                functionParameters =
                    AppFunctionData.Builder(
                            createNoteMetadata.parameters,
                            createNoteMetadata.components,
                        )
                        .setAppFunctionData(
                            "parameters",
                            AppFunctionData.Builder(
                                    requireTargetObjectTypeMetadata(
                                        "parameters",
                                        createNoteMetadata.parameters,
                                        createNoteMetadata.components,
                                    ),
                                    createNoteMetadata.components,
                                )
                                .setString("title", "Test Title")
                                .setString("content", "Some valid content")
                                .setAppFunctionDataList("attachments", emptyList())
                                .setString("groupId", "testGroupId")
                                .setString("externalUuid", "testExternalUuid")
                                .build(),
                        )
                        .build(),
            )

        val response = appFunctionManager.executeAppFunction(request)

        assertIs<ExecuteAppFunctionResponse.Success>(response)
        val resultNote =
            response.returnValue
                .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                ?.getAppFunctionData("createdNote")
        assertThrows(IllegalArgumentException::class.java) { resultNote?.getInt(("title")) }
    }

    @Test
    fun prepareAppFunctionData_wrongTopLevelParameterName_fail() = doBlocking {
        val createNoteMetadata =
            appFunctionManager
                .observeAppFunctions(
                    AppFunctionSearchSpec(
                        packageNames = setOf(context.packageName),
                        schemaCategory = "myNotes",
                        schemaName = "createNote",
                        minSchemaVersion = 2,
                    )
                )
                .first()
                .flatMap { it.appFunctions }
                .single()

        val innerData =
            AppFunctionData.Builder(
                    requireTargetObjectTypeMetadata(
                        "parameters",
                        createNoteMetadata.parameters,
                        createNoteMetadata.components,
                    ),
                    createNoteMetadata.components,
                )
                .setString("title", "Test Title")
                .setString("content", "Some valid content")
                .setAppFunctionDataList("attachments", emptyList())
                .setString("groupId", "testGroupId")
                .setString("externalUuid", "testExternalUuid")
                .build()
        assertThrows(IllegalArgumentException::class.java) {
            AppFunctionData.Builder(createNoteMetadata.parameters, createNoteMetadata.components)
                .setAppFunctionData("wrongParameters", innerData)
                .build()
        }
    }

    @Test
    fun prepareAppFunctionData_wrongNestedParameterName_fail() = doBlocking {
        val createNoteMetadata =
            appFunctionManager
                .observeAppFunctions(
                    AppFunctionSearchSpec(
                        packageNames = setOf(context.packageName),
                        schemaCategory = "myNotes",
                        schemaName = "createNote",
                        minSchemaVersion = 2,
                    )
                )
                .first()
                .flatMap { it.appFunctions }
                .single()

        assertThrows(IllegalArgumentException::class.java) {
            AppFunctionData.Builder(
                    requireTargetObjectTypeMetadata(
                        "parameters",
                        createNoteMetadata.parameters,
                        createNoteMetadata.components,
                    ),
                    createNoteMetadata.components,
                )
                .setString("wrongTitle", "Test Title")
                .build()
        }
    }

    @Test
    fun echoClassWithOptionalValues_allValuesProvided_shouldNotReturnDefault() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#echoClassWithOptionalValues"
            )
        val classWithOptionalValues =
            ClassWithOptionalValues(
                optionalNonNullInt = 1,
                optionalNullableInt = 2,
                optionalNonNullLong = 100L,
                optionalNullableLong = 200L,
                optionalNonNullBoolean = true,
                optionalNullableBoolean = true,
                optionalNonNullFloat = 10.5f,
                optionalNullableFloat = 20.5f,
                optionalNonNullDouble = 100.5,
                optionalNullableDouble = 200.5,
                optionalNullableString = "Initialized String",
                optionalNullableSerializable = Owner("John"),
                optionalNullableProxySerializable = LocalDateTime.now(),
                optionalNonNullIntArray = intArrayOf(1, 2, 3),
                optionalNullableIntArray = intArrayOf(4, 5, 6),
                optionalNonNullLongArray = longArrayOf(1L, 2L, 3L),
                optionalNullableLongArray = longArrayOf(4L, 5L, 6L),
                optionalNonNullBooleanArray = booleanArrayOf(true, false),
                optionalNullableBooleanArray = booleanArrayOf(false, true),
                optionalNonNullFloatArray = floatArrayOf(1.1f, 2.2f),
                optionalNullableFloatArray = floatArrayOf(3.3f, 4.4f),
                optionalNonNullDoubleArray = doubleArrayOf(11.1, 22.2),
                optionalNullableDoubleArray = doubleArrayOf(33.3, 44.4),
                optionalNonNullByteArray = byteArrayOf(1, 0, 1),
                optionalNullableByteArray = byteArrayOf(0, 1, 0),
                optionalNonNullListString = listOf("A", "B", "C"),
                optionalNullableListString = listOf("D", "E", "F"),
                optionalNonNullSerializableList = listOf(Owner("Alice"), Owner("Bob")),
                optionalNullableSerializableList = listOf(Owner("Charlie")),
                optionalNonNullProxySerializableList = listOf(LocalDateTime.now().minusDays(1)),
                optionalNullableProxySerializableList = listOf(LocalDateTime.now().plusDays(1)),
            )

        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setAppFunctionData(
                                "classWithOptionalValues",
                                AppFunctionData.serialize(
                                    classWithOptionalValues,
                                    ClassWithOptionalValues::class.java,
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        assertThat(
                checkNotNull(
                        successResponse.returnValue.getAppFunctionData(
                            ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                        )
                    )
                    .deserialize(ClassWithOptionalValues::class.java)
            )
            .isEqualTo(classWithOptionalValues)
    }

    @Test
    fun echoClassWithOptionalValues_noValueProvided_shouldReturnAppFunctionDefinedDefault() =
        doBlocking {
            val metadata =
                findAppFunctionMetadata(
                    "androidx.appfunctions.integration.tests.TestFunctions#echoClassWithOptionalValues"
                )
            val response =
                appFunctionManager.executeAppFunction(
                    request =
                        ExecuteAppFunctionRequest(
                            metadata.packageName,
                            metadata.id,
                            AppFunctionData.Builder(metadata.parameters, metadata.components)
                                .setAppFunctionData(
                                    "classWithOptionalValues",
                                    AppFunctionData.EMPTY,
                                )
                                .build(),
                        )
                )

            val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
            assertThat(
                    checkNotNull(
                            successResponse.returnValue.getAppFunctionData(
                                ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                            )
                        )
                        .deserialize(ClassWithOptionalValues::class.java)
                )
                .isEqualTo(
                    ClassWithOptionalValues(
                        optionalNonNullInt = 0,
                        optionalNullableInt = null,
                        optionalNonNullLong = 0L,
                        optionalNullableLong = null,
                        optionalNonNullBoolean = false,
                        optionalNullableBoolean = null,
                        optionalNonNullFloat = 0.0f,
                        optionalNullableFloat = null,
                        optionalNonNullDouble = 0.0,
                        optionalNullableDouble = null,
                        optionalNullableString = null,
                        optionalNullableSerializable = null,
                        optionalNullableProxySerializable = null,
                        optionalNonNullIntArray = intArrayOf(),
                        optionalNullableIntArray = null,
                        optionalNonNullLongArray = longArrayOf(),
                        optionalNullableLongArray = null,
                        optionalNonNullBooleanArray = booleanArrayOf(),
                        optionalNullableBooleanArray = null,
                        optionalNonNullFloatArray = floatArrayOf(),
                        optionalNullableFloatArray = null,
                        optionalNonNullDoubleArray = doubleArrayOf(),
                        optionalNullableDoubleArray = null,
                        optionalNonNullByteArray = byteArrayOf(),
                        optionalNullableByteArray = null,
                        optionalNonNullListString = listOf(),
                        optionalNullableListString = null,
                        optionalNonNullSerializableList = listOf(),
                        optionalNullableSerializableList = null,
                        optionalNonNullProxySerializableList = listOf(),
                        optionalNullableProxySerializableList = null,
                    )
                )
        }

    @Test
    fun echoFunctionWithOptionalParameters_allValuesProvided_shouldNotReturnDefault() = doBlocking {
        val metadata =
            findAppFunctionMetadata(
                "androidx.appfunctions.integration.tests.TestFunctions#echoFunctionWithOptionalParameters"
            )
        val response =
            appFunctionManager.executeAppFunction(
                request =
                    ExecuteAppFunctionRequest(
                        metadata.packageName,
                        metadata.id,
                        AppFunctionData.Builder(metadata.parameters, metadata.components)
                            .setInt("optionalNonNullInt", 1)
                            .setInt("optionalNullableInt", 2)
                            .setLong("optionalNonNullLong", 100L)
                            .setLong("optionalNullableLong", 200L)
                            .setBoolean("optionalNonNullBoolean", true)
                            .setBoolean("optionalNullableBoolean", true)
                            .setFloat("optionalNonNullFloat", 10.5f)
                            .setFloat("optionalNullableFloat", 20.5f)
                            .setDouble("optionalNonNullDouble", 100.5)
                            .setDouble("optionalNullableDouble", 200.5)
                            .setString("optionalNullableString", "Initialized String")
                            .setAppFunctionData(
                                "optionalNullableSerializable",
                                AppFunctionData.serialize(Owner("John"), Owner::class.java),
                            )
                            .setAppFunctionData(
                                "optionalNullableProxySerializable",
                                AppFunctionData.serialize(
                                    LocalDateTime.of(2025, 7, 4, 12, 0),
                                    LocalDateTime::class.java,
                                ),
                            )
                            .setIntArray("optionalNonNullIntArray", intArrayOf(1, 2, 3))
                            .setIntArray("optionalNullableIntArray", intArrayOf(4, 5, 6))
                            .setLongArray("optionalNonNullLongArray", longArrayOf(1L, 2L, 3L))
                            .setLongArray("optionalNullableLongArray", longArrayOf(4L, 5L, 6L))
                            .setBooleanArray(
                                "optionalNonNullBooleanArray",
                                booleanArrayOf(true, false),
                            )
                            .setBooleanArray(
                                "optionalNullableBooleanArray",
                                booleanArrayOf(false, true),
                            )
                            .setFloatArray("optionalNonNullFloatArray", floatArrayOf(1.1f, 2.2f))
                            .setFloatArray("optionalNullableFloatArray", floatArrayOf(3.3f, 4.4f))
                            .setDoubleArray("optionalNonNullDoubleArray", doubleArrayOf(11.1, 22.2))
                            .setDoubleArray(
                                "optionalNullableDoubleArray",
                                doubleArrayOf(33.3, 44.4),
                            )
                            .setByteArray("optionalNonNullByteArray", byteArrayOf(1, 0, 1))
                            .setByteArray("optionalNullableByteArray", byteArrayOf(0, 1, 0))
                            .setStringList("optionalNonNullListString", listOf("A", "B", "C"))
                            .setStringList("optionalNullableListString", listOf("D", "E", "F"))
                            .setAppFunctionDataList(
                                "optionalNonNullSerializableList",
                                listOf(
                                    AppFunctionData.serialize(Owner("Alice"), Owner::class.java),
                                    AppFunctionData.serialize(Owner("Bob"), Owner::class.java),
                                ),
                            )
                            .setAppFunctionDataList(
                                "optionalNullableSerializableList",
                                listOf(
                                    AppFunctionData.serialize(Owner("Charlie"), Owner::class.java)
                                ),
                            )
                            .setAppFunctionDataList(
                                "optionalNonNullProxySerializableList",
                                listOf(
                                    AppFunctionData.serialize(
                                        LocalDateTime.of(2025, 7, 4, 12, 0),
                                        LocalDateTime::class.java,
                                    )
                                ),
                            )
                            .setAppFunctionDataList(
                                "optionalNullableProxySerializableList",
                                listOf(
                                    AppFunctionData.serialize(
                                        LocalDateTime.of(2025, 7, 4, 12, 0),
                                        LocalDateTime::class.java,
                                    )
                                ),
                            )
                            .build(),
                    )
            )

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        assertThat(
                checkNotNull(
                        successResponse.returnValue.getAppFunctionData(
                            ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                        )
                    )
                    .deserialize(ClassWithOptionalValues::class.java)
            )
            .isEqualTo(
                ClassWithOptionalValues(
                    optionalNonNullInt = 1,
                    optionalNullableInt = 2,
                    optionalNonNullLong = 100L,
                    optionalNullableLong = 200L,
                    optionalNonNullBoolean = true,
                    optionalNullableBoolean = true,
                    optionalNonNullFloat = 10.5f,
                    optionalNullableFloat = 20.5f,
                    optionalNonNullDouble = 100.5,
                    optionalNullableDouble = 200.5,
                    optionalNullableString = "Initialized String",
                    optionalNullableSerializable = Owner("John"),
                    optionalNullableProxySerializable = LocalDateTime.of(2025, 7, 4, 12, 0),
                    optionalNonNullIntArray = intArrayOf(1, 2, 3),
                    optionalNullableIntArray = intArrayOf(4, 5, 6),
                    optionalNonNullLongArray = longArrayOf(1L, 2L, 3L),
                    optionalNullableLongArray = longArrayOf(4L, 5L, 6L),
                    optionalNonNullBooleanArray = booleanArrayOf(true, false),
                    optionalNullableBooleanArray = booleanArrayOf(false, true),
                    optionalNonNullFloatArray = floatArrayOf(1.1f, 2.2f),
                    optionalNullableFloatArray = floatArrayOf(3.3f, 4.4f),
                    optionalNonNullDoubleArray = doubleArrayOf(11.1, 22.2),
                    optionalNullableDoubleArray = doubleArrayOf(33.3, 44.4),
                    optionalNonNullByteArray = byteArrayOf(1, 0, 1),
                    optionalNullableByteArray = byteArrayOf(0, 1, 0),
                    optionalNonNullListString = listOf("A", "B", "C"),
                    optionalNullableListString = listOf("D", "E", "F"),
                    optionalNonNullSerializableList = listOf(Owner("Alice"), Owner("Bob")),
                    optionalNullableSerializableList = listOf(Owner("Charlie")),
                    optionalNonNullProxySerializableList =
                        listOf(LocalDateTime.of(2025, 7, 4, 12, 0)),
                    optionalNullableProxySerializableList =
                        listOf(LocalDateTime.of(2025, 7, 4, 12, 0)),
                )
            )
    }

    @Test
    fun echoFunctionWithOptionalParameters_noValueProvided_shouldReturnAppFunctionDefinedDefault() =
        doBlocking {
            val metadata =
                findAppFunctionMetadata(
                    "androidx.appfunctions.integration.tests.TestFunctions#echoFunctionWithOptionalParameters"
                )
            val response =
                appFunctionManager.executeAppFunction(
                    request =
                        ExecuteAppFunctionRequest(
                            metadata.packageName,
                            metadata.id,
                            AppFunctionData.Builder(metadata.parameters, metadata.components)
                                .build(),
                        )
                )

            val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
            assertThat(
                    checkNotNull(
                            successResponse.returnValue.getAppFunctionData(
                                ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                            )
                        )
                        .deserialize(ClassWithOptionalValues::class.java)
                )
                .isEqualTo(
                    ClassWithOptionalValues(
                        optionalNonNullInt = 0,
                        optionalNullableInt = null,
                        optionalNonNullLong = 0L,
                        optionalNullableLong = null,
                        optionalNonNullBoolean = false,
                        optionalNullableBoolean = null,
                        optionalNonNullFloat = 0.0f,
                        optionalNullableFloat = null,
                        optionalNonNullDouble = 0.0,
                        optionalNullableDouble = null,
                        optionalNullableString = null,
                        optionalNullableSerializable = null,
                        optionalNullableProxySerializable = null,
                        optionalNonNullIntArray = intArrayOf(),
                        optionalNullableIntArray = null,
                        optionalNonNullLongArray = longArrayOf(),
                        optionalNullableLongArray = null,
                        optionalNonNullBooleanArray = booleanArrayOf(),
                        optionalNullableBooleanArray = null,
                        optionalNonNullFloatArray = floatArrayOf(),
                        optionalNullableFloatArray = null,
                        optionalNonNullDoubleArray = doubleArrayOf(),
                        optionalNullableDoubleArray = null,
                        optionalNonNullByteArray = byteArrayOf(),
                        optionalNullableByteArray = null,
                        optionalNonNullListString = listOf(),
                        optionalNullableListString = null,
                        optionalNonNullSerializableList = listOf(),
                        optionalNullableSerializableList = null,
                        optionalNonNullProxySerializableList = listOf(),
                        optionalNullableProxySerializableList = null,
                    )
                )
        }

    @Test
    fun executeAppFunction_getFilesData_validUriAccess() = doBlocking {
        val request =
            ExecuteAppFunctionRequest(
                targetPackageName = context.packageName,
                functionIdentifier =
                    "androidx.appfunctions.integration.tests.TestFunctions#getFilesData",
                functionParameters = AppFunctionData.EMPTY,
            )

        val response = appFunctionManager.executeAppFunction(request)

        val successResponse = assertIs<ExecuteAppFunctionResponse.Success>(response)
        val filesData =
            successResponse.returnValue
                .getAppFunctionData(ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE)
                ?.deserialize(FilesData::class.java)
        assertThat(filesData).isNotNull()
        targetContext.assertReadAccessible(checkNotNull(filesData).readOnlyUri.uri)
        targetContext.assertWriteInaccessible(filesData.readOnlyUri.uri)
        targetContext.assertReadInaccessible(filesData.writeOnlyUri.uri)
        targetContext.assertWriteAccessible(filesData.writeOnlyUri.uri)
        targetContext.assertReadAccessible(filesData.readWriteUri.uri)
        targetContext.assertWriteAccessible(filesData.readWriteUri.uri)
    }

    @Test
    fun executeAppFunction_requestCancellation_isIsolated() = doBlocking {
        val requestA =
            ExecuteAppFunctionRequest(
                targetPackageName = context.packageName,
                functionIdentifier = TestFunctionsIds.LONG_RUNNING_FUNCTION_ID,
                functionParameters = AppFunctionData.EMPTY,
            )
        val requestB =
            ExecuteAppFunctionRequest(
                targetPackageName = context.packageName,
                functionIdentifier = TestFunctionsIds.LONG_RUNNING_FUNCTION_ID,
                functionParameters = AppFunctionData.EMPTY,
            )
        // Execute two functions simultaneously
        val responseADeferred = async { appFunctionManager.executeAppFunction(requestA) }
        val responseBDeferred = async { appFunctionManager.executeAppFunction(requestB) }

        // Cancel responseA execution
        responseADeferred.cancel()

        // Assert responseB is completed successfully
        val successResponse =
            assertIs<ExecuteAppFunctionResponse.Success>(responseBDeferred.await())
        assertThat(
                successResponse.returnValue.getString(
                    ExecuteAppFunctionResponse.Success.PROPERTY_RETURN_VALUE
                )
            )
            .isEqualTo("Completed")
    }

    @Test
    fun serializeAppFunctionSerializable_failsForInvalidValues() {
        assertFailsWith<IllegalArgumentException> {
            AppFunctionData.serialize(
                IntEnumSerializable(value = -1),
                IntEnumSerializable::class.java,
            )
        }
    }

    @Test
    fun serializeAppFunctionSerializable_success() {
        val afd =
            AppFunctionData.serialize(
                IntEnumSerializable(value = 10),
                IntEnumSerializable::class.java,
            )

        assertThat(afd.getInt("value")).isEqualTo(10)
    }

    @Test
    fun deserializeAppFunctionSerializable_failsForInvalidValues() {
        assertFailsWith<IllegalArgumentException> {
            AppFunctionData.Builder(
                    listOf(
                        AppFunctionParameterMetadata(
                            name = "value",
                            isRequired = false,
                            dataType = AppFunctionIntTypeMetadata(isNullable = true),
                        )
                    ),
                    AppFunctionComponentsMetadata(),
                )
                .setInt("value", -1)
                .build()
                .deserialize(IntEnumSerializable::class.java)
        }
    }

    @Test
    fun deserializeAppFunctionSerializable_success() {
        val intEnumSerializable =
            AppFunctionData.Builder(
                    listOf(
                        AppFunctionParameterMetadata(
                            name = "value",
                            isRequired = false,
                            dataType = AppFunctionIntTypeMetadata(isNullable = true),
                        )
                    ),
                    AppFunctionComponentsMetadata(),
                )
                .setInt("value", 10)
                .build()
                .deserialize(IntEnumSerializable::class.java)

        assertThat(intEnumSerializable.value).isEqualTo(10)
    }

    /**
     * Requires that [parameters] contains the [AppFunctionObjectTypeMetadata] under
     * [parameterName].
     *
     * @throws IllegalArgumentException If unable to find the target
     *   [AppFunctionObjectTypeMetadata].
     */
    private fun requireTargetObjectTypeMetadata(
        parameterName: String,
        parameters: List<AppFunctionParameterMetadata>,
        components: AppFunctionComponentsMetadata,
    ): AppFunctionObjectTypeMetadata {
        val targetParameterMetadata =
            parameters.find { it.name == parameterName }
                ?: throw IllegalArgumentException(
                    "Unable to find parameter metadata with name $parameterName"
                )
        val parameterDataTypeMetadata = targetParameterMetadata.dataType
        return when (parameterDataTypeMetadata) {
            is AppFunctionObjectTypeMetadata -> {
                parameterDataTypeMetadata
            }
            is AppFunctionReferenceTypeMetadata -> {
                components.dataTypes[parameterDataTypeMetadata.referenceDataType]
                    as? AppFunctionObjectTypeMetadata
                    ?: throw IllegalArgumentException(
                        "Unable to find object metadata with reference name ${parameterDataTypeMetadata.referenceDataType}"
                    )
            }
            else -> {
                throw IllegalArgumentException(
                    "The parameter metadata of $parameterName is not an object type."
                )
            }
        }
    }

    private suspend fun findAppFunctionMetadata(id: String): AppFunctionMetadata {
        return appFunctionManager
            .observeAppFunctions(AppFunctionSearchSpec())
            .first()
            .flatMap { it.appFunctions }
            .single { it.id == id }
    }

    private suspend fun Context.awaitAppFunctionsIndexed(
        targetPackage: String,
        expectedFunctionIds: Set<String>,
    ) {
        retryAssert {
            val functionIds =
                AppSearchMetadataHelper.collectFunctionIds(
                    this@awaitAppFunctionsIndexed,
                    targetPackage,
                )
            assertThat(functionIds).containsAtLeastElementsIn(expectedFunctionIds)
        }
    }

    private suspend fun Context.installApk(apk: String) {
        val installer = packageManager.packageInstaller
        val sessionParams =
            PackageInstaller.SessionParams(PackageInstaller.SessionParams.MODE_FULL_INSTALL)

        val sessionId = installer.createSession(sessionParams)

        installer.openSession(sessionId).use { session ->
            session.openWrite("apk_install", 0, -1).use { outputStream ->
                getResourceAsStream(apk).transferTo(outputStream)
            }
            assertThat(session.commitSession(this@installApk)).isTrue()
        }
    }

    fun getResourceAsStream(name: String): InputStream {
        return checkNotNull(Thread.currentThread().contextClassLoader).getResourceAsStream(name)
    }

    @OptIn(ExperimentalCoroutinesApi::class)
    private suspend fun PackageInstaller.Session.commitSession(context: Context): Boolean {
        val action = "com.example.COMMIT_COMPLETE.${System.currentTimeMillis()}"

        return suspendCancellableCoroutine { continuation ->
            val receiver =
                object : BroadcastReceiver() {
                    override fun onReceive(context: Context, intent: Intent) {
                        context.unregisterReceiver(this)

                        val status =
                            intent.getIntExtra(
                                PackageInstaller.EXTRA_STATUS,
                                PackageInstaller.STATUS_FAILURE,
                            )
                        val message = intent.getStringExtra(PackageInstaller.EXTRA_STATUS_MESSAGE)

                        if (status == PackageInstaller.STATUS_SUCCESS) {
                            continuation.resume(true) { cause, _, _ -> }
                        } else {
                            continuation.resumeWithException(
                                Exception("Installation failed: $message")
                            )
                        }
                    }
                }

            val filter = IntentFilter(action)
            context.registerReceiver(receiver, filter, Context.RECEIVER_EXPORTED)

            val intent = Intent(action).setPackage(context.packageName)
            val sender =
                PendingIntent.getBroadcast(
                    context,
                    0,
                    intent,
                    PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE,
                )

            this.commit(sender.intentSender)

            continuation.invokeOnCancellation {
                // Unregister the receiver if the coroutine is cancelled
                context.unregisterReceiver(receiver)
            }
        }
    }

    private companion object {
        const val ADDITIONAL_APK_FILE = "notes.apk"
        const val ADDITIONAL_APP_PACKAGE = "com.google.android.app.notes"
        const val ADDITIONAL_LEGACY_CREATE_NOTE =
            "com.example.android.architecture.blueprints.todoapp#NoteFunctions_createNote"
        const val TEST_APP_FUNCTION_DOC_SIZE_LIMIT = 512 * 1024 // 512kb

        val FUNCTION_IDS =
            setOf<String>(
                "androidx.appfunctions.integration.tests.TestFunctions#add",
                "androidx.appfunctions.integration.tests.TestFunctions#doThrow",
                "androidx.appfunctions.integration.tests.TestFunctions#voidFunction",
                "androidx.appfunctions.integration.tests.TestFunctions#createNote",
                "androidx.appfunctions.integration.tests.TestFunctions#updateNote",
                "androidx.appfunctions.integration.tests.TestFunctions#logLocalDateTime",
                "androidx.appfunctions.integration.tests.TestFunctions#getLocalDate",
                "androidx.appfunctions.integration.tests.TestFunctions#getOpenableNote",
                "androidx.appfunctions.integration.tests.TestFactory#isCreatedByFactory",
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#concat",
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#logUri",
                "androidx.appfunctions.integration.testapp.library.TestFunctions2#getUri",
            )
    }
}
