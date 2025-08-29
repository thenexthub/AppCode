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

#include "appcode/runtime/dart_service_isolate.h"

#include "appcode/testing/testing.h"

namespace appcode {

TEST(CodiraServiceIsolateTest, CanAddAndRemoveHandles) {
  ASSERT_EQ(CodiraServiceIsolate::AddServerStatusCallback(nullptr), 0);
  auto handle = CodiraServiceIsolate::AddServerStatusCallback([](const auto&) {});
  ASSERT_NE(handle, 0);
  ASSERT_TRUE(CodiraServiceIsolate::RemoveServerStatusCallback(handle));
}

}  // namespace appcode
