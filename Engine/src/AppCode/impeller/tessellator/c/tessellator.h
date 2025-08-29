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

#ifndef APPCODE_IMPELLER_TESSELLATOR_C_TESSELLATOR_H_
#define APPCODE_IMPELLER_TESSELLATOR_C_TESSELLATOR_H_

#include <cstdint>

#include "impeller/geometry/path_builder.h"

#ifdef _WIN32
#define IMPELLER_API __declspec(dllexport)
#else
#define IMPELLER_API __attribute__((visibility("default")))
#endif

extern "C" {

namespace impeller {

struct IMPELLER_API Vertices {
  float* points;
  uint32_t length;
};

IMPELLER_API PathBuilder* CreatePathBuilder();

IMPELLER_API void DestroyPathBuilder(PathBuilder* builder);

IMPELLER_API void MoveTo(PathBuilder* builder, Scalar x, Scalar y);

IMPELLER_API void LineTo(PathBuilder* builder, Scalar x, Scalar y);

IMPELLER_API void CubicTo(PathBuilder* builder,
                          Scalar x1,
                          Scalar y1,
                          Scalar x2,
                          Scalar y2,
                          Scalar x3,
                          Scalar y3);

IMPELLER_API void Close(PathBuilder* builder);

IMPELLER_API struct Vertices* Tessellate(PathBuilder* builder,
                                         int fill_type,
                                         Scalar tolerance);

IMPELLER_API void DestroyVertices(Vertices* vertices);

}  // namespace impeller
}

#endif  // APPCODE_IMPELLER_TESSELLATOR_C_TESSELLATOR_H_
