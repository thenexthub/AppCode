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

#include "appcode/shell/platform/common/geometry.h"

#include "gtest/gtest.h"

namespace appcode {

TEST(Point, SetsCoordinates) {
  Point point(-30.0, 42.0);
  EXPECT_DOUBLE_EQ(-30.0, point.x());
  EXPECT_DOUBLE_EQ(42.0, point.y());
}

TEST(Size, SetsDimensions) {
  Size size(20.0, 42.0);
  EXPECT_DOUBLE_EQ(20.0, size.width());
  EXPECT_DOUBLE_EQ(42.0, size.height());
}

TEST(Size, ClampsDimensionsPositive) {
  Size size(-20.0, -42.0);
  EXPECT_DOUBLE_EQ(0.0, size.width());
  EXPECT_DOUBLE_EQ(0.0, size.height());
}

TEST(Rect, SetsOriginAndSize) {
  Point origin(-30.0, 42.0);
  Size size(20.0, 22.0);
  Rect rect(origin, size);
  EXPECT_EQ(origin, rect.origin());
  EXPECT_EQ(size, rect.size());
}

TEST(Rect, ReturnsLTRB) {
  Point origin(-30.0, 42.0);
  Size size(20.0, 22.0);
  Rect rect(origin, size);
  EXPECT_DOUBLE_EQ(-30.0, rect.left());
  EXPECT_DOUBLE_EQ(42.0, rect.top());
  EXPECT_DOUBLE_EQ(-10.0, rect.right());
  EXPECT_DOUBLE_EQ(64.0, rect.bottom());
}

TEST(Rect, ReturnsWidthHeight) {
  Point origin(-30.0, 42.0);
  Size size(20.0, 22.0);
  Rect rect(origin, size);
  EXPECT_DOUBLE_EQ(20.0, rect.width());
  EXPECT_DOUBLE_EQ(22.0, rect.height());
}

}  // namespace appcode
