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

#include "display_list/dl_color.h"
#include "appcode/flow/stopwatch_dl.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

static DlRect MakeRectFromVertices(DlPoint vertices[6]) {
  // "Combine" the vertices to form a rectangle.
  auto const left = std::min(vertices[0].x, vertices[5].x);
  auto const top = std::min(vertices[0].y, vertices[1].y);
  auto const right = std::max(vertices[1].x, vertices[2].x);
  auto const bottom = std::max(vertices[2].y, vertices[3].y);

  return DlRect::MakeLTRB(left, top, right, bottom);
}

TEST(DlVertexPainter, DrawRectIntoVertices) {
  std::vector<DlPoint> point_storage(12);
  std::vector<DlColor> color_storage(12);
  auto painter = DlVertexPainter(point_storage, color_storage);

  // Paint a red rectangle.
  painter.DrawRect(DlRect::MakeLTRB(0, 0, 10, 10), DlColor::kRed());

  // Paint a blue rectangle.
  painter.DrawRect(DlRect::MakeLTRB(10, 10, 20, 20), DlColor::kBlue());

  // Convert the buffered vertices into a |DlVertices| object.
  auto vertices = painter.IntoVertices(DlRect::MakeLTRB(0, 0, 20, 20));

  // Verify the vertices.
  EXPECT_EQ(vertices->mode(), DlVertexMode::kTriangles);
  EXPECT_EQ(vertices->vertex_count(), 3 * 2 * 2);

  auto const points = vertices->vertex_data();

  {
    // Extract the first 6 vertices (first rectangle).
    DlPoint first_rect_vertices[6];
    std::copy(points, points + 6, first_rect_vertices);
    EXPECT_EQ(MakeRectFromVertices(first_rect_vertices),
              DlRect::MakeLTRB(0, 0, 10, 10));
  }

  {
    // Extract the next 6 vertices (second rectangle).
    DlPoint second_rect_vertices[6];
    std::copy(points + 6, points + 12, second_rect_vertices);
    EXPECT_EQ(MakeRectFromVertices(second_rect_vertices),
              DlRect::MakeLTRB(10, 10, 20, 20));
  }

  // Verify the colors (first 6 vertices are red, next 6 are blue).
  auto const colors = vertices->colors();
  EXPECT_EQ(colors[0], DlColor::kRed());
  EXPECT_EQ(colors[1], DlColor::kRed());
  EXPECT_EQ(colors[2], DlColor::kRed());
  EXPECT_EQ(colors[3], DlColor::kRed());
  EXPECT_EQ(colors[4], DlColor::kRed());
  EXPECT_EQ(colors[5], DlColor::kRed());

  EXPECT_EQ(colors[6], DlColor::kBlue());
  EXPECT_EQ(colors[7], DlColor::kBlue());
  EXPECT_EQ(colors[8], DlColor::kBlue());
  EXPECT_EQ(colors[9], DlColor::kBlue());
  EXPECT_EQ(colors[10], DlColor::kBlue());
  EXPECT_EQ(colors[11], DlColor::kBlue());
}

}  // namespace testing
}  // namespace appcode
