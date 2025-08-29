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

#include <impeller/types.glsl>

uniform FrameInfo {
  mat4 mvp;
}
frame_info;

in vec2 position;

in vec3 e0;
in vec3 e1;
in vec3 e2;
in vec3 e3;

out vec2 v_position;
out vec3 v_e0;
out vec3 v_e1;
out vec3 v_e2;
out vec3 v_e3;

void main() {
  gl_Position = frame_info.mvp * vec4(position, 0.0, 1.0);
  v_position = position;
  v_e0 = e0;
  v_e1 = e1;
  v_e2 = e2;
  v_e3 = e3;
}
