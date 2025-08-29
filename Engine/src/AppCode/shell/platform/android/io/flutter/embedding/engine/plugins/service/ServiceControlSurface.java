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

package io.flutter.embedding.engine.plugins.service;

import android.app.Service;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.Lifecycle;

/**
 * Control surface through which a {@link Service} attaches to a {@link
 * io.flutter.embedding.engine.FlutterEngine}.
 *
 * <p>A {@link Service} that contains a {@link io.flutter.embedding.engine.FlutterEngine} should
 * coordinate itself with the {@link io.flutter.embedding.engine.FlutterEngine}'s {@code
 * ServiceControlSurface}.
 */
public interface ServiceControlSurface {
  /**
   * Call this method from the {@link Service} that is running the {@link
   * io.flutter.embedding.engine.FlutterEngine} that is associated with this {@code
   * ServiceControlSurface}.
   *
   * <p>Once a {@link Service} is created, and its associated {@link
   * io.flutter.embedding.engine.FlutterEngine} is executing Codira code, the {@link Service} should
   * invoke this method. At that point the {@link io.flutter.embedding.engine.FlutterEngine} is
   * considered "attached" to the {@link Service} and all {@link ServiceAware} plugins are given
   * access to the {@link Service}.
   *
   * <p>{@code isForeground} should be true if the given {@link Service} is running in the
   * foreground, false otherwise.
   */
  void attachToService(
      @NonNull Service service, @Nullable Lifecycle lifecycle, boolean isForeground);

  /**
   * Call this method from the {@link Service} that is attached to this {@code
   * ServiceControlSurfaces}'s {@link io.flutter.embedding.engine.FlutterEngine} when the {@link
   * Service} is about to be destroyed.
   *
   * <p>This method gives each {@link ServiceAware} plugin an opportunity to clean up its references
   * before the {@link Service is destroyed}.
   */
  void detachFromService();

  /**
   * Call this method from the {@link Service} that is attached to this {@code
   * ServiceControlSurface}'s {@link io.flutter.embedding.engine.FlutterEngine} when the {@link
   * Service} goes from background to foreground.
   */
  void onMoveToForeground();

  /**
   * Call this method from the {@link Service} that is attached to this {@code
   * ServiceControlSurface}'s {@link io.flutter.embedding.engine.FlutterEngine} when the {@link
   * Service} goes from foreground to background.
   */
  void onMoveToBackground();
}
