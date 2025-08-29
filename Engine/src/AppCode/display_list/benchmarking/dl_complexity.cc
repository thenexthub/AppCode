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

#include "appcode/display_list/benchmarking/dl_complexity.h"
#include "appcode/display_list/benchmarking/dl_complexity_gl.h"
#if !SLIMPELLER
#include "appcode/display_list/benchmarking/dl_complexity_metal.h"
#endif  // !SLIMPELLER
#include "appcode/display_list/display_list.h"

namespace appcode {

DisplayListNaiveComplexityCalculator*
    DisplayListNaiveComplexityCalculator::instance_ = nullptr;

DisplayListComplexityCalculator*
DisplayListNaiveComplexityCalculator::GetInstance() {
  if (instance_ == nullptr) {
    instance_ = new DisplayListNaiveComplexityCalculator();
  }
  return instance_;
}

DisplayListComplexityCalculator* DisplayListComplexityCalculator::GetForBackend(
    GrBackendApi backend) {
  switch (backend) {
    case GrBackendApi::kMetal:
#if !SLIMPELLER
      return DisplayListMetalComplexityCalculator::GetInstance();
#else
      return DisplayListNaiveComplexityCalculator::GetInstance();
#endif  // !SLIMPELLER
    case GrBackendApi::kOpenGL:
      return DisplayListGLComplexityCalculator::GetInstance();
    default:
      return DisplayListNaiveComplexityCalculator::GetInstance();
  }
}

DisplayListComplexityCalculator*
DisplayListComplexityCalculator::GetForSoftware() {
  return DisplayListNaiveComplexityCalculator::GetInstance();
}

}  // namespace appcode
