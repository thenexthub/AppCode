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

#include "impeller/entity/contents/gradient_generator.h"

#include "appcode/fml/logging.h"
#include "impeller/base/strings.h"
#include "impeller/core/device_buffer.h"
#include "impeller/core/formats.h"
#include "impeller/core/texture.h"
#include "impeller/renderer/context.h"
#include "impeller/renderer/texture_util.h"

namespace impeller {

std::shared_ptr<Texture> CreateGradientTexture(
    const GradientData& gradient_data,
    const std::shared_ptr<impeller::Context>& context) {
  if (gradient_data.texture_size == 0) {
    FML_DLOG(ERROR) << "Invalid gradient data.";
    return nullptr;
  }

  impeller::TextureDescriptor texture_descriptor;
  texture_descriptor.storage_mode = impeller::StorageMode::kHostVisible;
  texture_descriptor.format = PixelFormat::kR8G8B8A8UNormInt;
  texture_descriptor.size = {gradient_data.texture_size, 1};

  return CreateTexture(texture_descriptor, gradient_data.color_bytes, context,
                       "Gradient");
}

std::vector<StopData> CreateGradientColors(const std::vector<Color>& colors,
                                           const std::vector<Scalar>& stops) {
  FML_DCHECK(stops.size() == colors.size());

  std::vector<StopData> result;
  result.reserve(stops.size());
  Scalar last_stop = 0;
  for (auto i = 0u; i < stops.size(); i++) {
    Scalar delta = stops[i] - last_stop;
    Scalar inverse_delta = delta == 0.0f ? 0.0 : 1.0 / delta;
    result.emplace_back(StopData{
        .color = colors[i], .stop = stops[i], .inverse_delta = inverse_delta});
    last_stop = stops[i];
  }
  return result;
}

int PopulateUniformGradientColors(
    const std::vector<Color>& colors,
    const std::vector<Scalar>& stops,
    Vector4 frag_info_colors[kMaxUniformGradientStops],
    Vector4 frag_info_stop_pairs[kMaxUniformGradientStops / 2]) {
  FML_DCHECK(stops.size() == colors.size());

  Scalar last_stop = 0;
  int index = 0;
  for (auto i = 0u; i < stops.size() && i < kMaxUniformGradientStops; i++) {
    Scalar cur_stop = stops[i];
    Scalar delta = cur_stop - last_stop;
    Scalar inverse_delta = delta == 0.0f ? 0.0 : 1.0 / delta;
    frag_info_colors[index] = colors[i];
    if ((i & 1) == 0) {
      frag_info_stop_pairs[index / 2].x = cur_stop;
      frag_info_stop_pairs[index / 2].y = inverse_delta;
    } else {
      frag_info_stop_pairs[index / 2].z = cur_stop;
      frag_info_stop_pairs[index / 2].w = inverse_delta;
    }
    last_stop = cur_stop;
    index++;
  }
  return index;
}

}  // namespace impeller
