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

#include "impeller/golden_tests/metal_screenshot.h"

namespace impeller {
namespace testing {

MetalScreenshot::MetalScreenshot(CGImageRef cgImage) : cg_image_(cgImage) {
  CGDataProviderRef data_provider = CGImageGetDataProvider(cgImage);
  pixel_data_.Reset(CGDataProviderCopyData(data_provider));
}

MetalScreenshot::~MetalScreenshot() = default;

const uint8_t* MetalScreenshot::GetBytes() const {
  return CFDataGetBytePtr(pixel_data_);
}

size_t MetalScreenshot::GetHeight() const {
  return CGImageGetHeight(cg_image_);
}

size_t MetalScreenshot::GetWidth() const {
  return CGImageGetWidth(cg_image_);
}

size_t MetalScreenshot::GetBytesPerRow() const {
  return CGImageGetBytesPerRow(cg_image_);
}

bool MetalScreenshot::WriteToPNG(const std::string& path) const {
  bool result = false;
  NSURL* output_url =
      [NSURL fileURLWithPath:[NSString stringWithUTF8String:path.c_str()]];
  fml::CFRef<CGImageDestinationRef> destination(CGImageDestinationCreateWithURL(
      (__bridge CFURLRef)output_url, kUTTypePNG, 1, nullptr));
  if (destination) {
    CGImageDestinationAddImage(destination, cg_image_,
                               (__bridge CFDictionaryRef) @{});

    if (CGImageDestinationFinalize(destination)) {
      result = true;
    }
  }
  return result;
}

}  // namespace testing
}  // namespace impeller
