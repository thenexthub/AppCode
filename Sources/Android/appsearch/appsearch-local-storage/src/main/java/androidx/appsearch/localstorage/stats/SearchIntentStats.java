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

import androidx.annotation.IntDef;
import androidx.annotation.RestrictTo;
import androidx.appsearch.annotation.CanIgnoreReturnValue;
import androidx.appsearch.stats.BaseStats;
import androidx.core.util.Preconditions;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

// TODO(b/319285816): link converter here.
/**
 * Class holds detailed stats of a search intent, converted from
 * {@link androidx.appsearch.app.PutDocumentsRequest#getTakenActionGenericDocuments}.
 *
 * A search intent includes a valid AppSearch search request, potentially followed by several user
 * click actions (see {@link ClickStats}) on fetched result documents. Related information of a
 * search intent will be extracted from
 * {@link androidx.appsearch.app.PutDocumentsRequest#getTakenActionGenericDocuments}.
 *
 * @exportToFramework:hide
 */
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
public final class SearchIntentStats extends BaseStats {
    /** AppSearch query correction type compared with the previous query. */
    @IntDef(value = {
            QUERY_CORRECTION_TYPE_UNKNOWN,
            QUERY_CORRECTION_TYPE_FIRST_QUERY,
            QUERY_CORRECTION_TYPE_REFINEMENT,
            QUERY_CORRECTION_TYPE_ABANDONMENT,
            QUERY_CORRECTION_TYPE_END_SESSION,
    })
    @Retention(RetentionPolicy.SOURCE)
    public @interface QueryCorrectionType {
    }

    public static final int QUERY_CORRECTION_TYPE_UNKNOWN = 0;

    public static final int QUERY_CORRECTION_TYPE_FIRST_QUERY = 1;

    public static final int QUERY_CORRECTION_TYPE_REFINEMENT = 2;

    public static final int QUERY_CORRECTION_TYPE_ABANDONMENT = 3;

    public static final int QUERY_CORRECTION_TYPE_END_SESSION = 4;

    private final @NonNull String mPackageName;

    private final @Nullable String mDatabase;

    private final @Nullable String mPrevQuery;

    private final @Nullable String mCurrQuery;

    private final long mTimestampMillis;

    private final int mNumResultsFetched;

    @QueryCorrectionType
    private final int mQueryCorrectionType;

    private final @NonNull List<ClickStats> mClicksStats;

    SearchIntentStats(@NonNull Builder builder) {
        super(builder);
        mPackageName = builder.mPackageName;
        mDatabase = builder.mDatabase;
        mPrevQuery = builder.mPrevQuery;
        mCurrQuery = builder.mCurrQuery;
        mTimestampMillis = builder.mTimestampMillis;
        mNumResultsFetched = builder.mNumResultsFetched;
        mQueryCorrectionType = builder.mQueryCorrectionType;
        mClicksStats = builder.mClicksStats;
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

    /** Returns the raw query string of the previous search intent. */
    public @Nullable String getPrevQuery() {
        return mPrevQuery;
    }

    /** Returns the raw query string of this (current) search intent. */
    public @Nullable String getCurrQuery() {
        return mCurrQuery;
    }

    /** Returns the search intent timestamp in milliseconds since Unix epoch. */
    public long getTimestampMillis() {
        return mTimestampMillis;
    }

    /**
     * Returns total number of results fetched from AppSearch by the client in this search intent.
     */
    public int getNumResultsFetched() {
        return mNumResultsFetched;
    }

    /**
     * Returns the correction type of the query in this search intent compared with the previous
     * search intent. Default value: {@link SearchIntentStats#QUERY_CORRECTION_TYPE_UNKNOWN}.
     */
    @QueryCorrectionType
    public int getQueryCorrectionType() {
        return mQueryCorrectionType;
    }

    /** Returns the list of {@link ClickStats} in this search intent. */
    public @NonNull List<ClickStats> getClicksStats() {
        return mClicksStats;
    }

    /** Builder for {@link SearchIntentStats} */
    public static final class Builder extends BaseStats.Builder<SearchIntentStats.Builder> {
        private final @NonNull String mPackageName;

        private @Nullable String mDatabase;

        private @Nullable String mPrevQuery;

        private @Nullable String mCurrQuery;

        private long mTimestampMillis;

        private int mNumResultsFetched;

        @QueryCorrectionType
        private int mQueryCorrectionType = QUERY_CORRECTION_TYPE_UNKNOWN;

        private @NonNull List<ClickStats> mClicksStats = new ArrayList<>();

        private boolean mBuilt = false;

        /** Constructor for the {@link Builder}. */
        public Builder(@NonNull String packageName) {
            mPackageName = Preconditions.checkNotNull(packageName);
        }

        /** Constructor the {@link Builder} from an existing {@link SearchIntentStats}. */
        public Builder(@NonNull SearchIntentStats searchIntentStats) {
            Preconditions.checkNotNull(searchIntentStats);

            mPackageName = searchIntentStats.getPackageName();
            mDatabase = searchIntentStats.getDatabase();
            mPrevQuery = searchIntentStats.getPrevQuery();
            mCurrQuery = searchIntentStats.getCurrQuery();
            mTimestampMillis = searchIntentStats.getTimestampMillis();
            mNumResultsFetched = searchIntentStats.getNumResultsFetched();
            mQueryCorrectionType = searchIntentStats.getQueryCorrectionType();
            mClicksStats.addAll(searchIntentStats.getClicksStats());
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

        /** Sets the raw query string of the previous search intent. */
        @CanIgnoreReturnValue
        public @NonNull Builder setPrevQuery(@Nullable String prevQuery) {
            resetIfBuilt();
            mPrevQuery = prevQuery;
            return this;
        }

        /** Sets the raw query string of this (current) search intent. */
        @CanIgnoreReturnValue
        public @NonNull Builder setCurrQuery(@Nullable String currQuery) {
            resetIfBuilt();
            mCurrQuery = currQuery;
            return this;
        }

        /** Sets the search intent timestamp in milliseconds since Unix epoch. */
        @CanIgnoreReturnValue
        public @NonNull Builder setTimestampMillis(long timestampMillis) {
            resetIfBuilt();
            mTimestampMillis = timestampMillis;
            return this;
        }

        /**
         * Sets total number of results fetched from AppSearch by the client in this search intent.
         */
        @CanIgnoreReturnValue
        public @NonNull Builder setNumResultsFetched(int numResultsFetched) {
            resetIfBuilt();
            mNumResultsFetched = numResultsFetched;
            return this;
        }

        /**
         * Sets the correction type of the query in this search intent compared with the previous
         * search intent.
         */
        @CanIgnoreReturnValue
        public @NonNull Builder setQueryCorrectionType(
                @QueryCorrectionType int queryCorrectionType) {
            resetIfBuilt();
            mQueryCorrectionType = queryCorrectionType;
            return this;
        }

        /** Adds one or more {@link ClickStats} objects to this search intent. */
        @CanIgnoreReturnValue
        public @NonNull Builder addClicksStats(ClickStats @NonNull ... clicksStats) {
            Preconditions.checkNotNull(clicksStats);
            resetIfBuilt();
            return addClicksStats(Arrays.asList(clicksStats));
        }

        /** Adds a collection of {@link ClickStats} objects to this search intent. */
        @CanIgnoreReturnValue
        public @NonNull Builder addClicksStats(
                @NonNull Collection<? extends ClickStats> clicksStats) {
            Preconditions.checkNotNull(clicksStats);
            resetIfBuilt();
            mClicksStats.addAll(clicksStats);
            return this;
        }

        /**
         * If built, make a copy of previous data for every field so that the builder can be reused.
         */
        private void resetIfBuilt() {
            if (mBuilt) {
                mClicksStats = new ArrayList<>(mClicksStats);
                mBuilt = false;
            }
        }

        /** Builds a new {@link SearchIntentStats} from the {@link Builder}. */
        @Override
        public @NonNull SearchIntentStats build() {
            mBuilt = true;
            return new SearchIntentStats(/* builder= */ this);
        }
    }
}
