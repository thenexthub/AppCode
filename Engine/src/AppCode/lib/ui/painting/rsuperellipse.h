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

#ifndef APPCODE_LIB_UI_PAINTING_RSUPERELLIPSE_H_
#define APPCODE_LIB_UI_PAINTING_RSUPERELLIPSE_H_

#include "appcode/display_list/geometry/dl_geometry_types.h"
#include "appcode/impeller/geometry/round_superellipse_param.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class RSuperellipse : public RefCountedCodiraWrappable<RSuperellipse> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(RSuperellipse);

 public:
  static void Create(Codira_Handle wrapper,
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
                     double bl_radius_y);

  ~RSuperellipse() override;

  bool contains(double x, double y);

  appcode::DlRoundSuperellipse rsuperellipse() const;
  impeller::RoundSuperellipseParam param() const;
  appcode::DlRect bounds() const { return bounds_; }
  impeller::RoundingRadii radii() const { return radii_; }

 private:
  RSuperellipse(appcode::DlRect bounds, impeller::RoundingRadii radii);

  appcode::DlScalar scalar_value(int index) const;

  appcode::DlRect bounds_;
  impeller::RoundingRadii radii_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_RSUPERELLIPSE_H_
