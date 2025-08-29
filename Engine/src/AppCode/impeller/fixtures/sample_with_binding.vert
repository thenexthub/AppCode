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

#include "types.h"

layout(set = 3, binding = 17) uniform UniformBufferObject {
  Uniforms uniforms;
}
ubo;

uniform sampler2D world;

in vec2 inPosition;
in vec3 inPosition22;
in vec4 inAnotherPosition;
in float stuff;

out vec4 outStuff;

void main() {
  gl_Position = ubo.uniforms.projection * ubo.uniforms.view *
                ubo.uniforms.model * vec4(inPosition22, 1.0) *
                inAnotherPosition;
  outStuff = texture(world, inPosition);
}
