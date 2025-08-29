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

#include <impeller/texture.glsl>
#include <impeller/types.glsl>

layout(constant_id = 0) const float supports_decal = 1.0;

uniform f16sampler2D texture_sampler;

uniform FragInfo {
  float x_tile_mode;
  float y_tile_mode;
  float alpha;
}
frag_info;

in highp vec2 v_texture_coords;

out f16vec4 frag_color;

void main() {
  if (supports_decal == 1.0) {
    frag_color = texture(texture_sampler,   // sampler
                         v_texture_coords,  // texture coordinates
                         float16_t(kDefaultMipBias)) *
                 float16_t(frag_info.alpha);
  } else {
    frag_color = IPHalfSampleWithTileMode(
                     texture_sampler,                   // sampler
                     v_texture_coords,                  // texture coordinates
                     float16_t(frag_info.x_tile_mode),  // x tile mode
                     float16_t(frag_info.y_tile_mode)   // y tile mode
                     ) *
                 float16_t(frag_info.alpha);
  }
}
