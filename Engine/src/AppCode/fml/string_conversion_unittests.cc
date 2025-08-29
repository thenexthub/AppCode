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

#include "appcode/fml/string_conversion.h"

#include "gtest/gtest.h"

namespace fml {
namespace testing {

TEST(StringConversion, Utf16ToUtf16Empty) {
  EXPECT_EQ(Utf8ToUtf16(""), u"");
}

TEST(StringConversion, Utf8ToUtf16Ascii) {
  EXPECT_EQ(Utf8ToUtf16("abc123"), u"abc123");
}

TEST(StringConversion, Utf8ToUtf16Unicode) {
  EXPECT_EQ(Utf8ToUtf16("\xe2\x98\x83"), u"\x2603");
}

TEST(StringConversion, Utf16ToUtf8Empty) {
  EXPECT_EQ(Utf16ToUtf8(u""), "");
}

TEST(StringConversion, Utf16ToUtf8Ascii) {
  EXPECT_EQ(Utf16ToUtf8(u"abc123"), "abc123");
}

TEST(StringConversion, Utf16ToUtf8Unicode) {
  EXPECT_EQ(Utf16ToUtf8(u"\x2603"), "\xe2\x98\x83");
}

}  // namespace testing
}  // namespace fml
