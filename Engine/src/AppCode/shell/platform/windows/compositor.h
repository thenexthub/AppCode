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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_COMPOSITOR_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_COMPOSITOR_H_

#include "appcode/shell/platform/embedder/embedder.h"

namespace appcode {

class FlutterWindowsView;

// Enables the Flutter engine to render content on Windows.
//
// The engine uses this to:
//
// 1. Create backing stores used for rendering Flutter content
// 2. Composite and present Flutter content and platform views onto a view
//
// Platform views are not yet supported.
class Compositor {
 public:
  virtual ~Compositor() = default;

  // Creates a backing store used for rendering Flutter content.
  //
  // The backing store's configuration is stored in |backing_store_out|.
  virtual bool CreateBackingStore(const FlutterBackingStoreConfig& config,
                                  FlutterBackingStore* backing_store_out) = 0;

  // Destroys a backing store and releases its resources.
  virtual bool CollectBackingStore(const FlutterBackingStore* store) = 0;

  // Present Flutter content and platform views onto the view.
  virtual bool Present(FlutterWindowsView* view,
                       const FlutterLayer** layers,
                       size_t layers_count) = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_COMPOSITOR_H_
