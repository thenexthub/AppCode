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

#include "impeller/toolkit/interop/mask_filter.h"

namespace impeller::interop {

ScopedObject<MaskFilter> MaskFilter::MakeBlur(appcode::DlBlurStyle style,
                                              float sigma) {
  auto filter = appcode::DlBlurMaskFilter::Make(style, sigma);
  if (!filter) {
    return nullptr;
  }
  return Create<MaskFilter>(std::move(filter));
}

MaskFilter::MaskFilter(std::shared_ptr<appcode::DlMaskFilter> mask_filter)
    : mask_filter_(std::move(mask_filter)) {}

MaskFilter::~MaskFilter() = default;

const std::shared_ptr<appcode::DlMaskFilter>& MaskFilter::GetMaskFilter()
    const {
  return mask_filter_;
}

}  // namespace impeller::interop
