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

#include "text_shadow.h"
#include "third_party/skia/include/core/SkColor.h"

namespace txt {

TextShadow::TextShadow() {}
TextShadow::TextShadow(SkColor color, SkPoint offset, double blur_sigma)
    : color(color), offset(offset), blur_sigma(blur_sigma) {}

bool TextShadow::operator==(const TextShadow& other) const {
  if (color != other.color) {
    return false;
  }
  if (offset != other.offset) {
    return false;
  }
  if (blur_sigma != other.blur_sigma) {
    return false;
  }

  return true;
}

bool TextShadow::operator!=(const TextShadow& other) const {
  return !(*this == other);
}

bool TextShadow::hasShadow() const {
  if (!offset.isZero()) {
    return true;
  }
  if (blur_sigma > 0.5) {
    return true;
  }

  return false;
}

}  // namespace txt
