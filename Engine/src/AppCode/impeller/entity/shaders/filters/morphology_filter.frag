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

layout(constant_id = 0) const float supports_decal = 1.0;

#include <impeller/constants.glsl>
#include <impeller/texture.glsl>
#include <impeller/types.glsl>

// These values must correspond to the order of the items in the
// 'FilterContents::MorphType' enum class.
// const float kMorphTypeDilate = 0.0;
// const float kMorphTypeErode = 1.0;

uniform f16sampler2D texture_sampler;

uniform FragInfo {
  f16vec2 uv_offset;
  float16_t radius;
  float morph_type;
  float supports_decal_sampler_address_mode;
}
frag_info;

in highp vec2 v_texture_coords;

out f16vec4 frag_color;

void main() {
  f16vec4 result = frag_info.morph_type == /*kMorphTypeDilate*/ 0.0
                       ? f16vec4(0.0)
                       : f16vec4(1.0);
  for (float16_t i = -frag_info.radius; i <= frag_info.radius; i++) {
    vec2 texture_coords = v_texture_coords + frag_info.uv_offset * i;

    f16vec4 color;
    if (supports_decal == 1) {
      color = texture(texture_sampler, texture_coords);
    } else {
      color = IPHalfSampleDecal(texture_sampler, texture_coords);
    }

    if (frag_info.morph_type == /*kMorphTypeDilate*/ 0.0) {
      result = max(color, result);
    } else {
      result = min(color, result);
    }
  }

  frag_color = result;
}
