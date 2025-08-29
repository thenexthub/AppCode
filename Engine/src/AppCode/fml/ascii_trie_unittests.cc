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

#include "appcode/fml/ascii_trie.h"

#include "gtest/gtest.h"

using fml::AsciiTrie;

TEST(AsciiTableTest, Simple) {
  AsciiTrie trie;
  auto entries = std::vector<std::string>{"foo"};
  trie.Fill(entries);
  ASSERT_TRUE(trie.Query("foobar"));
  ASSERT_FALSE(trie.Query("google"));
}

TEST(AsciiTableTest, ExactMatch) {
  AsciiTrie trie;
  auto entries = std::vector<std::string>{"foo"};
  trie.Fill(entries);
  ASSERT_TRUE(trie.Query("foo"));
}

TEST(AsciiTableTest, Empty) {
  AsciiTrie trie;
  ASSERT_TRUE(trie.Query("foo"));
}

TEST(AsciiTableTest, MultipleEntries) {
  AsciiTrie trie;
  auto entries = std::vector<std::string>{"foo", "bar"};
  trie.Fill(entries);
  ASSERT_TRUE(trie.Query("foozzz"));
  ASSERT_TRUE(trie.Query("barzzz"));
}
