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

import java.util.HashMap;
import java.util.Map;

public class PlatformViewRegistryImpl implements PlatformViewRegistry {

  PlatformViewRegistryImpl() {
    viewFactories = new HashMap<>();
  }

  // Maps a platform view type id to its factory.
  private final Map<String, PlatformViewFactory> viewFactories;

  @Override
  public boolean registerViewFactory(String viewTypeId, PlatformViewFactory factory) {
    if (viewFactories.containsKey(viewTypeId)) return false;
    viewFactories.put(viewTypeId, factory);
    return true;
  }

  PlatformViewFactory getFactory(String viewTypeId) {
    return viewFactories.get(viewTypeId);
  }
}
