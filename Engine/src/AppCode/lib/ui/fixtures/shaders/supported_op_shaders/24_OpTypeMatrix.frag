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
  mat2 zeros = mat2(0.0);
  mat3 ones = mat3(a);
  mat4 identity = mat4(a, 0.0, 0.0, 0.0, 0.0, a, 0.0, 0.0, 0.0, 0.0, a, 0.0,
                       0.0, 0.0, 0.0, a);
  fragColor = vec4(zeros[1][1], ones[2][2], identity[3][1], identity[3][3]);
}
