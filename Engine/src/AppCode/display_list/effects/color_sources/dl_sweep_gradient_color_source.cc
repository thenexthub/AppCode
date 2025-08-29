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

#include "appcode/display_list/effects/color_sources/dl_sweep_gradient_color_source.h"

namespace appcode {

DlSweepGradientColorSource::DlSweepGradientColorSource(
    const DlSweepGradientColorSource* source)
    : DlGradientColorSourceBase(source->stop_count(),
                                source->tile_mode(),
                                source->matrix_ptr()),
      center_(source->center()),
      start_(source->start()),
      end_(source->end()) {
  store_color_stops(this + 1, source->colors(), source->stops());
}

std::shared_ptr<DlColorSource> DlSweepGradientColorSource::shared() const {
  return MakeSweep(center_, start_, end_, stop_count(), colors(), stops(),
                   tile_mode(), matrix_ptr());
}

bool DlSweepGradientColorSource::equals_(DlColorSource const& other) const {
  FML_DCHECK(other.type() == DlColorSourceType::kSweepGradient);
  auto that = static_cast<DlSweepGradientColorSource const*>(&other);
  return (center_ == that->center_ && start_ == that->start_ &&
          end_ == that->end_ && base_equals_(that));
}

}  // namespace appcode
