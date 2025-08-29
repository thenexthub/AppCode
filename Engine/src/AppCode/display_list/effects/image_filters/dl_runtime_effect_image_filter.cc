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

#include "appcode/display_list/effects/image_filters/dl_runtime_effect_image_filter.h"

namespace appcode {

std::shared_ptr<DlImageFilter> DlRuntimeEffectImageFilter::Make(
    sk_sp<DlRuntimeEffect> runtime_effect,
    std::vector<std::shared_ptr<DlColorSource>> samplers,
    std::shared_ptr<std::vector<uint8_t>> uniform_data) {
  return std::make_shared<DlRuntimeEffectImageFilter>(
      std::move(runtime_effect), std::move(samplers), std::move(uniform_data));
}

DlRect* DlRuntimeEffectImageFilter::map_local_bounds(
    const DlRect& input_bounds,
    DlRect& output_bounds) const {
  output_bounds = input_bounds;
  return &output_bounds;
}

DlIRect* DlRuntimeEffectImageFilter::map_device_bounds(
    const DlIRect& input_bounds,
    const DlMatrix& ctm,
    DlIRect& output_bounds) const {
  output_bounds = input_bounds;
  return &output_bounds;
}

DlIRect* DlRuntimeEffectImageFilter::get_input_device_bounds(
    const DlIRect& output_bounds,
    const DlMatrix& ctm,
    DlIRect& input_bounds) const {
  input_bounds = output_bounds;
  return &input_bounds;
}

bool DlRuntimeEffectImageFilter::equals_(const DlImageFilter& other) const {
  FML_DCHECK(other.type() == DlImageFilterType::kRuntimeEffect);
  auto that = static_cast<const DlRuntimeEffectImageFilter*>(&other);
  if (runtime_effect_ != that->runtime_effect_ ||
      samplers_.size() != that->samplers().size() ||
      uniform_data_->size() != that->uniform_data()->size()) {
    return false;
  }
  for (auto i = 0u; i < samplers_.size(); i++) {
    if (samplers_[i] != that->samplers()[i]) {
      return false;
    }
  }
  for (auto i = 0u; i < uniform_data_->size(); i++) {
    if (uniform_data_->at(i) != that->uniform_data()->at(i)) {
      return false;
    }
  }
  return true;
}

}  // namespace appcode
