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

layout(location = 0) out vec4 oColor;

layout(location = 0) uniform float a;  // should be 1.0

float saturate(float x) {
  return clamp(x, 0.0, 1.0);
}

float addA(float x) {
  return x + a;
}

vec2 pairWithA(float x) {
  return vec2(x, a);
}

vec3 composedFunction(float x) {
  return vec3(addA(x), pairWithA(x));
}

float multiParam(float x, float y, float z) {
  return x * y * z * a;
}

void main() {
  float x = saturate(addA(0.0));          // x = 0 + 1;
  vec3 v3 = composedFunction(x);          // v3 = vec3(2, 1, 1);
  x = multiParam(v3.x, v3.y, v3.z);       // x = 2 * 1 * 1 * 1;
  oColor = vec4(0.0, x / 2.0, 0.0, 1.0);  // vec4(0, 1, 0, 1);
}
