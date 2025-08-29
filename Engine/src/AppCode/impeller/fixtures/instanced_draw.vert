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

#ifdef IMPELLER_TARGET_OPENGLES

void main() {
  // Instancing is not supported on legacy targets and test will be disabled.
}

#else  // IMPELLER_TARGET_OPENGLES

uniform FrameInfo {
  mat4 mvp;
}
frame_info;

readonly buffer InstanceInfo {
  vec4 colors[];
}
instance_info;

in vec2 vtx;

out vec4 v_color;

void main() {
  gl_Position =
      frame_info.mvp * vec4(vtx.x + 105.0 * gl_InstanceIndex,
                            vtx.y + 105.0 * gl_InstanceIndex, 0.0, 1.0);
  v_color = instance_info.colors[gl_InstanceIndex];
}

#endif  // IMPELLER_TARGET_OPENGLES
