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

package androidx.appsearch.localstorage.stats;

import androidx.annotation.RestrictTo;
import androidx.appsearch.annotation.CanIgnoreReturnValue;
import androidx.appsearch.stats.BaseStats;
import androidx.core.util.Preconditions;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

// TODO(b/319285816): link converter here.
/**
 * Class holds detailed stats of a search session, converted from {@link
 * androidx.appsearch.app.PutDocumentsRequest#getTakenActionGenericDocuments}. It contains a list of
 * {@link SearchIntentStats} and aggregated metrics of them.
 *
 * <p>A search session is consist of a sequence of related search intents. See {@link
 * SearchIntentStats} for more details.
 *
 * @exportToFramework:hide
 */
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
public final class SearchSessionStats extends BaseStats {
    private final @NonNull String mPackageName;

    private final @Nullable String mDatabase;

    private final @NonNull List<SearchIntentStats> mSearchIntentsStats;

    SearchSessionStats(@NonNull Builder builder) {
        super(builder);
        mPackageName = builder.mPackageName;
        mDatabase = builder.mDatabase;
        mSearchIntentsStats = builder.mSearchIntentsStats;
    }

    /**
     * Returns a nullable {@link SearchIntentStats} instance containing information of the last
     * search intent which ended the search session.
     *
     * <p>If {@link #getSearchIntentsStats} is empty (i.e. the caller didn't add any {@link
     * SearchIntentStats} via {@link Builder#addSearchIntentsStats}), then return null.
     *
     * <p>It is similar to the last element in {@link #getSearchIntentsStats}, except there is no
     * previous query and the query correction type is tagged as {@link
     * SearchIntentStats#QUERY_CORRECTION_TYPE_END_SESSION}.
     *
     * <p>This stats is useful to determine whether the user ended the search session with
     * satisfaction (i.e. had found desired result documents) or not.
     */
    public @Nullable SearchIntentStats getEndSessionSearchIntentStats() {
        if (mSearchIntentsStats.isEmpty()) {
            return null;
        }

        SearchIntentStats lastSearchIntentStats =
                mSearchIntentsStats.get(mSearchIntentsStats.size() - 1);
        return new SearchIntentStats.Builder(lastSearchIntentStats)
                .setPrevQuery(null)
                .setQueryCorrectionType(SearchIntentStats.QUERY_CORRECTION_TYPE_END_SESSION)
                .build();
    }

    /** Returns calling package name. */
    public @NonNull String getPackageName() {
        return mPackageName;
    }

    /**
     * Returns calling database name.
     *
     * <p>For global search, database name will be null.
     */
    public @Nullable String getDatabase() {
        return mDatabase;
    }

    /** Returns the list of {@link SearchIntentStats} in this search session. */
    public @NonNull List<SearchIntentStats> getSearchIntentsStats() {
        return mSearchIntentsStats;
    }

    /** Builder for {@link SearchSessionStats}. */
    public static final class Builder extends BaseStats.Builder<SearchSessionStats.Builder> {
        private final @NonNull String mPackageName;

        private @Nullable String mDatabase;

        private @NonNull List<SearchIntentStats> mSearchIntentsStats = new ArrayList<>();

        private boolean mBuilt = false;

        /** Constructor for the {@link Builder}. */
        public Builder(@NonNull String packageName) {
            mPackageName = Preconditions.checkNotNull(packageName);
        }

        /**
         * Sets calling database name.
         *
         * <p>For global search, database name will be null.
         */
        @CanIgnoreReturnValue
        public @NonNull Builder setDatabase(@Nullable String database) {
            resetIfBuilt();
            mDatabase = database;
            return this;
        }

        /** Adds one or more {@link SearchIntentStats} objects to this search intent. */
        @CanIgnoreReturnValue
        public @NonNull Builder addSearchIntentsStats(
                SearchIntentStats @NonNull ... searchIntentsStats) {
            Preconditions.checkNotNull(searchIntentsStats);
            resetIfBuilt();
            return addSearchIntentsStats(Arrays.asList(searchIntentsStats));
        }

        /** Adds a collection of {@link SearchIntentStats} objects to this search intent. */
        @CanIgnoreReturnValue
        public @NonNull Builder addSearchIntentsStats(
                @NonNull Collection<? extends SearchIntentStats> searchIntentsStats) {
            Preconditions.checkNotNull(searchIntentsStats);
            resetIfBuilt();
            mSearchIntentsStats.addAll(searchIntentsStats);
            return this;
        }

        /**
         * If built, make a copy of previous data for every field so that the builder can be reused.
         */
        private void resetIfBuilt() {
            if (mBuilt) {
                mSearchIntentsStats = new ArrayList<>(mSearchIntentsStats);
                mBuilt = false;
            }
        }

        /** Builds a new {@link SearchSessionStats} from the {@link Builder}. */
        @Override
        public @NonNull SearchSessionStats build() {
            mBuilt = true;
            return new SearchSessionStats(/* builder= */ this);
        }
    }
}
