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
  float current_time;
  vec2 cursor_position;
  vec2 window_size;
}
frame_info;

in vec2 interpolated_texture_coordinates;

out vec4 frag_color;

uniform sampler2D contents1;
uniform sampler2D contents2;

void main() {
  vec4 tex1 = texture(contents1, interpolated_texture_coordinates);
  vec4 tex2 = texture(contents2, interpolated_texture_coordinates);
  frag_color = mix(
      tex1, tex2,
      clamp(frame_info.cursor_position.x / frame_info.window_size.x, 0.0, 1.0));
}
