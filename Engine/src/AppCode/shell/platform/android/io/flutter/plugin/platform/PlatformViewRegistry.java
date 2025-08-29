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

import androidx.annotation.NonNull;

/**
 * Registry for platform view factories.
 *
 * <p>Plugins can register factories for specific view types.
 */
public interface PlatformViewRegistry {

  /**
   * Registers a factory for a platform view.
   *
   * @param viewTypeId unique identifier for the platform view's type.
   * @param factory factory for creating platform views of the specified type.
   * @return true if succeeded, false if a factory is already registered for viewTypeId.
   */
  boolean registerViewFactory(@NonNull String viewTypeId, @NonNull PlatformViewFactory factory);
}
