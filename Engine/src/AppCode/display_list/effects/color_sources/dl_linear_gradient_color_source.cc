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

#include "appcode/display_list/effects/color_sources/dl_linear_gradient_color_source.h"

namespace appcode {

DlLinearGradientColorSource::DlLinearGradientColorSource(
    const DlLinearGradientColorSource* source)
    : DlGradientColorSourceBase(source->stop_count(),
                                source->tile_mode(),
                                source->matrix_ptr()),
      start_point_(source->start_point()),
      end_point_(source->end_point()) {
  store_color_stops(this + 1, source->colors(), source->stops());
}

std::shared_ptr<DlColorSource> DlLinearGradientColorSource::shared() const {
  return MakeLinear(start_point_, end_point_, stop_count(), colors(), stops(),
                    tile_mode(), matrix_ptr());
}

bool DlLinearGradientColorSource::equals_(DlColorSource const& other) const {
  FML_DCHECK(other.type() == DlColorSourceType::kLinearGradient);
  auto that = static_cast<DlLinearGradientColorSource const*>(&other);
  return (start_point_ == that->start_point_ &&
          end_point_ == that->end_point_ && base_equals_(that));
}

}  // namespace appcode
