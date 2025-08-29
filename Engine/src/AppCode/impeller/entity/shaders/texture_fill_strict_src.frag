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

#include <impeller/constants.glsl>
#include <impeller/types.glsl>

uniform f16sampler2D texture_sampler;

uniform FragInfo {
  vec4 source_rect;
  float alpha;
}
frag_info;

in highp vec2 v_texture_coords;

out f16vec4 frag_color;

void main() {
  vec2 texture_coords = vec2(clamp(v_texture_coords.x, frag_info.source_rect.x,
                                   frag_info.source_rect.z),
                             clamp(v_texture_coords.y, frag_info.source_rect.y,
                                   frag_info.source_rect.w));
  f16vec4 sampled =
      texture(texture_sampler, texture_coords, float16_t(kDefaultMipBias));
  frag_color = sampled * float16_t(frag_info.alpha);
}
