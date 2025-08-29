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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_COMPOSITOR_SOFTWARE_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_COMPOSITOR_SOFTWARE_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/windows/compositor.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"

namespace appcode {

// Enables the Flutter engine to render content on Windows using software
// rasterization and bitmaps.
class CompositorSoftware : public Compositor {
 public:
  CompositorSoftware();

  /// |Compositor|
  bool CreateBackingStore(const FlutterBackingStoreConfig& config,
                          FlutterBackingStore* result) override;
  /// |Compositor|
  bool CollectBackingStore(const FlutterBackingStore* store) override;

  /// |Compositor|
  bool Present(FlutterWindowsView* view,
               const FlutterLayer** layers,
               size_t layers_count) override;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(CompositorSoftware);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_COMPOSITOR_SOFTWARE_H_
