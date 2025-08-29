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

in vec3 vertex_position;
in vec2 texture_coordinates;

out vec2 interpolated_texture_coordinates;

void main() {
  gl_Position = uniform_buffer.mvp * vec4(vertex_position, 1.0);
  interpolated_texture_coordinates = texture_coordinates;
}
