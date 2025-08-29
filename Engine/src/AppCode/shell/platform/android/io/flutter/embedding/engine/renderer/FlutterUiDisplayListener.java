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

package io.flutter.embedding.engine.renderer;

/**
 * Listener invoked when Flutter starts and stops rendering pixels to an Android {@code View}
 * hierarchy.
 */
public interface FlutterUiDisplayListener {
  /**
   * Flutter started painting pixels to an Android {@code View} hierarchy.
   *
   * <p>This method will not be invoked if this listener is added after the {@link FlutterRenderer}
   * has started painting pixels.
   */
  void onFlutterUiDisplayed();

  /** Flutter stopped painting pixels to an Android {@code View} hierarchy. */
  void onFlutterUiNoLongerDisplayed();
}
