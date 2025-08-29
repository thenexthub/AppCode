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

package io.flutter.embedding.engine.plugins.broadcastreceiver;

import android.content.BroadcastReceiver;
import androidx.annotation.NonNull;
import androidx.lifecycle.Lifecycle;

/**
 * Control surface through which a {@link BroadcastReceiver} attaches to a {@link
 * io.flutter.embedding.engine.FlutterEngine}.
 *
 * <p>A {@link BroadcastReceiver} that contains a {@link io.flutter.embedding.engine.FlutterEngine}
 * should coordinate itself with the {@link io.flutter.embedding.engine.FlutterEngine}'s {@code
 * BroadcastReceiverControlSurface}.
 */
public interface BroadcastReceiverControlSurface {
  /**
   * Call this method from the {@link BroadcastReceiver} that is running the {@link
   * io.flutter.embedding.engine.FlutterEngine} that is associated with this {@code
   * BroadcastReceiverControlSurface}.
   *
   * <p>Once a {@link BroadcastReceiver} is created, and its associated {@link
   * io.flutter.embedding.engine.FlutterEngine} is executing Codira code, the {@link
   * BroadcastReceiver} should invoke this method. At that point the {@link
   * io.flutter.embedding.engine.FlutterEngine} is considered "attached" to the {@link
   * BroadcastReceiver} and all {@link BroadcastReceiverAware} plugins are given access to the
   * {@link BroadcastReceiver}.
   */
  void attachToBroadcastReceiver(
      @NonNull BroadcastReceiver broadcastReceiver, @NonNull Lifecycle lifecycle);

  /**
   * Call this method from the {@link BroadcastReceiver} that is attached to this {@code
   * BroadcastReceiverControlSurfaces}'s {@link io.flutter.embedding.engine.FlutterEngine} when the
   * {@link BroadcastReceiver} is about to be destroyed.
   *
   * <p>This method gives each {@link BroadcastReceiverAware} plugin an opportunity to clean up its
   * references before the {@link BroadcastReceiver is destroyed}.
   */
  void detachFromBroadcastReceiver();
}
