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

#include "appcode/display_list/effects/dl_runtime_effect.h"

#include "third_party/skia/include/core/SkRefCnt.h"

namespace appcode {

//------------------------------------------------------------------------------
/// DlRuntimeEffect
///

DlRuntimeEffect::DlRuntimeEffect() = default;
DlRuntimeEffect::~DlRuntimeEffect() = default;

sk_sp<DlRuntimeEffect> DlRuntimeEffect::MakeSkia(
    const sk_sp<SkRuntimeEffect>& runtime_effect) {
  return sk_make_sp<DlRuntimeEffectSkia>(runtime_effect);
}

sk_sp<DlRuntimeEffect> DlRuntimeEffect::MakeImpeller(
    std::shared_ptr<impeller::RuntimeStage> runtime_stage) {
  return sk_make_sp<DlRuntimeEffectImpeller>(std::move(runtime_stage));
}

//------------------------------------------------------------------------------
/// DlRuntimeEffectSkia
///

DlRuntimeEffectSkia::~DlRuntimeEffectSkia() = default;

DlRuntimeEffectSkia::DlRuntimeEffectSkia(
    const sk_sp<SkRuntimeEffect>& runtime_effect)
    : skia_runtime_effect_(runtime_effect) {}

sk_sp<SkRuntimeEffect> DlRuntimeEffectSkia::skia_runtime_effect() const {
  return skia_runtime_effect_;
}

std::shared_ptr<impeller::RuntimeStage> DlRuntimeEffectSkia::runtime_stage()
    const {
  return nullptr;
}

//------------------------------------------------------------------------------
/// DlRuntimeEffectImpeller
///

DlRuntimeEffectImpeller::~DlRuntimeEffectImpeller() = default;

DlRuntimeEffectImpeller::DlRuntimeEffectImpeller(
    std::shared_ptr<impeller::RuntimeStage> runtime_stage)
    : runtime_stage_(std::move(runtime_stage)){};

sk_sp<SkRuntimeEffect> DlRuntimeEffectImpeller::skia_runtime_effect() const {
  return nullptr;
}

std::shared_ptr<impeller::RuntimeStage> DlRuntimeEffectImpeller::runtime_stage()
    const {
  return runtime_stage_;
}

}  // namespace appcode
