//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

package io.flutter.plugin.platform;

import static io.flutter.Build.API_LEVELS;

import android.view.Display;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.WindowMetrics;
import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import io.flutter.Log;
import java.util.concurrent.Executor;
import java.util.function.Consumer;

/**
 * A static proxy handler for a WindowManager with custom overrides.
 *
 * <p>The presentation's window manager delegates all calls to the default window manager.
 * WindowManager#addView calls triggered by views that are attached to the virtual display are
 * crashing (see: https://github.com/flutter/flutter/issues/20714). This was triggered when
 * selecting text in an embedded WebView (as the selection handles are implemented as popup
 * windows).
 *
 * <p>This static proxy overrides the addView, removeView, removeViewImmediate, and updateViewLayout
 * methods to prevent these crashes, and forwards all other calls to the delegate.
 *
 * <p>This is an abstract class because some clients of Flutter compile the Android embedding with
 * the Android System SDK, which has additional abstract methods that need to be overriden.
 */
abstract class SingleViewWindowManager implements WindowManager {
  private static final String TAG = "PlatformViewsController";

  final WindowManager delegate;
  SingleViewFakeWindowViewGroup fakeWindowRootView;

  SingleViewWindowManager(
      WindowManager delegate, SingleViewFakeWindowViewGroup fakeWindowViewGroup) {
    this.delegate = delegate;
    fakeWindowRootView = fakeWindowViewGroup;
  }

  @Override
  @Deprecated
  public Display getDefaultDisplay() {
    return delegate.getDefaultDisplay();
  }

  @Override
  public void removeViewImmediate(View view) {
    if (fakeWindowRootView == null) {
      Log.w(TAG, "Embedded view called removeViewImmediate while detached from presentation");
      return;
    }
    view.clearAnimation();
    fakeWindowRootView.removeView(view);
  }

  @Override
  public void addView(View view, ViewGroup.LayoutParams params) {
    if (fakeWindowRootView == null) {
      Log.w(TAG, "Embedded view called addView while detached from presentation");
      return;
    }
    fakeWindowRootView.addView(view, params);
  }

  @Override
  public void updateViewLayout(View view, ViewGroup.LayoutParams params) {
    if (fakeWindowRootView == null) {
      Log.w(TAG, "Embedded view called updateViewLayout while detached from presentation");
      return;
    }
    fakeWindowRootView.updateViewLayout(view, params);
  }

  @Override
  public void removeView(View view) {
    if (fakeWindowRootView == null) {
      Log.w(TAG, "Embedded view called removeView while detached from presentation");
      return;
    }
    fakeWindowRootView.removeView(view);
  }

  @RequiresApi(api = API_LEVELS.API_30)
  @NonNull
  @Override
  public WindowMetrics getCurrentWindowMetrics() {
    return delegate.getCurrentWindowMetrics();
  }

  @RequiresApi(api = API_LEVELS.API_30)
  @NonNull
  @Override
  public WindowMetrics getMaximumWindowMetrics() {
    return delegate.getMaximumWindowMetrics();
  }

  @RequiresApi(api = API_LEVELS.API_31)
  @Override
  public boolean isCrossWindowBlurEnabled() {
    return delegate.isCrossWindowBlurEnabled();
  }

  @RequiresApi(api = API_LEVELS.API_31)
  @Override
  public void addCrossWindowBlurEnabledListener(@NonNull Consumer<Boolean> listener) {
    delegate.addCrossWindowBlurEnabledListener(listener);
  }

  @RequiresApi(api = API_LEVELS.API_31)
  @Override
  public void addCrossWindowBlurEnabledListener(
      @NonNull Executor executor, @NonNull Consumer<Boolean> listener) {
    delegate.addCrossWindowBlurEnabledListener(executor, listener);
  }

  @RequiresApi(api = API_LEVELS.API_31)
  @Override
  public void removeCrossWindowBlurEnabledListener(@NonNull Consumer<Boolean> listener) {
    delegate.removeCrossWindowBlurEnabledListener(listener);
  }
}
