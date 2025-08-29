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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_COLOR_FILTER_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_COLOR_FILTER_H_

#include "appcode/display_list/effects/dl_color_filter.h"
#include "impeller/toolkit/interop/formats.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class ColorFilter final
    : public Object<ColorFilter,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerColorFilter)> {
 public:
  static ScopedObject<ColorFilter> MakeBlend(Color color, BlendMode mode);

  static ScopedObject<ColorFilter> MakeMatrix(const float matrix[20]);

  explicit ColorFilter(std::shared_ptr<const appcode::DlColorFilter> filter);

  ~ColorFilter() override;

  ColorFilter(const ColorFilter&) = delete;

  ColorFilter& operator=(const ColorFilter&) = delete;

  const std::shared_ptr<const appcode::DlColorFilter>& GetColorFilter() const;

 private:
  std::shared_ptr<const appcode::DlColorFilter> filter_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_COLOR_FILTER_H_
