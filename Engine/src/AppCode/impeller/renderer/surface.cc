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

#include "impeller/renderer/surface.h"

#include "appcode/fml/logging.h"

namespace impeller {

Surface::Surface() : Surface(RenderTarget{}) {}

Surface::Surface(const RenderTarget& target_desc) : desc_(target_desc) {
  if (auto size = desc_.GetColorAttachmentSize(0u); size.has_value()) {
    size_ = size.value();
  } else {
    return;
  }

  is_valid_ = true;
}

Surface::~Surface() = default;

const ISize& Surface::GetSize() const {
  return size_;
}

bool Surface::IsValid() const {
  return is_valid_;
}

const RenderTarget& Surface::GetRenderTarget() const {
  return desc_;
}

bool Surface::Present() const {
  return false;
};

}  // namespace impeller
