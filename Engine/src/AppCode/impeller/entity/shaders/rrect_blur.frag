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

// The math for this shader was based on the work done in Raph Levien's blog
// post "Blurred rounded rectangles":
// https://web.archive.org/web/20231103044404/https://raphlinus.github.io/graphics/2020/04/21/blurred-rounded-rects.html

precision highp float;

#include <impeller/gaussian.glsl>
#include <impeller/math.glsl>
#include <impeller/types.glsl>

uniform FragInfo {
  f16vec4 color;
  vec2 center;
  vec2 adjust;
  float minEdge;
  float r1;
  float exponent;
  float sInv;
  float exponentInv;
  float scale;
}
frag_info;

in vec2 v_position;

out f16vec4 frag_color;

const float kTwoOverSqrtPi = 2.0 / sqrt(3.1415926);

float maxXY(vec2 v) {
  return max(v.x, v.y);
}

// use crate::math::compute_erf7;
float computeErf7(float x) {
  x *= kTwoOverSqrtPi;
  float xx = x * x;
  x = x + (0.24295 + (0.03395 + 0.0104 * xx) * xx) * (x * xx);
  return x / sqrt(1.0 + x * x);
}

// The length formula, but with an exponent other than 2
float powerDistance(vec2 p) {
  float xp = POW(p.x, frag_info.exponent);
  float yp = POW(p.y, frag_info.exponent);
  return POW(xp + yp, frag_info.exponentInv);
}

void main() {
  vec2 adjusted = abs(v_position - frag_info.center) - frag_info.adjust;

  float dPos = powerDistance(max(adjusted, 0.0));
  float dNeg = min(maxXY(adjusted), 0.0);
  float d = dPos + dNeg - frag_info.r1;
  float z =
      frag_info.scale * (computeErf7(frag_info.sInv * (frag_info.minEdge + d)) -
                         computeErf7(frag_info.sInv * d));

  frag_color = frag_info.color * float16_t(z);
}
