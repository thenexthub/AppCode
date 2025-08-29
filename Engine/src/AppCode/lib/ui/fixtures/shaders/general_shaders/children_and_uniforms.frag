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

layout(location = 0) out vec4 color;

layout(location = 0) uniform sampler2D child1;
layout(location = 1) uniform float a;
layout(location = 2) uniform sampler2D child2;
layout(location = 3) uniform float b;

void main() {
  // child1 is a 10x10 image where the left half is blue and the right
  // half is green, and b should be 1, so c1 should be vec4(0, 1, 0, 1)
  vec4 c1 = texture(child1, vec2(b, 0));

  // child2 only contains vec4(0, 1, 0, 1).
  vec4 c2 = texture(child2, vec2(0));

  color = c1 * c2;
}
