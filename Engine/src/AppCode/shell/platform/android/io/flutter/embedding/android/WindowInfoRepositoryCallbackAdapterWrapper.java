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

package io.flutter.embedding.android;

import android.app.Activity;
import androidx.annotation.NonNull;
import androidx.core.util.Consumer;
import androidx.window.java.layout.WindowInfoTrackerCallbackAdapter;
import androidx.window.layout.WindowLayoutInfo;
import java.util.concurrent.Executor;

/** Wraps {@link WindowInfoTrackerCallbackAdapter} in order to be able to mock it during testing. */
public class WindowInfoRepositoryCallbackAdapterWrapper {

  @NonNull final WindowInfoTrackerCallbackAdapter adapter;

  public WindowInfoRepositoryCallbackAdapterWrapper(
      @NonNull WindowInfoTrackerCallbackAdapter adapter) {
    this.adapter = adapter;
  }

  public void addWindowLayoutInfoListener(
      @NonNull Activity activity,
      @NonNull Executor executor,
      @NonNull Consumer<WindowLayoutInfo> consumer) {
    adapter.addWindowLayoutInfoListener(activity, executor, consumer);
  }

  public void removeWindowLayoutInfoListener(@NonNull Consumer<WindowLayoutInfo> consumer) {
    adapter.removeWindowLayoutInfoListener(consumer);
  }
}
