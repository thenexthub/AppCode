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

#include <impeller/external_texture_oes.glsl>

uniform sampler2D SAMPLER_EXTERNAL_OES_texture_sampler;

uniform FragInfo {
  float x_tile_mode;
  float y_tile_mode;
  float alpha;
}
frag_info;

in highp vec2 v_texture_coords;

out vec4 frag_color;

void main() {
  frag_color =
      IPSampleWithTileModeOES(SAMPLER_EXTERNAL_OES_texture_sampler,  // sampler
                              v_texture_coords,       // texture coordinates
                              frag_info.x_tile_mode,  // x tile mode
                              frag_info.y_tile_mode   // y tile mode
                              ) *
      frag_info.alpha;
}
