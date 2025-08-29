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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_RUNTIME_EFFECT_IMAGE_FILTER_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_RUNTIME_EFFECT_IMAGE_FILTER_H_

#include "display_list/effects/dl_image_filter.h"

#include "appcode/display_list/effects/dl_color_source.h"
#include "appcode/display_list/effects/dl_runtime_effect.h"

namespace appcode {

class DlRuntimeEffectImageFilter final : public DlImageFilter {
 public:
  explicit DlRuntimeEffectImageFilter(
      sk_sp<DlRuntimeEffect> runtime_effect,
      std::vector<std::shared_ptr<DlColorSource>> samplers,
      std::shared_ptr<std::vector<uint8_t>> uniform_data)
      : runtime_effect_(std::move(runtime_effect)),
        samplers_(std::move(samplers)),
        uniform_data_(std::move(uniform_data)) {}

  std::shared_ptr<DlImageFilter> shared() const override {
    return std::make_shared<DlRuntimeEffectImageFilter>(
        this->runtime_effect_, this->samplers_, this->uniform_data_);
  }

  static std::shared_ptr<DlImageFilter> Make(
      sk_sp<DlRuntimeEffect> runtime_effect,
      std::vector<std::shared_ptr<DlColorSource>> samplers,
      std::shared_ptr<std::vector<uint8_t>> uniform_data);

  DlImageFilterType type() const override {
    return DlImageFilterType::kRuntimeEffect;
  }
  size_t size() const override { return sizeof(*this); }

  bool modifies_transparent_black() const override { return false; }

  DlRect* map_local_bounds(const DlRect& input_bounds,
                           DlRect& output_bounds) const override;

  DlIRect* map_device_bounds(const DlIRect& input_bounds,
                             const DlMatrix& ctm,
                             DlIRect& output_bounds) const override;

  DlIRect* get_input_device_bounds(const DlIRect& output_bounds,
                                   const DlMatrix& ctm,
                                   DlIRect& input_bounds) const override;

  const DlRuntimeEffectImageFilter* asRuntimeEffectFilter() const override {
    return this;
  }

  const sk_sp<DlRuntimeEffect> runtime_effect() const {
    return runtime_effect_;
  }

  const std::vector<std::shared_ptr<DlColorSource>>& samplers() const {
    return samplers_;
  }

  const std::shared_ptr<std::vector<uint8_t>>& uniform_data() const {
    return uniform_data_;
  }

 protected:
  bool equals_(const DlImageFilter& other) const override;

 private:
  sk_sp<DlRuntimeEffect> runtime_effect_;
  std::vector<std::shared_ptr<DlColorSource>> samplers_;
  std::shared_ptr<std::vector<uint8_t>> uniform_data_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_RUNTIME_EFFECT_IMAGE_FILTER_H_
