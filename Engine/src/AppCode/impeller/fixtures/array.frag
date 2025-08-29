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

uniform FragInfo {
  vec2 circle_positions[4];
  vec4 colors[4];
}
frag_info;

in vec2 v_position;

out vec4 frag_color;

float SphereDistance(vec2 position, float radius) {
  return length(v_position - position) - radius;
}

void main() {
  for (int i = 0; i < 4; i++) {
    if (SphereDistance(frag_info.circle_positions[i].xy, 20) <= 0) {
      frag_color = frag_info.colors[i];
      return;
    }
  }
  frag_color = vec4(0);
}
