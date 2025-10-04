import androidx.room3.RoomDatabase
import androidx.room3.util.getColumnIndexOrThrow
import androidx.room3.util.performBlocking
import androidx.sqlite.SQLiteStatement
import com.google.common.base.Optional
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

  public override fun queryOfOptional(): Optional<MyEntity> {
    val _sql: String = "SELECT * FROM MyEntity"
    return performBlocking(__db, true, false) { _connection ->
      val _stmt: SQLiteStatement = _connection.prepare(_sql)
      try {
        val _columnIndexOfPk: Int = getColumnIndexOrThrow(_stmt, "pk")
        val _columnIndexOfOther: Int = getColumnIndexOrThrow(_stmt, "other")
        val _value: MyEntity?
        if (_stmt.step()) {
          val _tmpPk: Int
          _tmpPk = _stmt.getLong(_columnIndexOfPk).toInt()
          val _tmpOther: String
          _tmpOther = _stmt.getText(_columnIndexOfOther)
          _value = MyEntity(_tmpPk,_tmpOther)
        } else {
          _value = null
        }
        val _result: Optional<MyEntity> = Optional.fromNullable(_value)
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
