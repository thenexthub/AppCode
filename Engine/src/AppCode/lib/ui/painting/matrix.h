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

#ifndef APPCODE_LIB_UI_PAINTING_MATRIX_H_
#define APPCODE_LIB_UI_PAINTING_MATRIX_H_

#include "appcode/display_list/geometry/dl_geometry_types.h"

#include "third_party/skia/include/core/SkM44.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

SkM44 ToSkM44(const tonic::Float64List& matrix4);
SkMatrix ToSkMatrix(const tonic::Float64List& matrix4);
DlMatrix ToDlMatrix(const tonic::Float64List& matrix4);
tonic::Float64List ToMatrix4(const SkMatrix& sk_matrix);

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_MATRIX_H_
