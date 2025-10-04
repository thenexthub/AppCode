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
package androidx.paging.rxjava2

import android.database.Cursor
import androidx.paging.PagingState
import androidx.room3.RoomDatabase
import androidx.room3.RoomSQLiteQuery
import androidx.room3.paging.CursorSQLiteStatement
import androidx.sqlite.SQLiteStatement

@Suppress("UNUSED_PARAMETER")
abstract class RxPagingSource<K : Any, T : Any>(
    private val sourceQuery: RoomSQLiteQuery,
    private val db: RoomDatabase,
    vararg tables: String
) : androidx.paging.PagingSource<K, T>() {
    override fun getRefreshKey(state: PagingState<K, T>): K? {
        return null
    }

    override public suspend fun load(params: LoadParams<K>): LoadResult<K, T> {
        return LoadResult.Invalid()
    }

    protected open fun convertRows(cursor: Cursor): List<T> {
        return convertRows(CursorSQLiteStatement(cursor))
    }

    protected open fun convertRows(statement: SQLiteStatement): List<T> {
        throw NotImplementedError(
            "Unexpected call to a function with no implementation that Room is suppose to " +
                "generate."
        )
    }
}