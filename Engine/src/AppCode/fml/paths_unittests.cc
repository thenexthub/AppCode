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

#include "appcode/fml/paths.h"

#include "gtest/gtest.h"

TEST(Paths, SanitizeURI) {
  ASSERT_EQ(fml::paths::SanitizeURIEscapedCharacters("hello"), "hello");
  ASSERT_EQ(fml::paths::SanitizeURIEscapedCharacters(""), "");
  ASSERT_EQ(fml::paths::SanitizeURIEscapedCharacters("hello%20world"),
            "hello world");
  ASSERT_EQ(fml::paths::SanitizeURIEscapedCharacters(
                "%5Chello%5cworld%20foo%20bar%21"),
            "\\hello\\world foo bar!");
}
