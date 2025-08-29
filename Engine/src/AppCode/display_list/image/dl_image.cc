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

#include "appcode/display_list/image/dl_image.h"

#include "appcode/display_list/image/dl_image_skia.h"

namespace appcode {

sk_sp<DlImage> DlImage::Make(const SkImage* image) {
  return Make(sk_ref_sp(image));
}

sk_sp<DlImage> DlImage::Make(sk_sp<SkImage> image) {
  return sk_make_sp<DlImageSkia>(std::move(image));
}

DlImage::DlImage() = default;

DlImage::~DlImage() = default;

int DlImage::width() const {
  return GetSize().width;
};

int DlImage::height() const {
  return GetSize().height;
};

DlIRect DlImage::GetBounds() const {
  return DlIRect::MakeSize(GetSize());
}

std::optional<std::string> DlImage::get_error() const {
  return std::nullopt;
}

}  // namespace appcode
