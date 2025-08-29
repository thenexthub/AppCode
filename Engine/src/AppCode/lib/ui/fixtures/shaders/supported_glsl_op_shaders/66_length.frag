#version 320 es

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

precision highp float;

layout(location = 0) out vec4 fragColor;

layout(location = 0) uniform float a;

void main() {
  fragColor = vec4(
      // length of a zero vector is 0.0
      length(vec3(a - 1.0, 0.0, 0.0)),
      // sqrt(3.0^2.0 + 4.0^2.0) - 4.0 = 5.0 - 4.0 = 1.0
      length(vec2(a * 3.0, 4.0)) - 4.0, 0.0,
      // sqrt(4.0^2.0 + (-4.0)^2.0 + (-4.0)^2.0) + 4.0^2.0) - 7.0 = sqrt(16.0
      // + 16.0 + 16.0 + 16.0) - 7.0 = sqrt(64.0) - 7.0 = 8.0 - 7.0 = 1.0
      length(vec4(a * 4.0, -4.0, -4.0, 4.0)) - 7.0);
}
