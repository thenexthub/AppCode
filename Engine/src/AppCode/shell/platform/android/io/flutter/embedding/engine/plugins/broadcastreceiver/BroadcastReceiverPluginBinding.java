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

/**
 * Binding that gives {@link BroadcastReceiverAware} plugins access to an associated {@link
 * BroadcastReceiver}.
 */
public interface BroadcastReceiverPluginBinding {

  /**
   * Returns the {@link BroadcastReceiver} that is currently attached to the {@link
   * io.flutter.embedding.engine.FlutterEngine} that owns this {@code
   * BroadcastReceiverAwarePluginBinding}.
   */
  @NonNull
  BroadcastReceiver getBroadcastReceiver();
}
