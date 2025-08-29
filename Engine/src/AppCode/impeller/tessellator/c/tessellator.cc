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

#include "tessellator.h"

#include <vector>

#include "impeller/tessellator/tessellator_libtess.h"

namespace impeller {
PathBuilder* CreatePathBuilder() {
  return new PathBuilder();
}

void DestroyPathBuilder(PathBuilder* builder) {
  delete builder;
}

void MoveTo(PathBuilder* builder, Scalar x, Scalar y) {
  builder->MoveTo(Point(x, y));
}

void LineTo(PathBuilder* builder, Scalar x, Scalar y) {
  builder->LineTo(Point(x, y));
}

void CubicTo(PathBuilder* builder,
             Scalar x1,
             Scalar y1,
             Scalar x2,
             Scalar y2,
             Scalar x3,
             Scalar y3) {
  builder->CubicCurveTo(Point(x1, y1), Point(x2, y2), Point(x3, y3));
}

void Close(PathBuilder* builder) {
  builder->Close();
}

struct Vertices* Tessellate(PathBuilder* builder,
                            int fill_type,
                            Scalar tolerance) {
  auto path = builder->CopyPath(static_cast<FillType>(fill_type));
  std::vector<float> points;
  if (TessellatorLibtess{}.Tessellate(
          path, tolerance,
          [&points](const float* vertices, size_t vertices_count,
                    const uint16_t* indices, size_t indices_count) {
            // Results are expected to be re-duplicated.
            std::vector<Point> raw_points;
            for (auto i = 0u; i < vertices_count * 2; i += 2) {
              raw_points.emplace_back(Point{vertices[i], vertices[i + 1]});
            }
            for (auto i = 0u; i < indices_count; i++) {
              auto point = raw_points[indices[i]];
              points.push_back(point.x);
              points.push_back(point.y);
            }
            return true;
          }) != TessellatorLibtess::Result::kSuccess) {
    return nullptr;
  }

  Vertices* vertices = new Vertices();
  vertices->points = new float[points.size()];
  if (!vertices->points) {
    return nullptr;
  }
  vertices->length = points.size();
  std::copy(points.begin(), points.end(), vertices->points);
  return vertices;
}

void DestroyVertices(Vertices* vertices) {
  delete vertices->points;
  delete vertices;
}

}  // namespace impeller
