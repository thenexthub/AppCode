/*
 * Copyright 2021 The Android Open Source Project
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

package androidx.room3.processor

import androidx.room3.compiler.processing.util.Source
import androidx.room3.compiler.processing.util.runProcessorTest
import androidx.room3.migration.bundle.DatabaseBundle
import androidx.room3.migration.bundle.EntityBundle
import androidx.room3.migration.bundle.FieldBundle
import androidx.room3.migration.bundle.PrimaryKeyBundle
import androidx.room3.migration.bundle.SchemaBundle
import androidx.room3.processor.ProcessorErrors.AUTOMIGRATION_SPEC_MISSING_NOARG_CONSTRUCTOR
import androidx.room3.processor.ProcessorErrors.INNER_CLASS_AUTOMIGRATION_SPEC_MUST_BE_STATIC
import androidx.room3.testing.context
import org.junit.Test

class AutoMigrationProcessorTest {
    @Test
    fun testElementHasNoArgConstructor() {
        val source =
            Source.java(
                "foo.bar.MyAutoMigration",
                """
            package foo.bar;
            import androidx.room3.migration.AutoMigrationSpec;
            public class MyAutoMigration {
                public MyAutoMigration (int x) {}
            }
            """
                    .trimIndent(),
            )

        runProcessorTest(listOf(source)) { invocation ->
            AutoMigrationProcessor(
                    context = invocation.context,
                    spec = invocation.processingEnv.requireType("foo.bar.MyAutoMigration"),
                    fromSchemaBundle = fromSchemaBundle.database,
                    toSchemaBundle = toSchemaBundle.database,
                )
                .process()
            invocation.assertCompilationResult {
                hasErrorContaining(AUTOMIGRATION_SPEC_MISSING_NOARG_CONSTRUCTOR)
            }
        }
    }

    @Test
    fun testElementIsClass() {
        val source =
            Source.java(
                "foo.bar.MyAutoMigration",
                """
            package foo.bar;
            import androidx.room3.migration.AutoMigrationSpec;
            public interface MyAutoMigration extends AutoMigrationSpec {}
            """
                    .trimIndent(),
            )

        runProcessorTest(listOf(source)) { invocation ->
            AutoMigrationProcessor(
                    context = invocation.context,
                    spec = invocation.processingEnv.requireType("foo.bar.MyAutoMigration"),
                    fromSchemaBundle = fromSchemaBundle.database,
                    toSchemaBundle = toSchemaBundle.database,
                )
                .process()
            invocation.assertCompilationResult {
                hasErrorContaining(ProcessorErrors.AUTOMIGRATION_SPEC_MUST_BE_CLASS)
            }
        }
    }

    @Test
    fun testInnerClassMustBeStatic() {
        val source =
            Source.java(
                "foo.bar.MyAutoMigrationDb",
                """
            package foo.bar;
            import androidx.room3.migration.AutoMigrationSpec;
            public class MyAutoMigrationDb {
                class MyAutoMigration implements AutoMigrationSpec {}
            }
            """
                    .trimIndent(),
            )

        runProcessorTest(listOf(source)) { invocation ->
            AutoMigrationProcessor(
                    context = invocation.context,
                    spec =
                        invocation.processingEnv.requireType(
                            "foo.bar.MyAutoMigrationDb.MyAutoMigration"
                        ),
                    fromSchemaBundle = fromSchemaBundle.database,
                    toSchemaBundle = toSchemaBundle.database,
                )
                .process()
            invocation.assertCompilationResult {
                hasErrorContaining(INNER_CLASS_AUTOMIGRATION_SPEC_MUST_BE_STATIC)
            }
        }
    }

    @Test
    fun testClassImplementsAutoMigrationSpec() {
        val source =
            Source.java(
                "foo.bar.MyAutoMigration",
                """
            package foo.bar;
            import androidx.room3.migration.AutoMigrationSpec;
            import androidx.room3.AutoMigration;
            import androidx.sqlite.db.SupportSQLiteDatabase;
            public class MyAutoMigration {}
            """
                    .trimIndent(),
            )

        runProcessorTest(listOf(source)) { invocation ->
            AutoMigrationProcessor(
                    context = invocation.context,
                    spec = invocation.processingEnv.requireType("foo.bar.MyAutoMigration"),
                    fromSchemaBundle = fromSchemaBundle.database,
                    toSchemaBundle = toSchemaBundle.database,
                )
                .process()
            invocation.assertCompilationResult {
                hasErrorContaining(
                    ProcessorErrors.autoMigrationElementMustImplementSpec("foo.bar.MyAutoMigration")
                )
            }
        }
    }

    /** Schemas for processor testing. */
    val fromSchemaBundle =
        SchemaBundle(
            1,
            DatabaseBundle(
                1,
                "",
                mutableListOf(
                    EntityBundle(
                        "Song",
                        "CREATE TABLE IF NOT EXISTS `Song` (`id` INTEGER NOT NULL, " +
                            "`title` TEXT NOT NULL, `length` INTEGER NOT NULL, PRIMARY KEY(`id`))",
                        listOf(
                            FieldBundle("id", "id", "INTEGER", true, "1"),
                            FieldBundle("title", "title", "TEXT", true, ""),
                            FieldBundle("length", "length", "INTEGER", true, "1"),
                        ),
                        PrimaryKeyBundle(false, mutableListOf("id")),
                        mutableListOf(),
                        mutableListOf(),
                    )
                ),
                mutableListOf(),
                mutableListOf(),
            ),
        )

    val toSchemaBundle =
        SchemaBundle(
            2,
            DatabaseBundle(
                2,
                "",
                mutableListOf(
                    EntityBundle(
                        "Song",
                        "CREATE TABLE IF NOT EXISTS `Song` (`id` INTEGER NOT NULL, " +
                            "`title` TEXT NOT NULL, `length` INTEGER NOT NULL, PRIMARY KEY(`id`))",
                        listOf(
                            FieldBundle("id", "id", "INTEGER", true, "1"),
                            FieldBundle("title", "title", "TEXT", true, ""),
                            FieldBundle("length", "length", "INTEGER", true, "1"),
                        ),
                        PrimaryKeyBundle(false, mutableListOf("id")),
                        mutableListOf(),
                        mutableListOf(),
                    )
                ),
                mutableListOf(),
                mutableListOf(),
            ),
        )
}
