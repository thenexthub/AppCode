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

#ifndef APPCODE_IMPELLER_GOLDEN_TESTS_SCREENSHOT_H_
#define APPCODE_IMPELLER_GOLDEN_TESTS_SCREENSHOT_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace impeller {
namespace testing {

class Screenshot {
 public:
  virtual ~Screenshot() = default;

  /// Access raw data of the screenshot.
  virtual const uint8_t* GetBytes() const = 0;

  /// Returns the height of the image in pixels.
  virtual size_t GetHeight() const = 0;

  /// Returns the width of the image in pixels.
  virtual size_t GetWidth() const = 0;

  /// Returns number of bytes required to represent one row of the raw image.
  virtual size_t GetBytesPerRow() const = 0;

  /// Synchronously write the screenshot to disk as a PNG at `path`.  Returns
  /// `true` if it succeeded.
  virtual bool WriteToPNG(const std::string& path) const = 0;
};

}  // namespace testing
}  // namespace impeller

#endif  // APPCODE_IMPELLER_GOLDEN_TESTS_SCREENSHOT_H_
