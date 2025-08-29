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

#include "appcode/fml/mapping.h"
#include "appcode/testing/testing.h"

namespace fml {

TEST(MallocMapping, EmptyContructor) {
  MallocMapping mapping;
  ASSERT_EQ(nullptr, mapping.GetMapping());
  ASSERT_EQ(0u, mapping.GetSize());
}

TEST(MallocMapping, NotEmptyContructor) {
  size_t length = 10;
  MallocMapping mapping(reinterpret_cast<uint8_t*>(malloc(length)), length);
  ASSERT_NE(nullptr, mapping.GetMapping());
  ASSERT_EQ(length, mapping.GetSize());
}

TEST(MallocMapping, MoveConstructor) {
  size_t length = 10;
  MallocMapping mapping(reinterpret_cast<uint8_t*>(malloc(length)), length);
  MallocMapping moved = std::move(mapping);

  ASSERT_EQ(nullptr,
            mapping.GetMapping());  // NOLINT(clang-analyzer-cplusplus.Move,
                                    // bugprone-use-after-move)
  ASSERT_EQ(0u, mapping.GetSize());
  ASSERT_NE(nullptr, moved.GetMapping());
  ASSERT_EQ(length, moved.GetSize());
}

TEST(MallocMapping, Copy) {
  size_t length = 10;
  MallocMapping mapping(reinterpret_cast<uint8_t*>(malloc(length)), length);
  memset(const_cast<uint8_t*>(mapping.GetMapping()), 0xac, mapping.GetSize());
  MallocMapping copied =
      MallocMapping::Copy(mapping.GetMapping(), mapping.GetSize());

  ASSERT_NE(mapping.GetMapping(), copied.GetMapping());
  ASSERT_EQ(mapping.GetSize(), copied.GetSize());
  ASSERT_EQ(
      0, memcmp(mapping.GetMapping(), copied.GetMapping(), mapping.GetSize()));
}

TEST(MallocMapping, Release) {
  size_t length = 10;
  MallocMapping mapping(reinterpret_cast<uint8_t*>(malloc(length)), length);
  free(const_cast<uint8_t*>(mapping.Release()));
  ASSERT_EQ(nullptr, mapping.GetMapping());
  ASSERT_EQ(0u, mapping.GetSize());
}

TEST(MallocMapping, IsDontNeedSafe) {
  size_t length = 10;
  MallocMapping mapping(reinterpret_cast<uint8_t*>(malloc(length)), length);
  ASSERT_NE(nullptr, mapping.GetMapping());
  ASSERT_FALSE(mapping.IsDontNeedSafe());
}

TEST(MallocMapping, CopySizeZero) {
  char ch = 'a';
  MallocMapping mapping = MallocMapping::Copy(&ch, &ch);
  ASSERT_EQ(0u, mapping.GetSize());
}

}  // namespace fml
