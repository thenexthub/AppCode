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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_RUNTIME_EFFECT_FILTER_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_RUNTIME_EFFECT_FILTER_CONTENTS_H_

#include "impeller/entity/contents/filters/filter_contents.h"

namespace impeller {

/// A filter that applies a runtime effect shader
class RuntimeEffectFilterContents final : public FilterContents {
 public:
  RuntimeEffectFilterContents() {}

  ~RuntimeEffectFilterContents() = default;

  void SetRuntimeStage(std::shared_ptr<RuntimeStage> runtime_stage);

  void SetUniforms(std::shared_ptr<std::vector<uint8_t>> uniforms);

  void SetTextureInputs(
      std::vector<RuntimeEffectContents::TextureInput> texture_inputs);

 private:
  std::shared_ptr<RuntimeStage> runtime_stage_;
  std::shared_ptr<std::vector<uint8_t>> uniforms_;
  std::vector<RuntimeEffectContents::TextureInput> texture_inputs_;

  // |FilterContents|
  std::optional<Entity> RenderFilter(
      const FilterInput::Vector& input_textures,
      const ContentContext& renderer,
      const Entity& entity,
      const Matrix& effect_transform,
      const Rect& coverage,
      const std::optional<Rect>& coverage_hint) const override;

  // |FilterContents|
  std::optional<Rect> GetFilterSourceCoverage(
      const Matrix& effect_transform,
      const Rect& output_limit) const override;

  RuntimeEffectFilterContents(const RuntimeEffectFilterContents&) = delete;

  RuntimeEffectFilterContents& operator=(const RuntimeEffectFilterContents&) =
      delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_RUNTIME_EFFECT_FILTER_CONTENTS_H_
