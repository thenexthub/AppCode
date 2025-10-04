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

package androidx.car.app.model;

import static androidx.annotation.RestrictTo.Scope.LIBRARY;

import androidx.annotation.RestrictTo;
import androidx.car.app.annotations.CarProtocol;
import androidx.car.app.annotations.ExperimentalCarApi;
import androidx.car.app.annotations.KeepFields;
import androidx.car.app.model.ItemList.OnItemVisibilityChangedListener;
import androidx.car.app.model.constraints.CarTextConstraints;
import androidx.car.app.serialization.ListDelegate;
import androidx.car.app.serialization.ListDelegateImpl;

import com.google.errorprone.annotations.CanIgnoreReturnValue;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

/**
 * The generic interface for a single section within a sectioned item template. Sections only allow
 * a single type of {@link Item} to be added.
 *
 * @param <T> the type of {@link Item} that's allowed to be added to this section
 */
@KeepFields
@CarProtocol
@ExperimentalCarApi
public abstract class Section<T extends Item> {
    private final @NonNull ListDelegate<T> mItemsDelegate;
    private final @Nullable CarText mTitle;
    private final @Nullable CarText mNoItemsMessage;
    private final @Nullable OnItemVisibilityChangedDelegate mOnItemVisibilityChangedDelegate;

    // Empty constructor for serialization
    protected Section() {
        mItemsDelegate = new ListDelegateImpl<>(Collections.emptyList());
        mTitle = null;
        mNoItemsMessage = null;
        mOnItemVisibilityChangedDelegate = null;
    }

    /** Constructor that fills out fields from any section builder. */
    protected Section(@NonNull BaseBuilder<T, ?> builder) {
        mItemsDelegate = new ListDelegateImpl<>(Collections.unmodifiableList(builder.mItems));
        mTitle = builder.mHeader;
        mNoItemsMessage = builder.mNoItemsMessage;
        mOnItemVisibilityChangedDelegate = builder.mOnItemVisibilityChangedDelegate;
    }

    /** Returns the items added to this section. */
    public @NonNull ListDelegate<T> getItemsDelegate() {
        return mItemsDelegate;
    }

    /** Returns the optional text that should appear with the items in this section. */
    public @Nullable CarText getTitle() {
        return mTitle;
    }

    /**
     * Returns the optional message that should appear if there are no items added to this section.
     */
    public @Nullable CarText getNoItemsMessage() {
        return mNoItemsMessage;
    }

    /**
     * Returns the {@link OnItemVisibilityChangedDelegate} to be called when the visible items in
     * this {@link Section} changes, or {@code null} if one isn't set.
     */
    public @Nullable OnItemVisibilityChangedDelegate getOnItemVisibilityChangedDelegate() {
        return mOnItemVisibilityChangedDelegate;
    }

    @Override
    public boolean equals(@Nullable Object other) {
        if (other == null) {
            return false;
        }
        if (!(other instanceof Section)) {
            return false;
        }
        Section<?> section = (Section<?>) other;

        return Objects.equals(mItemsDelegate, section.mItemsDelegate)
                && Objects.equals(mTitle, section.mTitle)
                && Objects.equals(mNoItemsMessage, section.mNoItemsMessage)
                && Objects.equals(mOnItemVisibilityChangedDelegate == null,
                section.mOnItemVisibilityChangedDelegate == null);
    }

    @Override
    public int hashCode() {
        return Objects.hash(
                mItemsDelegate, mTitle, mNoItemsMessage, mOnItemVisibilityChangedDelegate == null);
    }

    @Override
    public @NonNull String toString() {
        return "Section { title: " + mTitle
                + ", noItemsMessage: " + mNoItemsMessage
                + ", itemsDelegate: " + mItemsDelegate
                + ", onItemVisibilityChangedDelegate: "
                + (mOnItemVisibilityChangedDelegate != null);
    }

    /**
     * Generic {@link Section} builder that contains the fields that all sections share.
     *
     * @param <T> The {@link Item} type that this section contains
     * @param <B> The builder type to return for the builder methods
     */
    @SuppressWarnings({"StaticFinalBuilder", "MissingBuildMethod"})
    protected abstract static class BaseBuilder<T extends Item, B> {
        @NonNull List<T> mItems = new ArrayList<>();
        @Nullable CarText mHeader;
        @Nullable CarText mNoItemsMessage;
        @Nullable OnItemVisibilityChangedDelegate mOnItemVisibilityChangedDelegate;

        protected BaseBuilder() {
        }

        /**
         * Sets the {@link OnItemVisibilityChangedListener} to call when the visible items in this
         * {@link Section} changes.
         *
         * <p>Note that the listener relates to UI events and will be executed on the main thread
         * using {@code Looper#getMainLooper()}.
         *
         * <p>It's possible for more than 1 {@link Section} to be visible on the screen at the same
         * time, in which case, every visible Section's {@link OnItemVisibilityChangedListener}
         * will be triggered with their respective visible items.
         *
         * <p>Passing {@code null} will clear the {@link OnItemVisibilityChangedListener}.
         */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked", "ExecutorRegistration"})
        public @NonNull B setOnItemVisibilityChangedListener(
                @Nullable OnItemVisibilityChangedListener onItemVisibilityChangedListener) {
            if (onItemVisibilityChangedListener == null) {
                mOnItemVisibilityChangedDelegate = null;
            } else {
                mOnItemVisibilityChangedDelegate =
                        OnItemVisibilityChangedDelegateImpl.create(onItemVisibilityChangedListener);
            }
            return (B) this;
        }

        /**
         * Use {@link #setOnItemVisibilityChangedListener(OnItemVisibilityChangedListener)} instead.
         */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        @RestrictTo(LIBRARY)
        public @NonNull B setOnItemVisibilityChangedDelegate(
                @Nullable OnItemVisibilityChangedDelegate onItemVisibilityChangedDelegate) {
            mOnItemVisibilityChangedDelegate = onItemVisibilityChangedDelegate;
            return (B) this;
        }


        /** Sets the items for this section, overwriting any other previously set items. */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        public @NonNull B setItems(@NonNull List<T> items) {
            mItems = items;
            return (B) this;
        }

        /** Adds an item to this section, appending to the existing list of items. */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        public @NonNull B addItem(@NonNull T item) {
            mItems.add(item);
            return (B) this;
        }

        /** Delete all items in this section. */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        public @NonNull B clearItems() {
            mItems.clear();
            return (B) this;
        }

        /**
         * Sets or clears the optional title that appears above the items in this section. If not
         * set, no title shows up. The title must conform to {@link CarTextConstraints#TEXT_ONLY}
         * constraints.
         */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        public @NonNull B setTitle(@Nullable CharSequence title) {
            if (title == null) {
                mHeader = null;
                return (B) this;
            }

            CarText carText = CarText.create(title);
            CarTextConstraints.TEXT_ONLY.validateOrThrow(carText);
            mHeader = carText;
            return (B) this;
        }

        /**
         * Sets or clears the optional title that appears above the items in this section. If not
         * set, no title shows up. The title must conform to {@link CarTextConstraints#TEXT_ONLY}
         * constraints.
         */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        public @NonNull B setTitle(@Nullable CarText title) {
            if (title != null) {
                CarTextConstraints.TEXT_ONLY.validateOrThrow(title);
            }
            mHeader = title;
            return (B) this;
        }

        /**
         * Sets or clears the optional message to display in this section when there are 0 items
         * added to it. If not set, this section will not show any message when there are 0 items.
         * The message must conform to {@link CarTextConstraints#TEXT_ONLY} constraints.
         */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        public @NonNull B setNoItemsMessage(@Nullable CharSequence noItemsMessage) {
            if (noItemsMessage == null) {
                mNoItemsMessage = null;
                return (B) this;
            }

            CarText carText = CarText.create(noItemsMessage);
            CarTextConstraints.TEXT_ONLY.validateOrThrow(carText);
            mNoItemsMessage = carText;
            return (B) this;
        }

        /**
         * Sets or clears the optional message to display in this section when there are 0 items
         * added to it. If not set, this section will not show any message when there are 0 items.
         * The message must conform to {@link CarTextConstraints#TEXT_ONLY} constraints.
         */
        @CanIgnoreReturnValue
        @SuppressWarnings({"SetterReturnsThis", "unchecked"})
        public @NonNull B setNoItemsMessage(@Nullable CarText noItemsMessage) {
            if (noItemsMessage != null) {
                CarTextConstraints.TEXT_ONLY.validateOrThrow(noItemsMessage);
            }
            mNoItemsMessage = noItemsMessage;
            return (B) this;
        }
    }
}
