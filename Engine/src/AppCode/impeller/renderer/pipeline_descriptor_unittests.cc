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

#include <unordered_set>

#include "appcode/testing/testing.h"
#include "impeller/renderer/pipeline_descriptor.h"

namespace impeller {
namespace testing {

TEST(PipelineDescriptorTest, PrimitiveTypeHashEquality) {
  PipelineDescriptor descA;
  PipelineDescriptor descB;

  ASSERT_TRUE(descA.IsEqual(descB));
  ASSERT_EQ(descA.GetHash(), descB.GetHash());

  descA.SetPrimitiveType(PrimitiveType::kTriangleStrip);

  ASSERT_FALSE(descA.IsEqual(descB));
  ASSERT_NE(descA.GetHash(), descB.GetHash());
}

}  // namespace  testing
}  // namespace impeller
