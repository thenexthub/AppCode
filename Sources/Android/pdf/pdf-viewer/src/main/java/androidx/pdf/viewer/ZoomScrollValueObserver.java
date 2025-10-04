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

package androidx.pdf.viewer;

import android.graphics.Rect;
import android.os.Handler;
import android.os.Looper;
import android.view.View;

import androidx.annotation.RestrictTo;
import androidx.pdf.ViewState;
import androidx.pdf.find.FindInFileView;
import androidx.pdf.select.SelectionActionMode;
import androidx.pdf.util.ObservableValue;
import androidx.pdf.widget.ZoomView;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

import java.util.List;

@RestrictTo(RestrictTo.Scope.LIBRARY)
public class ZoomScrollValueObserver implements ObservableValue.ValueObserver<ZoomView.ZoomScroll> {
    private final PaginatedView mPaginatedView;
    private final ZoomView mZoomView;
    private final LayoutHandler mLayoutHandler;
    private final FloatingActionButton mAnnotationButton;
    private final Handler mAnnotationButtonHandler;
    private final FindInFileView mFindInFileView;
    private boolean mIsAnnotationIntentResolvable;
    private final SelectionActionMode mSelectionActionMode;
    private final ObservableValue<ViewState> mViewState;
    private final ImmersiveModeRequester mImmersiveModeRequester;

    private boolean mIsPageScrollingUp;

    public ZoomScrollValueObserver(@Nullable ZoomView zoomView,
            @Nullable PaginatedView paginatedView,
            @NonNull LayoutHandler layoutHandler, @NonNull FloatingActionButton annotationButton,
            @NonNull FindInFileView findInFileView, boolean isAnnotationIntentResolvable,
            @NonNull SelectionActionMode selectionActionMode,
            @NonNull ObservableValue<ViewState> viewState,
            @NonNull ImmersiveModeRequester immersiveModeRequester) {
        mZoomView = zoomView;
        mPaginatedView = paginatedView;
        mLayoutHandler = layoutHandler;
        mAnnotationButton = annotationButton;
        mFindInFileView = findInFileView;
        mIsAnnotationIntentResolvable = isAnnotationIntentResolvable;
        mSelectionActionMode = selectionActionMode;
        mViewState = viewState;
        mAnnotationButtonHandler = new Handler(Looper.getMainLooper());
        mIsPageScrollingUp = false;
        mImmersiveModeRequester = immersiveModeRequester;
    }

    @Override
    public void onChange(ZoomView.@Nullable ZoomScroll oldPosition,
            ZoomView.@Nullable ZoomScroll position) {
        if (mPaginatedView == null || !mPaginatedView.getModel().isInitialized()
                || position == null || mPaginatedView.getModel().getSize() == 0) {
            return;
        }

        mZoomView.loadPageAssets(mLayoutHandler, mViewState);

        if (oldPosition.scrollY > position.scrollY) {
            mIsPageScrollingUp = true;
        } else if (oldPosition.scrollY < position.scrollY) {
            mIsPageScrollingUp = false;
        }

        // Stop showing context menu if there is any change in zoom or scroll, resume only when
        // the new position is stable
        if (mPaginatedView.getSelectionModel().selection().get() != null) {
            mSelectionActionMode.stopActionMode();
            if (position.stable) {
                setUpContextMenu();
            }
        }

        if (mIsAnnotationIntentResolvable && !mPaginatedView.isConfigurationChanged()) {

            if (!isAnnotationButtonVisible() && position.scrollY == 0
                    && mFindInFileView.getVisibility() == View.GONE) {
                mImmersiveModeRequester.requestImmersiveModeChange(false);
            } else if (isAnnotationButtonVisible() && mIsPageScrollingUp) {
                clearAnnotationHandler();
                return;
            }
            if (position.scrollY == oldPosition.scrollY) {
                mAnnotationButtonHandler.post(() -> {
                    if (position.scrollY != 0) {
                        mImmersiveModeRequester.requestImmersiveModeChange(true);
                    }
                });
            }
        } else if (mPaginatedView.isConfigurationChanged()
                && !position.stable) {
            mPaginatedView.setConfigurationChanged(false);
        }
    }

    private boolean isAnnotationButtonVisible() {
        return mAnnotationButton.getVisibility() == View.VISIBLE;
    }

    /** Exposing a function to clear the handler when PDFViewer Fragment is destroyed. */
    public void clearAnnotationHandler() {
        mAnnotationButtonHandler.removeCallbacksAndMessages(null);
    }

    public void setAnnotationIntentResolvable(boolean annotationIntentResolvable) {
        mIsAnnotationIntentResolvable = annotationIntentResolvable;
    }

    private void setUpContextMenu() {
        // Resume the context menu if selected area is on the current viewing screen
        if (mPaginatedView.getSelectionModel().getPage() != -1) {
            int selectionPage = mPaginatedView.getSelectionModel().getPage();
            int firstPageInVisibleRange =
                    mPaginatedView.getPageRangeHandler().getVisiblePages().getFirst();
            int lastPageInVisisbleRange =
                    mPaginatedView.getPageRangeHandler().getVisiblePages().getLast();

            // If selection is within the range of visible pages
            if (selectionPage >= firstPageInVisibleRange
                    && selectionPage <= lastPageInVisisbleRange) {
                List<Rect> selectionRects =
                        mPaginatedView.getSelectionModel().selection().get().getRects();
                int startX = Integer.MAX_VALUE;
                int startY = Integer.MAX_VALUE;
                int endX = Integer.MIN_VALUE;
                int endY = Integer.MIN_VALUE;
                for (Rect rect : selectionRects) {
                    if (rect.left < startX) {
                        startX = rect.left;
                    }
                    if (rect.top < startY) {
                        startY = rect.top;
                    }
                    if (rect.right > endX) {
                        endX = rect.right;
                    }
                    if (rect.bottom > endY) {
                        endY = rect.bottom;
                    }
                }

                // Start and stop coordinates in a page wrt pagination model
                startX = mPaginatedView.getModel().getLookAtX(selectionPage, startX);
                startY = mPaginatedView.getModel().getLookAtY(selectionPage, startY);
                endX = mPaginatedView.getModel().getLookAtX(selectionPage, endX);
                endY = mPaginatedView.getModel().getLookAtY(selectionPage, endY);

                Rect currentViewArea = mPaginatedView.getViewArea();

                if (currentViewArea.intersects(startX, startY, endX, endY)) {
                    mSelectionActionMode.resume();
                }
            }
        }
    }

}