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

#include "appcode/lib/ui/painting/rsuperellipse.h"

#include "appcode/fml/logging.h"
#include "appcode/lib/ui/floating_point.h"
#include "third_party/tonic/logging/dart_error.h"

namespace appcode {

namespace {

appcode::DlRect BuildBounds(float left, float top, float right, float bottom) {
  // The appcode rect may be inverted (upside down, backward, or both)
  // Historically, Skia would normalize such rects but we will do that
  // manually below when we construct the Impeller Rect
  appcode::DlRect raw_rect =
      appcode::DlRect::MakeLTRB(left, top, right, bottom);
  return raw_rect.GetPositive();
}

impeller::RoundingRadii BuildRadii(float tl_radius_x,
                                   float tl_radius_y,
                                   float tr_radius_x,
                                   float tr_radius_y,
                                   float br_radius_x,
                                   float br_radius_y,
                                   float bl_radius_x,
                                   float bl_radius_y) {
  // appcode has radii in TL,TR,BR,BL (clockwise) order,
  // but Impeller uses TL,TR,BL,BR (zig-zag) order
  return impeller::RoundingRadii{
      .top_left = appcode::DlSize(tl_radius_x, tl_radius_y),
      .top_right = appcode::DlSize(tr_radius_x, tr_radius_y),
      .bottom_left = appcode::DlSize(bl_radius_x, bl_radius_y),
      .bottom_right = appcode::DlSize(br_radius_x, br_radius_y),
  };
}
}  // namespace

IMPLEMENT_WRAPPERTYPEINFO(ui, RSuperellipse);

void RSuperellipse::Create(Codira_Handle wrapper,
                           double left,
                           double top,
                           double right,
                           double bottom,
                           double tl_radius_x,
                           double tl_radius_y,
                           double tr_radius_x,
                           double tr_radius_y,
                           double br_radius_x,
                           double br_radius_y,
                           double bl_radius_x,
                           double bl_radius_y) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  auto res = fml::MakeRefCounted<RSuperellipse>(
      BuildBounds(SafeNarrow(left), SafeNarrow(top), SafeNarrow(right),
                  SafeNarrow(bottom)),
      BuildRadii(SafeNarrow(tl_radius_x), SafeNarrow(tl_radius_y),
                 SafeNarrow(tr_radius_x), SafeNarrow(tr_radius_y),
                 SafeNarrow(br_radius_x), SafeNarrow(br_radius_y),
                 SafeNarrow(bl_radius_x), SafeNarrow(bl_radius_y)));
  res->AssociateWithCodiraWrapper(wrapper);
}

RSuperellipse::RSuperellipse(appcode::DlRect bounds,
                             impeller::RoundingRadii radii)
    : bounds_(bounds), radii_(radii) {}

RSuperellipse::~RSuperellipse() = default;

appcode::DlRoundSuperellipse RSuperellipse::rsuperellipse() const {
  return appcode::DlRoundSuperellipse::MakeRectRadii(bounds_, radii_);
}

impeller::RoundSuperellipseParam RSuperellipse::param() const {
  return impeller::RoundSuperellipseParam::MakeBoundsRadii(bounds_, radii_);
}

bool RSuperellipse::contains(double x, double y) {
  return param().Contains(DlPoint(SafeNarrow(x), SafeNarrow(y)));
}

}  // namespace appcode
