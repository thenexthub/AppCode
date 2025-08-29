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

precision mediump float;

#include <impeller/color.glsl>
#include <impeller/dithering.glsl>
#include <impeller/types.glsl>

uniform FragInfo {
  float alpha;
}
frag_info;

in vec4 v_color;

out vec4 frag_color;

void main() {
  frag_color = IPPremultiply(v_color) * frag_info.alpha;
  // mod operator is not supported in GLES 2.0
#ifndef IMPELLER_TARGET_OPENGLES
  frag_color = IPOrderedDither8x8(frag_color, gl_FragCoord.xy);
#endif  // IMPELLER_TARGET_OPENGLES
}
