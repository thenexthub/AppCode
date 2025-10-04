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

package androidx.room3.integration.testapp.test;

import static com.google.common.truth.Truth.assertThat;

import android.content.Context;

import androidx.room3.Dao;
import androidx.room3.Database;
import androidx.room3.Entity;
import androidx.room3.Insert;
import androidx.room3.PrimaryKey;
import androidx.room3.Query;
import androidx.room3.Room;
import androidx.room3.RoomDatabase;
import androidx.test.core.app.ApplicationProvider;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.filters.SmallTest;

import org.jspecify.annotations.NonNull;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.List;

@SmallTest
@RunWith(AndroidJUnit4.class)
public class RecordPojoTest {

    @Ignore("b/445473504")
    @Test
    public void recordEntity() {
        Context context = ApplicationProvider.getApplicationContext();
        TestDatabase db = Room.inMemoryDatabaseBuilder(context, TestDatabase.class).build();
        RecordEntityDao recordDao = db.getDao();
        recordDao.insert(new RecordEntity(1, "I am a RECORD"));
        List<RecordEntity> result = recordDao.getAll();
        assertThat(result.size()).isEqualTo(1);
        assertThat(result.get(0).data()).isEqualTo("I am a RECORD");
    }

    @Entity
    record RecordEntity(
            @PrimaryKey long id,
            String data
    ) {}

    abstract static class TestDatabase  extends RoomDatabase {
        abstract RecordEntityDao getDao();
    }

    @Dao
    interface RecordEntityDao {
        @Query("SELECT * FROM RecordEntity")
        @NonNull List<RecordEntity> getAll();

        @Insert
        void insert(@NonNull RecordEntity item);
    }

}
