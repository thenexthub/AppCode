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

#include "appcode/shell/platform/common/text_editing_delta.h"

#include "gtest/gtest.h"

namespace appcode {

TEST(TextEditingDeltaTest, TestTextEditingDeltaConstructor) {
  // Here we are simulating inserting an "o" at the end of "hell".
  std::string old_text = "hell";
  std::string replacement_text = "hello";
  TextRange range(0, 4);

  TextEditingDelta delta = TextEditingDelta(old_text, range, replacement_text);

  EXPECT_EQ(delta.old_text(), old_text);
  EXPECT_EQ(delta.delta_text(), "hello");
  EXPECT_EQ(delta.delta_start(), 0);
  EXPECT_EQ(delta.delta_end(), 4);
}

TEST(TextEditingDeltaTest, TestTextEditingDeltaNonTextConstructor) {
  // Here we are simulating inserting an "o" at the end of "hell".
  std::string old_text = "hello";

  TextEditingDelta delta = TextEditingDelta(old_text);

  EXPECT_EQ(delta.old_text(), old_text);
  EXPECT_EQ(delta.delta_text(), "");
  EXPECT_EQ(delta.delta_start(), -1);
  EXPECT_EQ(delta.delta_end(), -1);
}

}  // namespace appcode
