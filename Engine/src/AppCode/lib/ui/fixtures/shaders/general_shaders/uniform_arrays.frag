#version 100 core

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

layout(location = 0) out vec4 oColor;

layout(location = 1) uniform float floatArray[2];
layout(location = 3) uniform vec2 vec2Array[2];
layout(location = 7) uniform vec3 vec3Array[2];
layout(location = 13) uniform mat2 mat2Array[2];

void main() {
  vec4 badColor = vec4(1.0, 0, 0, 1.0);
  vec4 goodColor = vec4(0, 1.0, 0, 1.0);

  // The test populates the uniforms with strictly increasing values, so if
  // out-of-order values are read out of the uniforms, then the bad color that
  // causes the test to fail is returned.
  if (floatArray[0] >= floatArray[1] || floatArray[1] >= vec2Array[0].x ||
      vec2Array[0].x >= vec2Array[0].y || vec2Array[0].y >= vec2Array[1].x ||
      vec2Array[1].x >= vec2Array[1].y || vec2Array[1].y >= vec3Array[0].x ||
      vec3Array[0].x >= vec3Array[0].y || vec3Array[0].y >= vec3Array[0].z ||
      vec3Array[0].z >= vec3Array[1].x || vec3Array[1].x >= vec3Array[1].y ||
      vec3Array[1].y >= vec3Array[1].z ||
      vec3Array[1].z >= mat2Array[0][0][0] ||
      mat2Array[0][0][0] >= mat2Array[0][0][1] ||
      mat2Array[0][0][1] >= mat2Array[0][1][0] ||
      mat2Array[0][1][0] >= mat2Array[0][1][1] ||
      mat2Array[0][1][1] >= mat2Array[1][0][0] ||
      mat2Array[1][0][0] >= mat2Array[1][0][1] ||
      mat2Array[1][0][1] >= mat2Array[1][1][0] ||
      mat2Array[1][1][0] >= mat2Array[1][1][1]) {
    oColor = badColor;
  } else {
    oColor = goodColor;
  }
}
