import androidx.room3.RoomDatabase
import androidx.room3.util.getColumnIndexOrThrow
import androidx.room3.util.performBlocking
import androidx.sqlite.SQLiteStatement
import com.google.common.collect.ImmutableList
import javax.`annotation`.processing.Generated
import kotlin.Int
import kotlin.String
import kotlin.Suppress
import kotlin.collections.List
import kotlin.reflect.KClass

@Generated(value = ["androidx.room3.RoomProcessor"])
@Suppress(names = ["UNCHECKED_CAST", "DEPRECATION", "REDUNDANT_PROJECTION", "REMOVAL"])
internal class MyDao_Impl(
  __db: RoomDatabase,
) : MyDao {
  private val __db: RoomDatabase
  init {
    this.__db = __db
  }

  public override fun queryOfList(): ImmutableList<MyEntity> {
    val _sql: String = "SELECT * FROM MyEntity"
    return performBlocking(__db, true, false) { _connection ->
      val _stmt: SQLiteStatement = _connection.prepare(_sql)
      try {
        val _columnIndexOfPk: Int = getColumnIndexOrThrow(_stmt, "pk")
        val _columnIndexOfOther: Int = getColumnIndexOrThrow(_stmt, "other")
        val _immutableListBuilder: ImmutableList.Builder<MyEntity> = ImmutableList.builder()
        while (_stmt.step()) {
          val _item: MyEntity
          val _tmpPk: Int
          _tmpPk = _stmt.getLong(_columnIndexOfPk).toInt()
          val _tmpOther: String
          _tmpOther = _stmt.getText(_columnIndexOfOther)
          _item = MyEntity(_tmpPk,_tmpOther)
          _immutableListBuilder.add(_item)
        }
        val _result: ImmutableList<MyEntity> = _immutableListBuilder.build()
        _result
      } finally {
        _stmt.close()
      }
    }
  }

  public companion object {
    public fun getRequiredConverters(): List<KClass<*>> = emptyList()
  }
}
