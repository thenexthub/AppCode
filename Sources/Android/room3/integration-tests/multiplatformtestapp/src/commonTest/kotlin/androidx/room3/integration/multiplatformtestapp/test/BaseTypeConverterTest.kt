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

package androidx.room3.integration.multiplatformtestapp.test

import androidx.kruth.assertThat
import androidx.kruth.assertThrows
import androidx.room3.ConstructedBy
import androidx.room3.Dao
import androidx.room3.Database
import androidx.room3.Entity
import androidx.room3.Insert
import androidx.room3.PrimaryKey
import androidx.room3.ProvidedTypeConverter
import androidx.room3.Query
import androidx.room3.RoomDatabase
import androidx.room3.RoomDatabaseConstructor
import androidx.room3.TypeConverter
import androidx.room3.TypeConverters
import androidx.room3.integration.multiplatformtestapp.test.BaseTypeConverterTest.TestDatabase
import kotlin.test.Test
import kotlinx.coroutines.test.runTest

abstract class BaseTypeConverterTest {
    abstract fun getDatabaseBuilder(): RoomDatabase.Builder<TestDatabase>

    @Test
    fun entityWithConverter() = runTest {
        val database = getDatabaseBuilder().addTypeConverter(BarConverter()).build()
        val entity = TestEntity(1, Foo(1979), Bar("Mujer Boricua"))
        database.getDao().insertItem(entity)
        assertThat(database.getDao().getItem(1)).isEqualTo(entity)
        database.close()
    }

    @Test
    fun entityWithSubclassedConverter() = runTest {
        val database = getDatabaseBuilder().addTypeConverter(SubBarConverter()).build()
        val entity = TestEntity(1, Foo(2018), Bar("Estamos Bien"))
        database.getDao().insertItem(entity)
        assertThat(database.getDao().getItem(1)).isEqualTo(entity)
        database.close()
    }

    @Test
    fun missingTypeConverter() {
        assertThrows<IllegalArgumentException> { getDatabaseBuilder().build() }
            .hasMessageThat()
            .isEqualTo(
                "A required type converter (" +
                    "${BarConverter::class.qualifiedName}) for ${TestDao::class.qualifiedName} is " +
                    "missing in the database configuration."
            )
    }

    @Test
    fun extraTypeConverter() {
        assertThrows<IllegalArgumentException> {
                getDatabaseBuilder()
                    .addTypeConverter(BarConverter())
                    .addTypeConverter(FooConverter)
                    .build()
            }
            .hasMessageThat()
            .startsWith("Unexpected type converter")
    }

    @Database(entities = [TestEntity::class], version = 1, exportSchema = false)
    @TypeConverters(FooConverter::class, BarConverter::class)
    @ConstructedBy(BaseTypeConverterTest_TestDatabaseConstructor::class)
    abstract class TestDatabase : RoomDatabase() {
        abstract fun getDao(): TestDao
    }

    @Dao
    interface TestDao {
        @Insert suspend fun insertItem(item: TestEntity)

        @Query("SELECT * FROM TestEntity WHERE id = :id") suspend fun getItem(id: Long): TestEntity
    }

    @Entity data class TestEntity(@PrimaryKey val id: Long, val foo: Foo, val bar: Bar)

    data class Foo(val number: Int)

    data class Bar(val text: String)

    object FooConverter {
        @TypeConverter fun toFoo(number: Int): Foo = Foo(number)

        @TypeConverter fun fromFoo(foo: Foo): Int = foo.number
    }

    @ProvidedTypeConverter
    open class BarConverter {
        @TypeConverter fun toBar(text: String): Bar = Bar(text)

        @TypeConverter fun fromBar(bar: Bar): String = bar.text
    }

    class SubBarConverter : BarConverter()
}

expect object BaseTypeConverterTest_TestDatabaseConstructor : RoomDatabaseConstructor<TestDatabase>
