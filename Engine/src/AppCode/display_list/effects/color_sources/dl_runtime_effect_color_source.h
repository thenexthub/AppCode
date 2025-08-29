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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_RUNTIME_EFFECT_COLOR_SOURCE_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_RUNTIME_EFFECT_COLOR_SOURCE_H_

#include "appcode/display_list/effects/dl_color_source.h"

namespace appcode {

class DlRuntimeEffectColorSource final : public DlColorSource {
 public:
  DlRuntimeEffectColorSource(
      sk_sp<DlRuntimeEffect> runtime_effect,
      std::vector<std::shared_ptr<DlColorSource>> samplers,
      std::shared_ptr<std::vector<uint8_t>> uniform_data);

  bool isUIThreadSafe() const override;

  const DlRuntimeEffectColorSource* asRuntimeEffect() const override {
    return this;
  }

  std::shared_ptr<DlColorSource> shared() const override;

  DlColorSourceType type() const override {
    return DlColorSourceType::kRuntimeEffect;
  }
  size_t size() const override { return sizeof(*this); }

  bool is_opaque() const override { return false; }

  const sk_sp<DlRuntimeEffect> runtime_effect() const {
    return runtime_effect_;
  }
  const std::vector<std::shared_ptr<DlColorSource>> samplers() const {
    return samplers_;
  }
  const std::shared_ptr<std::vector<uint8_t>> uniform_data() const {
    return uniform_data_;
  }

 protected:
  bool equals_(DlColorSource const& other) const override;

 private:
  sk_sp<DlRuntimeEffect> runtime_effect_;
  std::vector<std::shared_ptr<DlColorSource>> samplers_;
  std::shared_ptr<std::vector<uint8_t>> uniform_data_;

  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(DlRuntimeEffectColorSource);
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_RUNTIME_EFFECT_COLOR_SOURCE_H_
