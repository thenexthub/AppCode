#version 450

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
#include <impeller/color.glsl>
#include <impeller/texture.glsl>
#include <impeller/types.glsl>
#include "blend_select.glsl"

// Warning: if any of the constant values or layouts are changed in this
// file, then the hard-coded constant value in
// impeller/renderer/backend/vulkan/binding_helpers_vk.cc
layout(constant_id = 0) const float blend_type = 0;
layout(constant_id = 1) const float supports_decal = 1;

layout(input_attachment_index = 0) uniform subpassInputMS uSub;

vec4 ReadDestination() {
  return (subpassLoad(uSub, 0) + subpassLoad(uSub, 1) + subpassLoad(uSub, 2) +
          subpassLoad(uSub, 3)) /
         vec4(4.0);
}

uniform sampler2D texture_sampler_src;

uniform FragInfo {
  float16_t src_input_alpha;
  float16_t dst_input_alpha;
}
frag_info;

in vec2 v_src_texture_coords;

out vec4 frag_color;

vec4 Sample(sampler2D texture_sampler, vec2 texture_coords) {
  if (supports_decal > 1) {
    return texture(texture_sampler, texture_coords);
  }
  return IPSampleDecal(texture_sampler, texture_coords);
}

void main() {
  f16vec4 dst = IPHalfUnpremultiply(f16vec4(ReadDestination()));
  dst.a *= frag_info.dst_input_alpha;
  f16vec4 src = IPHalfUnpremultiply(
      f16vec4(Sample(texture_sampler_src,  // sampler
                     v_src_texture_coords  // texture coordinates
                     )));
  src.a *= frag_info.src_input_alpha;

  f16vec3 blend_result = AdvancedBlend(dst.rgb, src.rgb, int(blend_type));

  frag_color = IPApplyBlendedColor(dst, src, blend_result);
}
