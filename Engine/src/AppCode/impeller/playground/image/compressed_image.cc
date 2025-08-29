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

#include "impeller/playground/image/compressed_image.h"

namespace impeller {

CompressedImage::CompressedImage(std::shared_ptr<const fml::Mapping> allocation)
    : source_(std::move(allocation)) {}

CompressedImage::~CompressedImage() = default;

bool CompressedImage::IsValid() const {
  return static_cast<bool>(source_);
}

}  // namespace impeller
