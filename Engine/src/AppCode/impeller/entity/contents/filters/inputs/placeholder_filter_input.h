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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_INPUTS_PLACEHOLDER_FILTER_INPUT_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_INPUTS_PLACEHOLDER_FILTER_INPUT_H_

#include "impeller/entity/contents/filters/inputs/filter_input.h"

namespace impeller {

class PlaceholderFilterInput final : public FilterInput {
 public:
  explicit PlaceholderFilterInput(Rect coverage);

  ~PlaceholderFilterInput() override;

  // |FilterInput|
  std::optional<Snapshot> GetSnapshot(std::string_view label,
                                      const ContentContext& renderer,
                                      const Entity& entity,
                                      std::optional<Rect> coverage_limit,
                                      int32_t mip_count = 1) const override;

  // |FilterInput|
  std::optional<Rect> GetCoverage(const Entity& entity) const override;

 private:
  Rect coverage_rect_;

  friend FilterInput;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_INPUTS_PLACEHOLDER_FILTER_INPUT_H_
