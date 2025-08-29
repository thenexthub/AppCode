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

import android.view.View;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.flutter.view.AccessibilityBridge;

/** Facilitates interaction between the accessibility bridge and embedded platform views. */
public interface PlatformViewsAccessibilityDelegate {
  /**
   * Returns the root of the view hierarchy for the platform view with the requested id, or null if
   * there is no corresponding view.
   */
  @Nullable
  View getPlatformViewById(int viewId);

  /** Returns true if the platform view uses virtual displays. */
  boolean usesVirtualDisplay(int id);

  /**
   * Attaches an accessibility bridge for this platform views accessibility delegate.
   *
   * <p>Accessibility events originating in platform views belonging to this delegate will be
   * delegated to this accessibility bridge.
   */
  void attachAccessibilityBridge(@NonNull AccessibilityBridge accessibilityBridge);

  /**
   * Detaches the current accessibility bridge.
   *
   * <p>Any accessibility events sent by platform views belonging to this delegate will be ignored
   * until a new accessibility bridge is attached.
   */
  void detachAccessibilityBridge();
}
