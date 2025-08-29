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

#include "appcode/testing/testing.h"
#include "impeller/core/buffer_view.h"

namespace impeller {
namespace testing {

TEST(BufferViewTest, Empty) {
  BufferView buffer_view;
  EXPECT_FALSE(buffer_view);
}

TEST(BufferViewTest, TakeRaw) {
  DeviceBuffer* buffer = reinterpret_cast<DeviceBuffer*>(0xcafebabe);
  BufferView buffer_view(buffer, {0, 123});
  EXPECT_TRUE(buffer_view);
  std::shared_ptr<const DeviceBuffer> taken = buffer_view.TakeBuffer();
  EXPECT_FALSE(taken);
  EXPECT_EQ(buffer_view.GetBuffer(), buffer);
}

}  // namespace testing
}  // namespace impeller
