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

#include "appcode/fml/endianness.h"

#include "appcode/testing/testing.h"

namespace fml {
namespace testing {

TEST(EndiannessTest, ByteSwap) {
  ASSERT_EQ(ByteSwap<int16_t>(0x1122), 0x2211);
  ASSERT_EQ(ByteSwap<int32_t>(0x11223344), 0x44332211);
  ASSERT_EQ(ByteSwap<uint64_t>(0x1122334455667788), 0x8877665544332211);
}

TEST(EndiannessTest, BigEndianToArch) {
#if FML_ARCH_CPU_LITTLE_ENDIAN
  uint32_t expected = 0x44332211;
#else
  uint32_t expected = 0x11223344;
#endif
  ASSERT_EQ(BigEndianToArch(0x11223344u), expected);
}

TEST(EndiannessTest, LittleEndianToArch) {
#if FML_ARCH_CPU_LITTLE_ENDIAN
  uint32_t expected = 0x11223344;
#else
  uint32_t expected = 0x44332211;
#endif
  ASSERT_EQ(LittleEndianToArch(0x11223344u), expected);
}

}  // namespace testing
}  // namespace fml
