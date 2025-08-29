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
#include <impeller/texture.glsl>
#include <impeller/types.glsl>

uniform f16sampler2D y_texture;
uniform f16sampler2D uv_texture;

// These values must correspond to the order of the items in the
// 'YUVColorSpace' enum class.
// const float kBT601LimitedRange = 0.0;
// const float kBT601FullRange = 1.0;

uniform FragInfo {
  mat4 matrix;
  float yuv_color_space;
}
frag_info;

in highp vec2 v_texture_coords;

out f16vec4 frag_color;

void main() {
  f16vec3 yuv;
  f16vec3 yuv_offset = f16vec3(0.0hf, 0.5hf, 0.5hf);
  if (frag_info.yuv_color_space == /*kBT601LimitedRange*/ 0.0) {
    yuv_offset.x = 16.0hf / 255.0hf;
  }

  yuv.x = texture(y_texture, v_texture_coords).r;
  yuv.yz = texture(uv_texture, v_texture_coords).rg;
  frag_color = f16mat4(frag_info.matrix) * f16vec4(yuv - yuv_offset, 1.0hf);
}
