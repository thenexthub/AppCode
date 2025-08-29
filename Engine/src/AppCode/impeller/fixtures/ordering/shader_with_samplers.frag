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

// Declare samplers in different order than usage.
uniform sampler2D textureA;
uniform sampler2D textureB;

out vec4 frag_color;

void main() {
  vec4 sample_1 = texture(textureB, vec2(1.0));
  vec4 sample_2 = texture(textureA, vec2(1.0));
  frag_color = sample_1 + sample_2;
}
