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

uniform FrameInfo {
  mat4 mvp;
  vec2 atlas_size;
  vec4 text_color;
}
frame_info;

in vec2 unit_vertex;
in mat4
    glyph_position;  // <--- Causes multiple slots to be used and is a failure.
in vec2 destination_size;
in vec2 source_position;
in vec2 source_glyph_size;

out vec2 v_unit_vertex;
out vec2 v_source_position;
out vec2 v_source_glyph_size;
out vec2 v_atlas_size;
out vec4 v_text_color;

void main() {
  gl_Position = frame_info.mvp * glyph_position *
                vec4(unit_vertex.x * destination_size.x,
                     unit_vertex.y * destination_size.y, 0.0, 1.0);

  v_unit_vertex = unit_vertex;
  v_source_position = source_position;
  v_source_glyph_size = source_glyph_size;
  v_atlas_size = frame_info.atlas_size;
  v_text_color = frame_info.text_color;
}
