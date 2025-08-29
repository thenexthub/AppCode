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

#include "vertices_uber.glsl"

// A shader that implements the required src/dst blending for drawVertices and
// drawAtlas advanced blends without requiring an offscreen render pass. This is
// done in a single shader to reduce the permutations of PSO needed at runtime
// for rarely used features.
void main() {
  f16vec4 dst = IPHalfUnpremultiply(v_color);
  f16vec4 src = IPHalfUnpremultiply(
      Sample(texture_sampler, v_texture_coords, frag_info.tmx, frag_info.tmy));
  f16vec3 blend_result =
      AdvancedBlendHalf1(dst.rgb, src.rgb, int(frag_info.blend_mode - 14.0));
  frag_color = IPApplyBlendedColor(dst, src, blend_result);
  frag_color *= frag_info.alpha;
}
