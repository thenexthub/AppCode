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

in vec4 v_color;
in vec4 v_color2;

out vec4 frag_color;

uniform FragInfo {
  float time;
}
frag_info;

void main() {
  float floor = floor(frag_info.time);
  float fract = frag_info.time - floor;
  if (mod(int(floor), 2) == 0) {
    fract = 1.0 - fract;
  }
  frag_color = mix(v_color, v_color2, fract);
}
