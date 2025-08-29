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

#include <memory>
#include "appcode/testing/testing.h"
#include "gtest/gtest.h"
#include "impeller/entity/contents/filters/inputs/filter_input.h"
#include "impeller/entity/entity.h"
#include "impeller/geometry/geometry_asserts.h"

namespace impeller {
namespace testing {

TEST(FilterInputTest, CanSetLocalTransformForTexture) {
  std::shared_ptr<Texture> texture = nullptr;
  auto input =
      FilterInput::Make(texture, Matrix::MakeTranslation({1.0, 0.0, 0.0}));
  Entity e;
  e.SetTransform(Matrix::MakeTranslation({0.0, 2.0, 0.0}));

  ASSERT_MATRIX_NEAR(input->GetLocalTransform(e),
                     Matrix::MakeTranslation({1.0, 0.0, 0.0}));
  ASSERT_MATRIX_NEAR(input->GetTransform(e),
                     Matrix::MakeTranslation({1.0, 2.0, 0.0}));
}

}  // namespace testing
}  // namespace impeller
