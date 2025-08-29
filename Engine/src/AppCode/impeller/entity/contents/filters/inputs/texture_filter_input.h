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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_INPUTS_TEXTURE_FILTER_INPUT_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_INPUTS_TEXTURE_FILTER_INPUT_H_

#include "impeller/entity/contents/filters/inputs/filter_input.h"

#include "impeller/geometry/matrix.h"

namespace impeller {

class TextureFilterInput final : public FilterInput {
 public:
  ~TextureFilterInput() override;

  // |FilterInput|
  std::optional<Snapshot> GetSnapshot(std::string_view label,
                                      const ContentContext& renderer,
                                      const Entity& entity,
                                      std::optional<Rect> coverage_limit,
                                      int32_t mip_count) const override;

  // |FilterInput|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

  // |FilterInput|
  Matrix GetLocalTransform(const Entity& entity) const override;

 private:
  explicit TextureFilterInput(std::shared_ptr<Texture> texture,
                              Matrix local_transform = Matrix());

  std::shared_ptr<Texture> texture_;
  Matrix local_transform_;

  friend FilterInput;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_INPUTS_TEXTURE_FILTER_INPUT_H_
