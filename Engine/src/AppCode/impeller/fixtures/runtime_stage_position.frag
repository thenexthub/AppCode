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

#include <flutter/runtime_effect.glsl>

uniform vec4 ltrb;

out vec4 frag_color;

// Output solid red if frag position is within LTRB rectangle.
void main() {
  if (FlutterFragCoord().x >= ltrb.x && FlutterFragCoord().x <= ltrb.z &&
      FlutterFragCoord().y >= ltrb.y && FlutterFragCoord().y <= ltrb.w) {
    frag_color = vec4(1.0, 0.0, 0.0, 1.0);
  } else {
    frag_color = vec4(0.0);
  }
}
