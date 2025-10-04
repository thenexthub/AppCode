import androidx.paging.ListenableFuturePagingSource
import androidx.paging.PagingSource
import androidx.room3.RoomDatabase
import androidx.room3.RoomRawQuery
import androidx.room3.RoomSQLiteQuery
import androidx.room3.RoomSQLiteQuery.Companion.acquire
import androidx.room3.paging.LimitOffsetPagingSource
import androidx.room3.paging.guava.LimitOffsetListenableFuturePagingSource
import androidx.room3.util.getColumnIndexOrThrow
import androidx.room3.util.performSuspending
import androidx.sqlite.SQLiteStatement
import javax.`annotation`.processing.Generated
import kotlin.Int
import kotlin.Long
import kotlin.String
import kotlin.Suppress
import kotlin.collections.List
import kotlin.collections.MutableList
import kotlin.collections.mutableListOf
import kotlin.reflect.KClass
import androidx.paging.rxjava2.RxPagingSource as Rxjava2RxPagingSource
import androidx.paging.rxjava3.RxPagingSource as Rxjava3RxPagingSource
import androidx.room3.paging.rxjava2.LimitOffsetRxPagingSource as Rxjava2LimitOffsetRxPagingSource
import androidx.room3.paging.rxjava3.LimitOffsetRxPagingSource as Rxjava3LimitOffsetRxPagingSource

@Generated(value = ["androidx.room3.RoomProcessor"])
@Suppress(names = ["UNCHECKED_CAST", "DEPRECATION", "REDUNDANT_PROJECTION", "REMOVAL"])
internal class MyDao_Impl(
  __db: RoomDatabase,
) : MyDao() {
  private val __db: RoomDatabase
  init {
    this.__db = __db
  }

  public override fun getAllIds(): PagingSource<Int, MyEntity> {
    val _sql: String = "SELECT pk FROM MyEntity"
    val _rawQuery: RoomRawQuery = RoomRawQuery(_sql)
    return object : LimitOffsetPagingSource<MyEntity>(_rawQuery, __db, "MyEntity") {
      protected override suspend fun convertRows(limitOffsetQuery: RoomRawQuery, itemCount: Int): List<MyEntity> = performSuspending(__db, true, false) { _connection ->
        val _stmt: SQLiteStatement = _connection.prepare(limitOffsetQuery.sql)
        limitOffsetQuery.getBindingFunction().invoke(_stmt)
        try {
          val _columnIndexOfPk: Int = 0
          val _result: MutableList<MyEntity> = mutableListOf()
          while (_stmt.step()) {
            val _item: MyEntity
            val _tmpPk: String
            _tmpPk = _stmt.getText(_columnIndexOfPk)
            _item = MyEntity(_tmpPk)
            _result.add(_item)
          }
          _result
        } finally {
          _stmt.close()
        }
      }
    }
  }

  public override fun getAllIdsWithArgs(gt: Long): PagingSource<Int, MyEntity> {
    val _sql: String = "SELECT * FROM MyEntity WHERE pk > ? ORDER BY pk ASC"
    val _rawQuery: RoomRawQuery = RoomRawQuery(_sql) { _stmt ->
      var _argIndex: Int = 1
      _stmt.bindLong(_argIndex, gt)
    }
    return object : LimitOffsetPagingSource<MyEntity>(_rawQuery, __db, "MyEntity") {
      protected override suspend fun convertRows(limitOffsetQuery: RoomRawQuery, itemCount: Int): List<MyEntity> = performSuspending(__db, true, false) { _connection ->
        val _stmt: SQLiteStatement = _connection.prepare(limitOffsetQuery.sql)
        limitOffsetQuery.getBindingFunction().invoke(_stmt)
        try {
          val _columnIndexOfPk: Int = getColumnIndexOrThrow(_stmt, "pk")
          val _result: MutableList<MyEntity> = mutableListOf()
          while (_stmt.step()) {
            val _item: MyEntity
            val _tmpPk: String
            _tmpPk = _stmt.getText(_columnIndexOfPk)
            _item = MyEntity(_tmpPk)
            _result.add(_item)
          }
          _result
        } finally {
          _stmt.close()
        }
      }
    }
  }

  public override fun getAllIdsRx2(): Rxjava2RxPagingSource<Int, MyEntity> {
    val _sql: String = "SELECT pk FROM MyEntity"
    val _statement: RoomSQLiteQuery = acquire(_sql, 0)
    return object : Rxjava2LimitOffsetRxPagingSource<MyEntity>(_statement, __db, "MyEntity") {
      protected override fun convertRows(statement: SQLiteStatement): List<MyEntity> {
        val _columnIndexOfPk: Int = 0
        val _result: MutableList<MyEntity> = mutableListOf()
        while (statement.step()) {
          val _item: MyEntity
          val _tmpPk: String
          _tmpPk = statement.getText(_columnIndexOfPk)
          _item = MyEntity(_tmpPk)
          _result.add(_item)
        }
        return _result
      }
    }
  }

  public override fun getAllIdsRx3(): Rxjava3RxPagingSource<Int, MyEntity> {
    val _sql: String = "SELECT pk FROM MyEntity"
    val _statement: RoomSQLiteQuery = acquire(_sql, 0)
    return object : Rxjava3LimitOffsetRxPagingSource<MyEntity>(_statement, __db, "MyEntity") {
      protected override fun convertRows(statement: SQLiteStatement): List<MyEntity> {
        val _columnIndexOfPk: Int = 0
        val _result: MutableList<MyEntity> = mutableListOf()
        while (statement.step()) {
          val _item: MyEntity
          val _tmpPk: String
          _tmpPk = statement.getText(_columnIndexOfPk)
          _item = MyEntity(_tmpPk)
          _result.add(_item)
        }
        return _result
      }
    }
  }

  public override fun getAllIdsGuava(): ListenableFuturePagingSource<Int, MyEntity> {
    val _sql: String = "SELECT pk FROM MyEntity"
    val _statement: RoomSQLiteQuery = acquire(_sql, 0)
    return object : LimitOffsetListenableFuturePagingSource<MyEntity>(_statement, __db, "MyEntity") {
      protected override fun convertRows(statement: SQLiteStatement): List<MyEntity> {
        val _columnIndexOfPk: Int = 0
        val _result: MutableList<MyEntity> = mutableListOf()
        while (statement.step()) {
          val _item: MyEntity
          val _tmpPk: String
          _tmpPk = statement.getText(_columnIndexOfPk)
          _item = MyEntity(_tmpPk)
          _result.add(_item)
        }
        return _result
      }
    }
  }

  public companion object {
    public fun getRequiredConverters(): List<KClass<*>> = emptyList()
  }
}
