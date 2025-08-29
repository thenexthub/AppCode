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

#include "impeller/playground/widgets.h"

namespace impeller {

Point DrawPlaygroundPoint(PlaygroundPoint& point) {
  if (ImGui::GetCurrentContext()) {
    impeller::Point mouse_pos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
    if (!point.prev_mouse_pos.has_value()) {
      point.prev_mouse_pos = mouse_pos;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_R)) {
      point.position = point.reset_position;
      point.dragging = false;
    }

    bool hovering =
        point.position.GetDistance(mouse_pos) < point.radius &&
        point.position.GetDistance(point.prev_mouse_pos.value()) < point.radius;
    if (!ImGui::IsMouseDown(0)) {
      point.dragging = false;
    } else if (hovering && ImGui::IsMouseClicked(0)) {
      point.dragging = true;
    }
    if (point.dragging) {
      point.position += mouse_pos - point.prev_mouse_pos.value();
    }
    ImGui::GetBackgroundDrawList()->AddCircleFilled(
        {point.position.x, point.position.y}, point.radius,
        ImColor(point.color.red, point.color.green, point.color.blue,
                (hovering || point.dragging) ? 0.6f : 0.3f));
    if (hovering || point.dragging) {
      ImGui::GetBackgroundDrawList()->AddText(
          {point.position.x - point.radius,
           point.position.y + point.radius + 10},
          ImColor(point.color.red, point.color.green, point.color.blue, 1.0f),
          impeller::SPrintF("x:%0.3f y:%0.3f", point.position.x,
                            point.position.y)
              .c_str());
    }
    point.prev_mouse_pos = mouse_pos;
  }
  return point.position;
}

std::tuple<Point, Point> DrawPlaygroundLine(PlaygroundPoint& point_a,
                                            PlaygroundPoint& point_b) {
  Point position_a = DrawPlaygroundPoint(point_a);
  Point position_b = DrawPlaygroundPoint(point_b);

  if (ImGui::GetCurrentContext()) {
    auto dir = (position_b - position_a).Normalize() * point_a.radius;
    auto line_a = position_a + dir;
    auto line_b = position_b - dir;
    ImGui::GetBackgroundDrawList()->AddLine(
        {line_a.x, line_a.y}, {line_b.x, line_b.y},
        ImColor(point_b.color.red, point_b.color.green, point_b.color.blue,
                0.3f));
  }
  return std::make_tuple(position_a, position_b);
}
//

}  // namespace impeller
