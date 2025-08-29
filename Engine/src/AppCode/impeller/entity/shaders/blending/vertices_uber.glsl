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

#include <impeller/blending.glsl>
#include <impeller/texture.glsl>
#include <impeller/types.glsl>
#include "blend_select.glsl"

layout(constant_id = 0) const float supports_decal = 1.0;

uniform FragInfo {
  float16_t alpha;
  float16_t blend_mode;
  float tmx;
  float tmy;
}
frag_info;

uniform f16sampler2D texture_sampler;

in mediump vec2 v_texture_coords;
in mediump f16vec4 v_color;

out f16vec4 frag_color;

f16vec4 Sample(f16sampler2D texture_sampler,
               vec2 texture_coords,
               float tmx,
               float tmy) {
  if (supports_decal > 0.0) {
    return texture(texture_sampler, texture_coords);
  }
  return IPHalfSampleWithTileMode(texture_sampler, texture_coords, tmx, tmy);
}
