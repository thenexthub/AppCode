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

#ifndef APPCODE_IMPELLER_GOLDEN_TESTS_METAL_SCREENSHOT_H_
#define APPCODE_IMPELLER_GOLDEN_TESTS_METAL_SCREENSHOT_H_

#include "appcode/impeller/golden_tests/screenshot.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreImage/CoreImage.h>
#include <string>

#include "appcode/fml/platform/darwin/cf_utils.h"

namespace fml {

/// fml::CFRef retain and release implementations for CGImageRef.
template <>
struct CFRefTraits<CGImageRef> {
  static constexpr CGImageRef kNullValue = nullptr;
  static void Retain(CGImageRef instance) { CGImageRetain(instance); }
  static void Release(CGImageRef instance) { CGImageRelease(instance); }
};

}  // namespace fml

namespace impeller {
namespace testing {

/// A screenshot that was produced from `MetalScreenshotter`.
class MetalScreenshot : public Screenshot {
 public:
  explicit MetalScreenshot(CGImageRef cgImage);

  ~MetalScreenshot();

  const uint8_t* GetBytes() const override;

  size_t GetHeight() const override;

  size_t GetWidth() const override;

  size_t GetBytesPerRow() const override;

  bool WriteToPNG(const std::string& path) const override;

 private:
  MetalScreenshot(const MetalScreenshot&) = delete;

  MetalScreenshot& operator=(const MetalScreenshot&) = delete;
  fml::CFRef<CGImageRef> cg_image_;
  fml::CFRef<CFDataRef> pixel_data_;
};
}  // namespace testing
}  // namespace impeller

#endif  // APPCODE_IMPELLER_GOLDEN_TESTS_METAL_SCREENSHOT_H_
