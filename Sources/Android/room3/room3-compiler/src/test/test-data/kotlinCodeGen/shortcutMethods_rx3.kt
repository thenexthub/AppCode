import androidx.room3.EntityDeleteOrUpdateAdapter
import androidx.room3.EntityInsertAdapter
import androidx.room3.EntityUpsertAdapter
import androidx.room3.RoomDatabase
import androidx.room3.rxjava3.createCompletable
import androidx.room3.rxjava3.createSingle
import androidx.sqlite.SQLiteStatement
import io.reactivex.rxjava3.core.Completable
import io.reactivex.rxjava3.core.Single
import javax.`annotation`.processing.Generated
import kotlin.Int
import kotlin.Long
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

  private val __insertAdapterOfMyEntity: EntityInsertAdapter<MyEntity>

  private val __deleteAdapterOfMyEntity: EntityDeleteOrUpdateAdapter<MyEntity>

  private val __updateAdapterOfMyEntity: EntityDeleteOrUpdateAdapter<MyEntity>

  private val __upsertAdapterOfMyEntity: EntityUpsertAdapter<MyEntity>
  init {
    this.__db = __db
    this.__insertAdapterOfMyEntity = object : EntityInsertAdapter<MyEntity>() {
      protected override fun createQuery(): String = "INSERT OR ABORT INTO `MyEntity` (`pk`,`other`) VALUES (?,?)"

      protected override fun bind(statement: SQLiteStatement, entity: MyEntity) {
        statement.bindLong(1, entity.pk.toLong())
        statement.bindText(2, entity.other)
      }
    }
    this.__deleteAdapterOfMyEntity = object : EntityDeleteOrUpdateAdapter<MyEntity>() {
      protected override fun createQuery(): String = "DELETE FROM `MyEntity` WHERE `pk` = ?"

      protected override fun bind(statement: SQLiteStatement, entity: MyEntity) {
        statement.bindLong(1, entity.pk.toLong())
      }
    }
    this.__updateAdapterOfMyEntity = object : EntityDeleteOrUpdateAdapter<MyEntity>() {
      protected override fun createQuery(): String = "UPDATE OR ABORT `MyEntity` SET `pk` = ?,`other` = ? WHERE `pk` = ?"

      protected override fun bind(statement: SQLiteStatement, entity: MyEntity) {
        statement.bindLong(1, entity.pk.toLong())
        statement.bindText(2, entity.other)
        statement.bindLong(3, entity.pk.toLong())
      }
    }
    this.__upsertAdapterOfMyEntity = EntityUpsertAdapter<MyEntity>(object : EntityInsertAdapter<MyEntity>() {
      protected override fun createQuery(): String = "INSERT INTO `MyEntity` (`pk`,`other`) VALUES (?,?)"

      protected override fun bind(statement: SQLiteStatement, entity: MyEntity) {
        statement.bindLong(1, entity.pk.toLong())
        statement.bindText(2, entity.other)
      }
    }, object : EntityDeleteOrUpdateAdapter<MyEntity>() {
      protected override fun createQuery(): String = "UPDATE `MyEntity` SET `pk` = ?,`other` = ? WHERE `pk` = ?"

      protected override fun bind(statement: SQLiteStatement, entity: MyEntity) {
        statement.bindLong(1, entity.pk.toLong())
        statement.bindText(2, entity.other)
        statement.bindLong(3, entity.pk.toLong())
      }
    })
  }

  public override fun insertSingle(vararg entities: MyEntity): Single<List<Long>> = createSingle(__db, false, true) { _connection ->
    val _result: List<Long>? = __insertAdapterOfMyEntity.insertAndReturnIdsList(_connection, entities)
    _result
  }

  public override fun insertCompletable(vararg entities: MyEntity): Completable = createCompletable(__db, false, true) { _connection ->
    __insertAdapterOfMyEntity.insert(_connection, entities)
  }

  public override fun deleteSingle(entity: MyEntity): Single<Int> = createSingle(__db, false, true) { _connection ->
    var _result: Int = 0
    _result += __deleteAdapterOfMyEntity.handle(_connection, entity)
    _result
  }

  public override fun deleteCompletable(entity: MyEntity): Completable = createCompletable(__db, false, true) { _connection ->
    __deleteAdapterOfMyEntity.handle(_connection, entity)
  }

  public override fun updateSingle(entity: MyEntity): Single<Int> = createSingle(__db, false, true) { _connection ->
    var _result: Int = 0
    _result += __updateAdapterOfMyEntity.handle(_connection, entity)
    _result
  }

  public override fun updateCompletable(entity: MyEntity): Completable = createCompletable(__db, false, true) { _connection ->
    __updateAdapterOfMyEntity.handle(_connection, entity)
  }

  public override fun upsertSingle(vararg entities: MyEntity): Single<List<Long>> = createSingle(__db, false, true) { _connection ->
    val _result: List<Long>? = __upsertAdapterOfMyEntity.upsertAndReturnIdsList(_connection, entities)
    _result
  }

  public override fun upsertCompletable(vararg entities: MyEntity): Completable = createCompletable(__db, false, true) { _connection ->
    __upsertAdapterOfMyEntity.upsert(_connection, entities)
  }

  public companion object {
    public fun getRequiredConverters(): List<KClass<*>> = emptyList()
  }
}
