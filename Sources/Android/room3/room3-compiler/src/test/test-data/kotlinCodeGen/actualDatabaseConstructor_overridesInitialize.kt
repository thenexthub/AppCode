import androidx.room3.RoomDatabaseConstructor

public actual object MyDatabaseCtor : RoomDatabaseConstructor<MyDatabase> {
    actual override fun initialize(): MyDatabase = MyDatabase_Impl()
}