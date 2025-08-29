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
  float depth;
}
frame_info;

in vec2 position;

void main() {
  gl_Position = frame_info.mvp * vec4(position, 0.0, 1.0);
  // We can just absorb W and override the depth value here because we don't
  // need to worry about perspective correcting any vertex attributes when
  // drawing clips.
  gl_Position /= gl_Position.w;
  gl_Position.z = frame_info.depth;
}
