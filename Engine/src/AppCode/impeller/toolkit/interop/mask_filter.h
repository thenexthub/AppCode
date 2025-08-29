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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_MASK_FILTER_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_MASK_FILTER_H_

#include "appcode/display_list/effects/dl_mask_filter.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class MaskFilter final
    : public Object<MaskFilter,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerMaskFilter)> {
 public:
  static ScopedObject<MaskFilter> MakeBlur(appcode::DlBlurStyle style,
                                           float sigma);

  explicit MaskFilter(std::shared_ptr<appcode::DlMaskFilter> mask_filter);

  ~MaskFilter() override;

  MaskFilter(const MaskFilter&) = delete;

  MaskFilter& operator=(const MaskFilter&) = delete;

  const std::shared_ptr<appcode::DlMaskFilter>& GetMaskFilter() const;

 public:
  std::shared_ptr<appcode::DlMaskFilter> mask_filter_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_MASK_FILTER_H_
