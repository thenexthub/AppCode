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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_PAINT_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_PAINT_H_

#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/dl_paint.h"
#include "impeller/geometry/color.h"
#include "impeller/toolkit/interop/color_filter.h"
#include "impeller/toolkit/interop/color_source.h"
#include "impeller/toolkit/interop/formats.h"
#include "impeller/toolkit/interop/image_filter.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/mask_filter.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class Paint final
    : public Object<Paint, IMPELLER_INTERNAL_HANDLE_NAME(ImpellerPaint)> {
 public:
  Paint();

  ~Paint() override;

  Paint(const Paint&) = delete;

  Paint& operator=(const Paint&) = delete;

  const appcode::DlPaint& GetPaint() const;

  void SetColor(appcode::DlColor color);

  void SetBlendMode(BlendMode mode);

  void SetDrawStyle(appcode::DlDrawStyle style);

  void SetStrokeCap(appcode::DlStrokeCap stroke_cap);

  void SetStrokeJoin(appcode::DlStrokeJoin stroke_join);

  void SetStrokeWidth(Scalar width);

  void SetStrokeMiter(Scalar miter);

  void SetColorFilter(const ColorFilter& filter);

  void SetColorSource(const ColorSource& source);

  void SetImageFilter(const ImageFilter& filter);

  void SetMaskFilter(const MaskFilter& filter);

 private:
  appcode::DlPaint paint_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_PAINT_H_
