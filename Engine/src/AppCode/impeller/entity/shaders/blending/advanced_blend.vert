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

#include <impeller/conversions.glsl>
#include <impeller/types.glsl>

uniform FrameInfo {
  mat4 mvp;
  float dst_y_coord_scale;
  float src_y_coord_scale;
}
frame_info;

in vec2 vertices;
in vec2 dst_texture_coords;
in vec2 src_texture_coords;

out vec2 v_dst_texture_coords;
out vec2 v_src_texture_coords;

void main() {
  gl_Position = frame_info.mvp * vec4(vertices, 0.0, 1.0);
  v_dst_texture_coords =
      IPRemapCoords(dst_texture_coords, frame_info.dst_y_coord_scale);
  v_src_texture_coords =
      IPRemapCoords(src_texture_coords, frame_info.src_y_coord_scale);
}
