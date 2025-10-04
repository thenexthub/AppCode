/*
 * Copyright 2023 The Android Open Source Project
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

package androidx.pdf.find;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Parcelable;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import androidx.annotation.RestrictTo;
import androidx.core.os.BundleCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.pdf.R;
import androidx.pdf.models.MatchRects;
import androidx.pdf.util.Accessibility;
import androidx.pdf.util.CycleRange;
import androidx.pdf.util.ObservableValue;
import androidx.pdf.util.ObservableValue.ValueObserver;
import androidx.pdf.viewer.ImmersiveModeRequester;
import androidx.pdf.viewer.PaginatedView;
import androidx.pdf.viewer.SearchModel;
import androidx.pdf.viewer.SelectedMatch;
import androidx.pdf.viewer.loader.PdfLoader;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

import java.util.Objects;

/**
 * A View that has a search query box, find-next and find-previous button, useful for finding
 * matches in a file. This does not use the real platform SearchView because it cannot be styled to
 * remove the search icon and underline.
 */
@RestrictTo(RestrictTo.Scope.LIBRARY)
public class FindInFileView extends LinearLayout {
    private static final char MATCH_STATUS_COUNTING = '\u2026';
    private static final String KEY_SUPER = "super";
    private static final String KEY_FOCUS = "focus";
    private static final String KEY_IS_SAVED = "is_saved";
    private static final String KEY_MATCH_RECTS = "match_rects";
    private static final String KEY_SELECTED_PAGE = "selected_page";
    private static final String KEY_SELECTED_INDEX = "selected_index";

    private TextView mQueryBox;
    private ImageView mPrevButton;
    private ImageView mNextButton;
    private TextView mMatchStatus;
    private View mCloseButton;
    private FloatingActionButton mAnnotationButton;
    private PaginatedView mPaginatedView;
    private ImmersiveModeRequester mImmersiveModeRequester;

    private FindInFileListener mFindInFileListener;
    private Runnable mOnClosedButtonCallback;

    private SearchModel mSearchModel;
    private ObservableValue<MatchCount> mMatchCount;

    private boolean mIsAnnotationIntentResolvable;
    private boolean mIsRestoring;
    private boolean mFocus;
    private int mViewingPage;
    private int mSelectedMatch;
    private MatchRects mMatches;

    private OnVisibilityChangedListener mOnVisibilityChangedListener;

    private final OnClickListener mOnClickListener = new OnClickListener() {
        @Override
        public void onClick(View v) {
            if (v == mPrevButton || v == mNextButton) {
                mQueryBox.clearFocus();
                if (mFindInFileListener != null) {
                    boolean mBackwards = (v == mPrevButton);
                    mFindInFileListener.onFindNextMatch(mQueryBox.getText().toString(), mBackwards);
                }
            } else if (v == mCloseButton) {
                resetFindInFile();
                if (mIsAnnotationIntentResolvable) {
                    mImmersiveModeRequester.requestImmersiveModeChange(false);
                }
            }
        }
    };

    private final FindInFileListener mFindInFileListenerSetter = new FindInFileListener() {
        @Override
        public boolean onQueryTextChange(@Nullable String query) {
            if (mSearchModel != null && mPaginatedView != null) {
                mSearchModel.setQuery(query, getViewingPage());
                return true;
            }
            return false;
        }

        @Override
        public boolean onFindNextMatch(String query, boolean backwards) {
            if (mSearchModel != null) {
                CycleRange.Direction direction;
                if (backwards) {
                    direction = CycleRange.Direction.BACKWARDS;
                } else {
                    direction = CycleRange.Direction.FORWARDS;
                }
                mSearchModel.selectNextMatch(direction,
                        mPaginatedView.getPageRangeHandler().getVisiblePage());
                return true;
            }
            return false;
        }

        @Override
        public @Nullable ObservableValue<MatchCount> matchCount() {
            return mSearchModel != null ? mSearchModel.matchCount() : null;
        }
    };

    private final ValueObserver<MatchCount> mMatchCountObserver = new ValueObserver<MatchCount>() {
        @Override
        public void onChange(MatchCount oldMatchCount, MatchCount newMatchCount) {
            if (newMatchCount == null) {
                mMatchStatus.setFocusableInTouchMode(false);
                mMatchStatus.setText("");
            } else {
                String matchStatusText = getContext().getString(R.string.message_match_status,
                        newMatchCount.mSelectedIndex + 1,
                        // Zero-based - change to one-based for user.
                        newMatchCount.mTotalMatches);

                String matchStatusDescription =
                        getContext().getString(R.string.match_status_description,
                        newMatchCount.mSelectedIndex + 1,
                        //Zero-based - change to one-based for user.
                        newMatchCount.mTotalMatches);

                if (newMatchCount.mIsAllPagesCounted) {
                    if (newMatchCount.mSelectedIndex >= 0) {
                        Accessibility.get().announce(getContext(), FindInFileView.this,
                                matchStatusDescription);
                    }
                    else {
                        Accessibility.get().announce(getContext(), FindInFileView.this,
                                R.string.message_no_match_status);
                    }
                } else {
                    matchStatusText += MATCH_STATUS_COUNTING;  // Not yet all counted, use ellipses.
                }
                mMatchStatus.setText(matchStatusText);
                mMatchStatus.setFocusableInTouchMode(true);
            }
        }
    };

    private final TextWatcher mOnQueryTextListener = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
        }

        @Override
        public void afterTextChanged(Editable s) {
            if (mFindInFileListener != null) {
                mFindInFileListener.onQueryTextChange(s.toString());
            }

            // Enable next/prev button
            if (!TextUtils.isGraphic(s)) {
                mMatchStatus.setVisibility(GONE);
            } else {
                mMatchStatus.setVisibility(VISIBLE);
            }
        }
    };

    private final OnEditorActionListener mOnActionListener = new OnEditorActionListener() {
        @Override
        public boolean onEditorAction(TextView textView, int actionId, KeyEvent event) {
            if (actionId == EditorInfo.IME_ACTION_SEARCH) {
                mQueryBox.clearFocus();
                if (mFindInFileListener != null) {
                    return mFindInFileListener.onFindNextMatch(textView.getText().toString(),
                            false);
                }
            }
            return false;
        }
    };

    /**
     *  Listener interface for receiving FindInFile visibility change events.
     */
    public interface OnVisibilityChangedListener {
        /**
         * Called when the visibility state changes.
         */
        void onVisibilityChanged(boolean isVisible);
    }

    public void setOnVisibilityChangedListener(@Nullable OnVisibilityChangedListener listener) {
        this.mOnVisibilityChangedListener = listener;
    }

    public FindInFileView(@NonNull Context context) {
        this(context, null);
    }

    public FindInFileView(@NonNull Context context, @NonNull AttributeSet attrs) {
        super(context, attrs);
        LayoutInflater.from(context).inflate(R.layout.find_in_file, this, true);

        // Init UI Elements
        mQueryBox = (TextView) findViewById(R.id.find_query_box);
        mPrevButton = findViewById(R.id.find_prev_btn);
        mNextButton = findViewById(R.id.find_next_btn);
        mMatchStatus = (TextView) findViewById(R.id.match_status_textview);
        mCloseButton = findViewById(R.id.close_btn);

        // Set Listeners
        mQueryBox.addTextChangedListener(mOnQueryTextListener);
        mQueryBox.setOnEditorActionListener(mOnActionListener);
        mPrevButton.setOnClickListener(mOnClickListener);
        mNextButton.setOnClickListener(mOnClickListener);
        mCloseButton.setOnClickListener(mOnClickListener);

        // Set Focus In Touch Mode
        setFocusInTouchMode();
    }

    @Override
    protected void onVisibilityChanged(@NonNull View changedView, int visibility) {
        super.onVisibilityChanged(changedView, visibility);


        if (changedView == this && mOnVisibilityChangedListener != null) {
            mOnVisibilityChangedListener.onVisibilityChanged(visibility == View.VISIBLE);
        }
    }

    @Override
    protected @NonNull Parcelable onSaveInstanceState() {
        Bundle bundle = new Bundle();
        bundle.putParcelable(KEY_SUPER, super.onSaveInstanceState());
        // Save TextView Focus State
        bundle.putBoolean(KEY_FOCUS, mQueryBox.hasFocus());
        // Save SearchModel State
        if (mSearchModel != null && mSearchModel.selectedMatch().get() != null) {
            bundle.putBoolean(KEY_IS_SAVED, true);
            bundle.putParcelable(KEY_MATCH_RECTS, Objects.requireNonNull(
                    mSearchModel.selectedMatch().get()).getPageMatches());
            bundle.putInt(KEY_SELECTED_PAGE, mSearchModel.getSelectedPage());
            bundle.putInt(KEY_SELECTED_INDEX,
                    Objects.requireNonNull(mSearchModel.selectedMatch().get()).getSelected());
        }
        return bundle;
    }

    @Override
    protected void onRestoreInstanceState(Parcelable state) {
        Bundle bundle = (Bundle) state;
        super.onRestoreInstanceState(
                BundleCompat.getParcelable(bundle, KEY_SUPER, Parcelable.class));
        if (bundle.getBoolean(KEY_IS_SAVED)) {
            mIsRestoring = true;
            mSelectedMatch = bundle.getInt(KEY_SELECTED_INDEX);
            mViewingPage = bundle.getInt(KEY_SELECTED_PAGE);
            mMatches = BundleCompat.getParcelable(bundle, KEY_MATCH_RECTS, MatchRects.class);
        }
    }

    /**
     * Handles touch events and prevents further propagation
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return true;
    }

    /**
     * Sets the pdfLoader and create a new {@link SearchModel} instance with the given pdfLoader.
     */
    public void setPdfLoader(@NonNull PdfLoader pdfLoader) {
        mSearchModel = new SearchModel(pdfLoader);
    }

    public void setPaginatedView(@NonNull PaginatedView paginatedView) {
        mPaginatedView = paginatedView;
    }

    public void setOnClosedButtonCallback(@NonNull Runnable onClosedButtonCallback) {
        this.mOnClosedButtonCallback = onClosedButtonCallback;
    }

    public @NonNull SearchModel getSearchModel() {
        return mSearchModel;
    }

    public void setAnnotationButton(
            @NonNull FloatingActionButton annotationButton,
            @NonNull ImmersiveModeRequester immersiveModeRequester) {
        mAnnotationButton = annotationButton;
        mImmersiveModeRequester = immersiveModeRequester;
    }

    public void setAnnotationIntentResolvable(
            boolean isAnnotationIntentResolvable) {
        mIsAnnotationIntentResolvable = isAnnotationIntentResolvable;
    }

    /**
     * Sets the visibility of the find-in-file view and configures its behavior.
     *
     * @param visibility true to show the find-in-file view, false to hide it.
     */
    public void setFindInFileView(boolean visibility) {
        if (mSearchModel == null) {
            return; // Ignore call. Models not initialized yet
        }
        if (visibility) {
            this.setVisibility(VISIBLE);
            if (mAnnotationButton != null && mAnnotationButton.getVisibility() == VISIBLE) {
                mImmersiveModeRequester.requestImmersiveModeChange(true);
            }
            // We set the FIF listener after the document loads
            // to prevent incomplete search results.
            setFindInFileListener();
            setMatchStatus();
            // Requests the keyboard based on the focus flag
            if (mFocus) {
                queryBoxRequestFocus();
            }
            // Restores search model select state
            if (mIsRestoring) {
                restoreSelectedMatch();
            }
        } else {
            this.setVisibility(GONE);
        }
    }

    /**
     * Resets the visibility of the FindInFileView and resets the search query
     */
    public void resetFindInFile() {
        mOnClosedButtonCallback.run();
        this.setVisibility(GONE);
        mQueryBox.clearFocus();
        mQueryBox.setText("");
        mFocus = true;
        mIsRestoring = false;
    }

    private void restoreSelectedMatch() {
        // If the first match is selected, no need to restore since it will be reselected by default
        if (mSelectedMatch > 0) {
            mSearchModel.setSelectedMatch(
                    new SelectedMatch(mSearchModel.query().get(), mViewingPage, mMatches,
                            mSelectedMatch - 1));
            mSearchModel.selectNextMatch(CycleRange.Direction.FORWARDS, mViewingPage);
        }
    }

    /**
     * registers the {@link FindInFileListener}
     */
    private void setFindInFileListener() {
        this.mFindInFileListener = mFindInFileListenerSetter;
    }

    /**
     *  Sets match count observer and search for any existing string in QueryBox
     */
    private void setMatchStatus() {
        // Set MatchCount Observer for Search
        setObservableMatchCount(
                (mFindInFileListener != null) ? mFindInFileListener.matchCount() : null);
        // Check for any existing string in QueryBox
        if (!mQueryBox.getText().toString().isEmpty()) {
            if (mFindInFileListener != null) {
                mFindInFileListener.onQueryTextChange(mQueryBox.getText().toString());
            }
            mMatchStatus.setVisibility(VISIBLE);
        }
    }

    private void setObservableMatchCount(@Nullable ObservableValue<MatchCount> matchCount) {
        if (this.mMatchCount != null) {
            this.mMatchCount.removeObserver(mMatchCountObserver);
        }
        this.mMatchCount = matchCount;
        if (this.mMatchCount != null) {
            this.mMatchCount.addObserver(mMatchCountObserver);
        }
    }

    /**
     * Requests focus and shows the keyboard when find in file view is inflated.
     */
    private void queryBoxRequestFocus() {
        mQueryBox.requestFocus();
        WindowCompat.getInsetsController(((Activity) getContext()).getWindow(), this)
                .show(WindowInsetsCompat.Type.ime());
    }

    /**
     * Enables touch mode focus for the view and sets the focus flag.
     */
    private void setFocusInTouchMode() {
        this.setFocusableInTouchMode(true);
        mFocus = true;
    }

    private int getViewingPage() {
        if (mIsRestoring) {
            return mViewingPage;
        }
        return mPaginatedView.getPageRangeHandler().getVisiblePage();
    }

    /**
     * Hides the Keyboard and clears focus from QueryBox when SingleTap event is detected
     */
    public void handleSingleTapEvent() {
        mQueryBox.clearFocus();
    }
}
