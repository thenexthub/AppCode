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

uniform VertInfo {  // 128 bytes (alignment = NPOT(largest member))
  mat4 mvp;         // offset 0 bytes, size 64 bytes
  vec4 color;       // offset 64 bytes, size 16 bytes
}
vert_info;

in vec2 position;
out vec4 v_color;

void main() {
  v_color = vert_info.color;
  gl_Position = vert_info.mvp * vec4(position, 0.0, 1.0);
}
