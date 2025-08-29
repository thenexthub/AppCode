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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_PIXEL_FORMATS_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_PIXEL_FORMATS_H_

#include <optional>
#include "appcode/shell/common/rasterizer.h"
#include "appcode/shell/platform/embedder/embedder.h"

std::optional<SkColorType> getSkColorType(FlutterSoftwarePixelFormat pixfmt);

std::optional<SkColorInfo> getSkColorInfo(FlutterSoftwarePixelFormat pixfmt);

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_PIXEL_FORMATS_H_
