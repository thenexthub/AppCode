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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_GRADIENT_GENERATOR_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_GRADIENT_GENERATOR_H_

#include <memory>
#include <vector>

#include "appcode/impeller/core/texture.h"
#include "impeller/core/shader_types.h"
#include "impeller/geometry/color.h"
#include "impeller/geometry/gradient.h"

namespace impeller {

class Context;

/**
 * @brief Create a host visible texture that contains the gradient defined
 * by the provided gradient data.
 */
std::shared_ptr<Texture> CreateGradientTexture(
    const GradientData& gradient_data,
    const std::shared_ptr<impeller::Context>& context);

struct StopData {
  Color color;
  Scalar stop;
  Scalar inverse_delta;
  Padding<8> _padding_;
};

static_assert(sizeof(StopData) == 32);

/**
 * @brief Populate a vector with the color and stop data for a gradient
 *
 * @param colors
 * @param stops
 * @return StopData
 */
std::vector<StopData> CreateGradientColors(const std::vector<Color>& colors,
                                           const std::vector<Scalar>& stops);

static constexpr uint32_t kMaxUniformGradientStops = 256u;

/**
 * @brief Populate 2 arrays with the colors and stop data for a gradient
 *
 * The color data is simply converted to a vec4 format, but the stop data
 * is both turned into pairs of {t, inverse_delta} information and also
 * stops are themselves paired up into a vec4 format for efficient packing
 * in the uniform data.
 *
 * @param colors colors from gradient
 * @param stops  stops from gradient
 * @param frag_info_colors colors for fragment shader in vec4 format
 * @param frag_info_stop_pairs pairs of stop data for shader in vec4 format
 * @return count of colors stored
 */
int PopulateUniformGradientColors(
    const std::vector<Color>& colors,
    const std::vector<Scalar>& stops,
    Vector4 frag_info_colors[kMaxUniformGradientStops],
    Vector4 frag_info_stop_pairs[kMaxUniformGradientStops / 2]);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_GRADIENT_GENERATOR_H_
