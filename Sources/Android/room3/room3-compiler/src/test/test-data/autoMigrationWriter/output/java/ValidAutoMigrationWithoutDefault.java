package foo.bar;

import androidx.annotation.NonNull;
import androidx.room3.migration.AutoMigrationSpec;
import androidx.room3.migration.Migration;
import androidx.sqlite.SQLite;
import androidx.sqlite.SQLiteConnection;
import java.lang.Override;
import java.lang.SuppressWarnings;
import javax.annotation.processing.Generated;

@Generated("androidx.room3.RoomProcessor")
@SuppressWarnings({"unchecked", "deprecation", "removal"})
final class MyDatabase_AutoMigration_1_2_Impl extends Migration {
    private final AutoMigrationSpec callback = new ValidAutoMigrationWithoutDefault();

    public MyDatabase_AutoMigration_1_2_Impl() {
        super(1, 2);
    }

    @Override
    public void migrate(@NonNull final SQLiteConnection connection) {
        SQLite.execSQL(connection, "ALTER TABLE `Song` ADD COLUMN `artistId` INTEGER DEFAULT NULL");
        callback.onPostMigrate(connection);
    }
}