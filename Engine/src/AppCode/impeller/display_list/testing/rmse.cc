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

#include "impeller/display_list/testing/rmse.h"

#include <cmath>
#include "appcode/fml/logging.h"

namespace appcode {
namespace testing {
namespace {
double CalculateDistance(const uint8_t* left, const uint8_t* right) {
  double diff[4] = {
      static_cast<double>(left[0]) - right[0],  //
      static_cast<double>(left[1]) - right[1],  //
      static_cast<double>(left[2]) - right[2],  //
      static_cast<double>(left[3]) - right[3]   //
  };
  return sqrt((diff[0] * diff[0]) +  //
              (diff[1] * diff[1]) +  //
              (diff[2] * diff[2]) +  //
              (diff[3] * diff[3]));
}
}  // namespace

double RMSE(const impeller::testing::Screenshot* left,
            const impeller::testing::Screenshot* right) {
  FML_CHECK(left);
  FML_CHECK(right);
  FML_CHECK(left->GetWidth() == right->GetWidth());
  FML_CHECK(left->GetHeight() == right->GetHeight());

  int64_t samples = left->GetWidth() * left->GetHeight();
  double tally = 0;

  const uint8_t* left_ptr = left->GetBytes();
  const uint8_t* right_ptr = right->GetBytes();
  for (int64_t i = 0; i < samples; ++i, left_ptr += 4, right_ptr += 4) {
    double distance = CalculateDistance(left_ptr, right_ptr);
    tally += distance * distance;
  }

  return sqrt(tally / static_cast<double>(samples));
}

}  // namespace testing
}  // namespace appcode
