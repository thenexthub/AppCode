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

// For a given incident vector I and surface normal N reflect returns the
// reflection direction calculated as I - 2.0 * dot(N, I) * N.
void main() {
  // To get [0.0, 1.0] as the output, choose [0.6, 0.8] as N, and solve for I.
  // Since the reflection is symmetric:
  // I’ = reflect(I)
  // I’ = I - 2 dot(N, I) N
  // I = I’ - 2 dot(N, I’) N
  // N = [0.6, 0.8]
  // I’ = [0, 1]
  // I = [0, 1] - 2 * 0.8 [0.6, 0.8]
  // I = [-0.96, -0.28]
  fragColor =
      vec4(reflect(vec2(a * -0.96, -0.28), vec2(0.6, 0.8))[0],
           reflect(vec2(a * -0.96, -0.28), vec2(0.6, 0.8))[1], 0.0, 1.0);
}
