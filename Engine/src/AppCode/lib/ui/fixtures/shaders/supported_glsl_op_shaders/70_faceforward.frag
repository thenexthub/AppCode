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
  fragColor =
      vec4(0.0,
           // dot product of incident vector (2nd param) and reference vector
           // (3rd param) is non-negative, so the negated first param is
           // returned, and its first value is 1.0.
           faceforward(vec2(-a, 5.0), vec2(1.0, 2.0), vec2(3.0, 4.0))[0], 0.0,
           // dot product of incident vector (2nd param) and reference vector
           // (3rd param) is negative, so the original first param is returned,
           // and its second value is 5.0, so subtract 4.0 to get 1.0.
           faceforward(vec2(a, 5.0), vec2(1.0, -2.0), vec2(3.0, 4.0))[1] - 4.0);
}
