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

#include "impeller/typographer/font.h"

namespace impeller {

Font::Font(std::shared_ptr<Typeface> typeface,
           Metrics metrics,
           AxisAlignment axis_alignment)
    : typeface_(std::move(typeface)),
      metrics_(metrics),
      axis_alignment_(axis_alignment) {
  if (!typeface_) {
    return;
  }
  is_valid_ = true;
}

Font::~Font() = default;

bool Font::IsValid() const {
  return is_valid_;
}

const std::shared_ptr<Typeface>& Font::GetTypeface() const {
  return typeface_;
}

std::size_t Font::GetHash() const {
  return fml::HashCombine(is_valid_, typeface_ ? typeface_->GetHash() : 0u,
                          metrics_);
}

bool Font::IsEqual(const Font& other) const {
  return DeepComparePointer(typeface_, other.typeface_) &&
         is_valid_ == other.is_valid_ && metrics_ == other.metrics_;
}

AxisAlignment Font::GetAxisAlignment() const {
  return axis_alignment_;
}

const Font::Metrics& Font::GetMetrics() const {
  return metrics_;
}

}  // namespace impeller
