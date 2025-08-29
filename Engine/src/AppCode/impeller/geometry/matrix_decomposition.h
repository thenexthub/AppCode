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

#ifndef APPCODE_IMPELLER_GEOMETRY_MATRIX_DECOMPOSITION_H_
#define APPCODE_IMPELLER_GEOMETRY_MATRIX_DECOMPOSITION_H_

#include "impeller/geometry/quaternion.h"
#include "impeller/geometry/scalar.h"
#include "impeller/geometry/shear.h"
#include "impeller/geometry/vector.h"

namespace impeller {

struct MatrixDecomposition {
  Vector3 translation;
  Vector3 scale;
  Shear shear;
  Vector4 perspective;
  Quaternion rotation;

  enum class Component {
    kTranslation = 1 << 0,
    kScale = 1 << 1,
    kShear = 1 << 2,
    kPerspective = 1 << 3,
    kRotation = 1 << 4,
  };

  uint64_t GetComponentsMask() const;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_GEOMETRY_MATRIX_DECOMPOSITION_H_
