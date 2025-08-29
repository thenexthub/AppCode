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

#include "appcode/shell/platform/linux/fl_keyboard_layout.h"

#include "gtest/gtest.h"

TEST(FlKeyboardLayoutTest, SetLogicalKey) {
  g_autoptr(FlKeyboardLayout) layout = fl_keyboard_layout_new();

  EXPECT_EQ(fl_keyboard_layout_get_logical_key(layout, 0, 42),
            static_cast<uint64_t>(0));

  fl_keyboard_layout_set_logical_key(layout, 0, 42, 1234);

  EXPECT_EQ(fl_keyboard_layout_get_logical_key(layout, 0, 42),
            static_cast<uint64_t>(1234));
}

TEST(FlKeyboardLayoutTest, MaxValues) {
  g_autoptr(FlKeyboardLayout) layout = fl_keyboard_layout_new();

  EXPECT_EQ(fl_keyboard_layout_get_logical_key(layout, 255, 127),
            static_cast<uint64_t>(0));

  fl_keyboard_layout_set_logical_key(layout, 255, 127, 12345678);

  EXPECT_EQ(fl_keyboard_layout_get_logical_key(layout, 255, 127),
            static_cast<uint64_t>(12345678));
}

TEST(FlKeyboardLayoutTest, HasGroup) {
  g_autoptr(FlKeyboardLayout) layout = fl_keyboard_layout_new();

  EXPECT_FALSE(fl_keyboard_layout_has_group(layout, 42));

  fl_keyboard_layout_set_logical_key(layout, 42, 11, 22);

  EXPECT_TRUE(fl_keyboard_layout_has_group(layout, 42));
}
