/*
 * Copyright 2022 The Android Open Source Project
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

package androidx.room3.integration.kotlintestapp.test

import androidx.kruth.assertThat
import androidx.kruth.assertThrows
import androidx.room3.ExperimentalRoomApi
import androidx.room3.Room
import androidx.room3.integration.kotlintestapp.TestDatabase
import androidx.room3.integration.kotlintestapp.dao.BooksDao
import androidx.room3.integration.kotlintestapp.test.TestDatabaseTest.UseDriver
import androidx.room3.withReadTransaction
import androidx.room3.withWriteTransaction
import androidx.sqlite.driver.AndroidSQLiteDriver
import androidx.sqlite.driver.bundled.BundledSQLiteDriver
import androidx.test.core.app.ApplicationProvider
import androidx.test.filters.MediumTest
import kotlin.time.Duration.Companion.minutes
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.buffer
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.flow.produceIn
import kotlinx.coroutines.launch
import kotlinx.coroutines.test.TestScope
import kotlinx.coroutines.test.runTest
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.Parameterized
import org.junit.runners.Parameterized.Parameters

@OptIn(ExperimentalCoroutinesApi::class)
@MediumTest
@RunWith(Parameterized::class)
class InvalidationTrackerFlowTest(private val useDriver: UseDriver) {

    private companion object {
        @JvmStatic
        @Parameters(name = "useDriver={0}")
        fun parameters() = arrayOf(UseDriver.BUNDLED, UseDriver.ANDROID)
    }

    private val testCoroutineScope = TestScope()

    private lateinit var database: TestDatabase
    private lateinit var booksDao: BooksDao

    @Before
    fun setup() {
        database =
            Room.inMemoryDatabaseBuilder(
                    ApplicationProvider.getApplicationContext(),
                    TestDatabase::class.java,
                )
                .apply {
                    if (useDriver == UseDriver.ANDROID) {
                        setDriver(AndroidSQLiteDriver())
                    } else if (useDriver == UseDriver.BUNDLED) {
                        setDriver(BundledSQLiteDriver())
                    }
                }
                .setQueryCoroutineContext(testCoroutineScope.coroutineContext)
                .build()
        booksDao = database.booksDao()
    }

    @Test
    fun initiallyEmitAllTableNames(): Unit = runTest {
        val result = database.invalidationTracker.createFlow("author", "publisher", "book").first()
        assertThat(result).containsExactly("author", "publisher", "book")
    }

    @Test
    fun initiallyEmitNothingWhenLazy(): Unit = runTest {
        val channel =
            database.invalidationTracker.createFlow("author", "publisher", "book").produceIn(this)

        testScheduler.advanceUntilIdle()

        assertThat(channel.isEmpty)

        channel.cancel()
    }

    @Test
    fun invalidationEmitTableNames(): Unit = runTest {
        booksDao.addAuthors(TestUtil.AUTHOR_1)
        booksDao.addPublishers(TestUtil.PUBLISHER)
        booksDao.addBooks(TestUtil.BOOK_1)

        val channel =
            database.invalidationTracker.createFlow("author", "publisher", "book").produceIn(this)

        assertThat(channel.receive()).isEqualTo(setOf("author", "publisher", "book"))

        booksDao.insertBookSuspend(TestUtil.BOOK_2)
        testScheduler.advanceUntilIdle()

        assertThat(channel.receive()).containsExactly("book")

        booksDao.addPublisher(TestUtil.PUBLISHER2)
        testScheduler.advanceUntilIdle()

        assertThat(channel.receive()).containsExactly("publisher")

        assertThat(channel.isEmpty).isTrue()

        channel.cancel()
    }

    @Test
    @OptIn(ExperimentalRoomApi::class)
    fun emitOnceForMultipleTablesInTransaction(): Unit = runTest {
        val resultChannel = Channel<Set<String>>(capacity = 10)
        val job =
            backgroundScope.launch(Dispatchers.IO) {
                database.invalidationTracker.createFlow("author", "publisher", "book").collect {
                    resultChannel.send(it)
                }
            }

        testScheduler.advanceUntilIdle()

        database.withWriteTransaction {
            booksDao.addAuthors(TestUtil.AUTHOR_1)
            booksDao.addPublishers(TestUtil.PUBLISHER)
            booksDao.addBooks(TestUtil.BOOK_1)
        }

        testScheduler.advanceUntilIdle()

        val result = resultChannel.receive()
        assertThat(result).containsExactly("author", "publisher", "book")
        assertThat(result.isEmpty())

        resultChannel.close()
        job.cancel()
    }

    @Test
    fun dropInvalidationUsingConflated() = runTest {
        val channel =
            database.invalidationTracker
                .createFlow("author", "publisher", "book")
                .buffer(Channel.CONFLATED)
                .produceIn(this)

        booksDao.addAuthors(TestUtil.AUTHOR_1)
        testScheduler.advanceUntilIdle()

        booksDao.addPublishers(TestUtil.PUBLISHER)
        testScheduler.advanceUntilIdle()

        booksDao.addBooks(TestUtil.BOOK_1)
        testScheduler.advanceUntilIdle()

        assertThat(channel.receive()).containsExactly("book")
        assertThat(channel.isEmpty).isTrue()

        channel.cancel()
    }

    @Test
    @OptIn(ExperimentalRoomApi::class)
    fun collectInTransaction(): Unit = runTest {
        database.withWriteTransaction {
            val result = database.invalidationTracker.createFlow("author").first()
            assertThat(result).containsExactly("author")
        }
    }

    @Test
    fun mapBlockingQuery() = runTest {
        booksDao.addAuthors(TestUtil.AUTHOR_1)
        booksDao.addPublishers(TestUtil.PUBLISHER)
        booksDao.addBooks(TestUtil.BOOK_1)

        val channel =
            database.invalidationTracker
                .createFlow("book")
                .map { booksDao.getAllBooks() }
                .produceIn(this)

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1)

        booksDao.addBooks(TestUtil.BOOK_2)
        testScheduler.advanceUntilIdle()

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1, TestUtil.BOOK_2)

        channel.cancel()
    }

    @Test
    fun mapSuspendingQuery() = runTest {
        booksDao.addAuthors(TestUtil.AUTHOR_1)
        booksDao.addPublishers(TestUtil.PUBLISHER)
        booksDao.addBooks(TestUtil.BOOK_1)

        val channel =
            database.invalidationTracker
                .createFlow("book")
                .map { booksDao.getBooksSuspend() }
                .produceIn(this)

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1)

        booksDao.addBooks(TestUtil.BOOK_2)
        testScheduler.advanceUntilIdle()

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1, TestUtil.BOOK_2)

        channel.cancel()
    }

    @Test
    fun mapFlowQuery() = runTest {
        booksDao.addAuthors(TestUtil.AUTHOR_1)
        booksDao.addPublishers(TestUtil.PUBLISHER)
        booksDao.addBooks(TestUtil.BOOK_1)

        val channel =
            database.invalidationTracker
                .createFlow("book")
                .map { booksDao.getBooksFlow().first() }
                .produceIn(this)

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1)

        booksDao.addBooks(TestUtil.BOOK_2)
        testScheduler.advanceUntilIdle()

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1, TestUtil.BOOK_2)

        channel.cancel()
    }

    @Test
    @OptIn(ExperimentalRoomApi::class)
    fun mapTransactionQuery() = runTest {
        booksDao.addAuthors(TestUtil.AUTHOR_1)
        booksDao.addPublishers(TestUtil.PUBLISHER)
        booksDao.addBooks(TestUtil.BOOK_1)

        val channel =
            database.invalidationTracker
                .createFlow("book")
                .map { database.withReadTransaction { booksDao.getBooksSuspend() } }
                .produceIn(this)

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1)

        booksDao.addBooks(TestUtil.BOOK_2)
        testScheduler.advanceUntilIdle()

        assertThat(channel.receive()).containsExactly(TestUtil.BOOK_1, TestUtil.BOOK_2)

        channel.cancel()
    }

    @Test
    @OptIn(ExperimentalRoomApi::class)
    fun transactionUpdateAndTransactionQuery() = runTest {
        booksDao.addPublishers(TestUtil.PUBLISHER)
        booksDao.addBooks(TestUtil.BOOK_1)

        val resultChannel = Channel<List<String>>(capacity = 2)

        val job =
            backgroundScope.launch(Dispatchers.IO) {
                database.invalidationTracker
                    .createFlow("author", "publisher")
                    .map {
                        val (books, publishers) =
                            database.withReadTransaction {
                                booksDao.getBooksSuspend() to booksDao.getPublishersSuspend()
                            }
                        books.map { book ->
                            val publisherName =
                                publishers.first { it.publisherId == book.bookPublisherId }.name
                            "${book.title} from $publisherName"
                        }
                    }
                    .collect { resultChannel.send(it) }
            }

        testScheduler.advanceUntilIdle()

        resultChannel.receive().let { result ->
            assertThat(result).containsExactly("book title 1 from publisher 1")
        }

        database.withWriteTransaction {
            booksDao.addPublishersSuspend(TestUtil.PUBLISHER2)
            booksDao.addBooks(TestUtil.BOOK_2)
        }
        testScheduler.advanceUntilIdle()

        resultChannel.receive().let { result ->
            assertThat(result)
                .containsExactly("book title 1 from publisher 1", "book title 2 from publisher 1")
        }

        resultChannel.close()
        job.cancel()
    }

    @Test
    fun invalidTable() = runTest {
        assertThrows<IllegalArgumentException> {
                database.invalidationTracker.createFlow("foo").first()
            }
            .hasMessageThat()
            .isEqualTo("There is no table with name foo")

        database.close()
    }

    @Test
    fun emptyTables() = runTest {
        booksDao.addAuthors(TestUtil.AUTHOR_1)

        val channel = database.invalidationTracker.createFlow().produceIn(this)

        assertThat(channel.receive()).isEmpty()

        booksDao.addAuthorsSuspend(TestUtil.AUTHOR_2)
        testScheduler.advanceUntilIdle()

        assertThat(channel.isEmpty).isTrue()

        channel.cancel()
    }

    private fun runTest(testBody: suspend TestScope.() -> Unit) =
        testCoroutineScope.runTest(timeout = 10.minutes) {
            testBody.invoke(this)
            testScheduler.advanceUntilIdle()
            database.close()
        }
}
