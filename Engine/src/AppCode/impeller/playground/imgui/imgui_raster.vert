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

uniform UniformBuffer {
  mat4 mvp;
}
uniform_buffer;

in vec2 vertex_position;
in vec2 texture_coordinates;
in vec4 vertex_color;

out vec2 frag_texture_coordinates;
out vec4 frag_vertex_color;

void main() {
  gl_Position = uniform_buffer.mvp * vec4(vertex_position.xy, 0.0, 1.0);
  frag_texture_coordinates = texture_coordinates;
  frag_vertex_color = vertex_color;
}
