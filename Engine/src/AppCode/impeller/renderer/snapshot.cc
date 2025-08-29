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

#include "impeller/renderer/snapshot.h"

#include <optional>

namespace impeller {

std::optional<Rect> Snapshot::GetCoverage() const {
  if (!texture) {
    return std::nullopt;
  }
  return Rect::MakeSize(texture->GetSize()).TransformBounds(transform);
}

std::optional<Matrix> Snapshot::GetUVTransform() const {
  if (!texture || texture->GetSize().IsEmpty()) {
    return std::nullopt;
  }
  return Matrix::MakeScale(1 / Vector2(texture->GetSize())) *
         transform.Invert();
}

std::optional<std::array<Point, 4>> Snapshot::GetCoverageUVs(
    const Rect& coverage) const {
  auto uv_transform = GetUVTransform();
  if (!uv_transform.has_value()) {
    return std::nullopt;
  }
  return coverage.GetTransformedPoints(uv_transform.value());
}

}  // namespace impeller
