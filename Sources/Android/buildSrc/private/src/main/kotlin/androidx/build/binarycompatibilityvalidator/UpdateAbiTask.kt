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

package androidx.build.binarycompatibilityvalidator

import androidx.binarycompatibilityvalidator.KlibDumpParser
import androidx.binarycompatibilityvalidator.ParseException
import javax.inject.Inject
import org.gradle.api.DefaultTask
import org.gradle.api.GradleException
import org.gradle.api.file.ConfigurableFileCollection
import org.gradle.api.file.DirectoryProperty
import org.gradle.api.file.FileSystemOperations
import org.gradle.api.file.RegularFileProperty
import org.gradle.api.provider.ListProperty
import org.gradle.api.provider.Property
import org.gradle.api.tasks.CacheableTask
import org.gradle.api.tasks.Classpath
import org.gradle.api.tasks.Input
import org.gradle.api.tasks.InputFile
import org.gradle.api.tasks.Internal
import org.gradle.api.tasks.OutputDirectory
import org.gradle.api.tasks.PathSensitive
import org.gradle.api.tasks.PathSensitivity
import org.gradle.api.tasks.TaskAction
import org.gradle.workers.WorkAction
import org.gradle.workers.WorkParameters
import org.gradle.workers.WorkerExecutor
import org.jetbrains.kotlin.library.abi.ExperimentalLibraryAbiReader

@CacheableTask
abstract class UpdateAbiTask
@Inject
constructor(@Internal protected val workerExecutor: WorkerExecutor) : DefaultTask() {

    @get:Inject abstract val fileSystemOperations: FileSystemOperations

    @get:Input abstract val version: Property<String>

    @get:Input abstract val shouldWriteVersionedApiFile: Property<Boolean>

    @get:Input abstract val unsupportedNativeTargetNames: ListProperty<String>

    /** Text file from which API signatures will be read. */
    @get:PathSensitive(PathSensitivity.RELATIVE)
    @get:InputFile
    abstract val inputApiLocation: RegularFileProperty

    /** Directory to which API signatures will be written. */
    @get:OutputDirectory abstract val outputDir: DirectoryProperty

    @get:Classpath abstract val runtimeClasspath: ConfigurableFileCollection

    @TaskAction
    fun execute() {
        unsupportedNativeTargetNames.get().let { targets ->
            if (targets.isNotEmpty()) {
                throw GradleException(
                    "Cannot update API files because the current host doesn't support the " +
                        "following targets: ${targets.joinToString(", ")}"
                )
            }
        }
        fileSystemOperations.copy {
            it.from(inputApiLocation)
            it.into(outputDir)
        }
        if (shouldWriteVersionedApiFile.get()) {
            fileSystemOperations.copy {
                it.from(inputApiLocation)
                it.into(outputDir)
                it.rename(CURRENT_API_FILE_NAME, "${version.get()}.txt")
            }
        }

        // Execute BCV code as a WorkAction to allow setting the classpath for the action.
        // This is to work around the kotlin compiler needing to be a compileOnly dependency for
        // buildSrc (https://kotl.in/gradle/internal-compiler-symbols, aosp/3368960).
        val workQueue = workerExecutor.classLoaderIsolation { it.classpath.from(runtimeClasspath) }
        workQueue.submit(UpdateAbiWorker::class.java) { params ->
            params.abiFile.set(outputDir.file("current.txt"))
        }
    }
}

private interface UpdateAbiParameters : WorkParameters {
    val abiFile: RegularFileProperty
}

private abstract class UpdateAbiWorker : WorkAction<UpdateAbiParameters> {
    @OptIn(ExperimentalLibraryAbiReader::class)
    override fun execute() {
        try {
            KlibDumpParser(parameters.abiFile.get().asFile).parse()
        } catch (e: ParseException) {
            System.err.println(
                "Successfully updated API file but parser was unable to parse the generated output. " +
                    "This is a bug in the parser and should be filed to $NEW_ISSUE_URL"
            )
            e.printStackTrace()
        }
    }
}
