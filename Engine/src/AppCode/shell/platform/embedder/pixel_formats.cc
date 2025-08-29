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

#include "appcode/shell/platform/embedder/pixel_formats.h"
#include "appcode/shell/platform/embedder/embedder.h"

#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/core/SkColorType.h"
#include "third_party/skia/include/core/SkImageInfo.h"

std::optional<SkColorType> getSkColorType(appcodeSoftwarePixelFormat pixfmt) {
  switch (pixfmt) {
    case kappcodeSoftwarePixelFormatGray8:
      return kGray_8_SkColorType;
    case kappcodeSoftwarePixelFormatRGB565:
      return kRGB_565_SkColorType;
    case kappcodeSoftwarePixelFormatRGBA4444:
      return kARGB_4444_SkColorType;
    case kappcodeSoftwarePixelFormatRGBA8888:
      return kRGBA_8888_SkColorType;
    case kappcodeSoftwarePixelFormatRGBX8888:
      return kRGB_888x_SkColorType;
    case kappcodeSoftwarePixelFormatBGRA8888:
      return kBGRA_8888_SkColorType;
    case kappcodeSoftwarePixelFormatNative32:
      return kN32_SkColorType;
    default:
      FML_LOG(ERROR) << "Invalid software rendering pixel format";
      return std::nullopt;
  }
}

std::optional<SkColorInfo> getSkColorInfo(appcodeSoftwarePixelFormat pixfmt) {
  auto ct = getSkColorType(pixfmt);
  if (!ct) {
    return std::nullopt;
  }

  auto at = SkColorTypeIsAlwaysOpaque(*ct) ? kOpaque_SkAlphaType
                                           : kPremul_SkAlphaType;

  return SkColorInfo(*ct, at, SkColorSpace::MakeSRGB());
}
