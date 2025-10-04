/*
 * Copyright (C) 2017 The Android Open Source Project
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

package androidx.room3.integration.testapp.dao;

import androidx.room3.Dao;
import androidx.room3.Insert;
import androidx.room3.OnConflictStrategy;
import androidx.room3.Query;
import androidx.room3.RoomWarnings;
import androidx.room3.integration.testapp.vo.Coordinates;
import androidx.room3.integration.testapp.vo.School;
import androidx.room3.integration.testapp.vo.SchoolRef;

import java.util.List;

@Dao
public abstract class SchoolDao {
    @Insert(onConflict = OnConflictStrategy.REPLACE)
    public abstract void insert(School... schools);

    @Query("SELECT * from School WHERE address_street LIKE '%' || :street || '%'")
    public abstract List<School> findByStreet(String street);

    @SuppressWarnings({RoomWarnings.QUERY_MISMATCH, RoomWarnings.CURSOR_MISMATCH})
    @Query("SELECT mId, mName, manager_mName FROM School")
    public abstract List<School> schoolAndManagerNames();

    @Query("SELECT mId, mName, manager_mName FROM School")
    @SuppressWarnings({RoomWarnings.QUERY_MISMATCH, RoomWarnings.CURSOR_MISMATCH})
    public abstract List<SchoolRef> schoolAndManagerNamesAsPojo();

    @Query("SELECT address_lat as lat, address_lng as lng FROM School WHERE mId = :schoolId")
    public abstract Coordinates loadCoordinates(int schoolId);

    @SuppressWarnings({RoomWarnings.QUERY_MISMATCH, RoomWarnings.CURSOR_MISMATCH})
    @Query("SELECT mId, address_lat, address_lng FROM School WHERE mId = :schoolId")
    public abstract School loadCoordinatesAsSchool(int schoolId);
}
