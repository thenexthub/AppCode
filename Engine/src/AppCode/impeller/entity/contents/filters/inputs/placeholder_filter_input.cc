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

#include "impeller/entity/contents/filters/inputs/placeholder_filter_input.h"

#include <optional>

namespace impeller {

PlaceholderFilterInput::PlaceholderFilterInput(Rect coverage_rect)
    : coverage_rect_(coverage_rect) {}

PlaceholderFilterInput::~PlaceholderFilterInput() = default;

std::optional<Snapshot> PlaceholderFilterInput::GetSnapshot(
    std::string_view label,
    const ContentContext& renderer,
    const Entity& entity,
    std::optional<Rect> coverage_limit,
    int32_t mip_count) const {
  return std::nullopt;
}

std::optional<Rect> PlaceholderFilterInput::GetCoverage(
    const Entity& entity) const {
  return coverage_rect_;
}

}  // namespace impeller
