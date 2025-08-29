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
#include <impeller/constants.glsl>
#include <impeller/texture.glsl>
#include <impeller/types.glsl>

uniform sampler2D texture_sampler;

uniform FragInfo {
  highp vec2 center;
  float bias;
  float scale;
  float tile_mode;
  vec4 decal_border_color;
  float texture_sampler_y_coord_scale;
  float alpha;
  vec2 half_texel;
}
frag_info;

highp in vec2 v_position;

out vec4 frag_color;

void main() {
  vec2 coord = v_position - frag_info.center;
  float angle = atan(-coord.y, -coord.x);

  float t = (angle * k1Over2Pi + 0.5 + frag_info.bias) * frag_info.scale;
  frag_color =
      IPSampleLinearWithTileMode(texture_sampler,                          //
                                 vec2(t, 0.5),                             //
                                 frag_info.texture_sampler_y_coord_scale,  //
                                 frag_info.half_texel,                     //
                                 frag_info.tile_mode,                      //
                                 frag_info.decal_border_color);
  frag_color = IPPremultiply(frag_color) * frag_info.alpha;
}
