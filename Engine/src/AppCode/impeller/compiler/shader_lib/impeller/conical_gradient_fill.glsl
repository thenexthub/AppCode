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

#ifndef CONICAL_GRADIENT_UNIFORM_FILL_GLSL_
#define CONICAL_GRADIENT_UNIFORM_FILL_GLSL_

#include <impeller/texture.glsl>

uniform sampler2D texture_sampler;

uniform FragInfo {
  highp vec2 center;
  float radius;
  float tile_mode;
  vec4 decal_border_color;
  float texture_sampler_y_coord_scale;
  float alpha;
  vec2 half_texel;
  vec2 focus;
  float focus_radius;
}
frag_info;

vec4 DoConicalGradientTextureFill(vec2 res) {
  if (res.y < 0.0) {
    return vec4(0);
  }

  float t = res.x;
  vec4 result =
      IPSampleLinearWithTileMode(texture_sampler,                          //
                                 vec2(t, 0.5),                             //
                                 frag_info.texture_sampler_y_coord_scale,  //
                                 frag_info.half_texel,                     //
                                 frag_info.tile_mode,                      //
                                 frag_info.decal_border_color);
  result = IPPremultiply(result) * frag_info.alpha;
  return result;
}

#endif  // CONICAL_GRADIENT_UNIFORM_FILL_GLSL_
