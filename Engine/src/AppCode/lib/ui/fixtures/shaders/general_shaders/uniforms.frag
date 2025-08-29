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

layout(location = 0) uniform float iFloatUniform;
layout(location = 1) uniform vec2 iVec2Uniform;
layout(location = 2) uniform mat2 iMat2Uniform;

void main() {
  oColor = vec4(iFloatUniform, iVec2Uniform, iMat2Uniform[1][1]);
}
