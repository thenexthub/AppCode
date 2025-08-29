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

#ifndef APPCODE_IMPELLER_PLAYGROUND_WIDGETS_H_
#define APPCODE_IMPELLER_PLAYGROUND_WIDGETS_H_

#include <optional>
#include <tuple>

#include "impeller/base/strings.h"
#include "impeller/geometry/color.h"
#include "impeller/geometry/point.h"
#include "third_party/imgui/imgui.h"

namespace impeller {
struct PlaygroundPoint {
  PlaygroundPoint(Point default_position, Scalar p_radius, Color p_color)
      : position(default_position),
        reset_position(default_position),
        radius(p_radius),
        color(p_color) {}
  Point position;
  Point reset_position;
  bool dragging = false;
  std::optional<Point> prev_mouse_pos;
  Scalar radius;
  Color color;
  PlaygroundPoint(const PlaygroundPoint&) = delete;
  PlaygroundPoint(PlaygroundPoint&&) = delete;
  PlaygroundPoint& operator=(const PlaygroundPoint&) = delete;
};

Point DrawPlaygroundPoint(PlaygroundPoint& point);

std::tuple<Point, Point> DrawPlaygroundLine(PlaygroundPoint& point_a,
                                            PlaygroundPoint& point_b);
}  // namespace impeller
#endif  // APPCODE_IMPELLER_PLAYGROUND_WIDGETS_H_
