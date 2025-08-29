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

#ifndef UI_GFX_TRANSFORM_H_
#define UI_GFX_TRANSFORM_H_

#include <iosfwd>
#include <string>

#include "geometry/rect_f.h"
#include "gfx_export.h"

namespace gfx {

class RectF;

// 4x4 transformation matrix. Transform is cheap and explicitly allows
// copy/assign.
class GFX_EXPORT Transform {
 public:
  Transform();

  Transform(float col1row1,
            float col2row1,
            float col3row1,
            float col4row1,
            float col1row2,
            float col2row2,
            float col3row2,
            float col4row2,
            float col1row3,
            float col2row3,
            float col3row3,
            float col4row3,
            float col1row4,
            float col2row4,
            float col3row4,
            float col4row4);
  // Constructs a transform from explicit 2d elements. All other matrix
  // elements remain the same as the corresponding elements of an identity
  // matrix.
  Transform(float col1row1,
            float col2row1,
            float col1row2,
            float col2row2,
            float x_translation,
            float y_translation);

  bool operator==(const Transform& rhs) const;
  bool operator!=(const Transform& rhs) const { return !(*this == rhs); };

  float operator[](int index) const {
    BASE_DCHECK((unsigned)index < 16);
    return matrix_[index];
  }

  // Returns true if this is the identity matrix.
  bool IsIdentity() const;

  // Applies the current transformation on a scaling and assigns the result
  // to |this|.
  void Scale(float x, float y);

  // Applies transformation on the given rect. After the function completes,
  // |rect| will be the smallest axis aligned bounding rect containing the
  // transformed rect.
  void TransformRect(RectF* rect) const;

  // Applies transformation on the given point
  void TransformPoint(PointF* point) const;

  std::string ToString() const;

 private:
  // Row-major array
  float matrix_[16];
  bool is_identity_;

  void UpdateIdentity();
};

// This is declared here for use in gtest-based unit tests but is defined in
// the //ui/gfx:test_support target. Depend on that to use this in your unit
// test. This should not be used in production code - call ToString() instead.
void PrintTo(const Transform& transform, ::std::ostream* os);

}  // namespace gfx

#endif  // UI_GFX_TRANSFORM_H_
