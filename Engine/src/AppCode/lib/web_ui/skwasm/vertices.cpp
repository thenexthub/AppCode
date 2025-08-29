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

#include "export.h"

#include "third_party/skia/include/core/SkVertices.h"

SKWASM_EXPORT SkVertices* vertices_create(SkVertices::VertexMode vertexMode,
                                          int vertexCount,
                                          SkPoint* positions,
                                          SkPoint* textureCoordinates,
                                          SkColor* colors,
                                          int indexCount,
                                          uint16_t* indices) {
  return SkVertices::MakeCopy(vertexMode, vertexCount, positions,
                              textureCoordinates, colors, indexCount, indices)
      .release();
}

SKWASM_EXPORT void vertices_dispose(SkVertices* vertices) {
  vertices->unref();
}
