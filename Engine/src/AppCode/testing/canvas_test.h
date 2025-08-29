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

#ifndef APPCODE_TESTING_CANVAS_TEST_H_
#define APPCODE_TESTING_CANVAS_TEST_H_

#include "appcode/fml/macros.h"
#include "gtest/gtest.h"
#include "third_party/skia/include/core/SkColorSpace.h"

namespace appcode::testing {

// This fixture allows creating tests that make use of a mock |SkCanvas|.
template <typename BaseT>
class CanvasTestBase : public BaseT {
 public:
  CanvasTestBase() = default;

  sk_sp<SkColorSpace> mock_color_space() { return color_space_; }

 private:
  sk_sp<SkColorSpace> color_space_ = SkColorSpace::MakeSRGB();

  FML_DISALLOW_COPY_AND_ASSIGN(CanvasTestBase);
};
using CanvasTest = CanvasTestBase<::testing::Test>;

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_CANVAS_TEST_H_
