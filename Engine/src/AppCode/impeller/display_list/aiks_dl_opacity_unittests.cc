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

#include "display_list/dl_blend_mode.h"
#include "appcode/impeller/display_list/aiks_unittests.h"

#include "appcode/display_list/dl_builder.h"
#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/dl_paint.h"
#include "appcode/testing/testing.h"

namespace impeller {
namespace testing {

using namespace appcode;

TEST_P(AiksTest, DrawOpacityPeephole) {
  DisplayListBuilder builder;

  DlPaint green;
  green.setColor(DlColor::kGreen().modulateOpacity(0.5));

  DlPaint alpha;
  alpha.setColor(DlColor::kRed().modulateOpacity(0.5));

  builder.SaveLayer(std::nullopt, &alpha);
  builder.DrawRect(DlRect::MakeXYWH(0, 0, 100, 100), green);
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, CanRenderGroupOpacity) {
  DisplayListBuilder builder;

  DlPaint red;
  red.setColor(DlColor::kRed());
  DlPaint green;
  green.setColor(DlColor::kGreen().modulateOpacity(0.5));
  DlPaint blue;
  blue.setColor(DlColor::kBlue());

  DlPaint alpha;
  alpha.setColor(DlColor::kRed().modulateOpacity(0.5));

  builder.SaveLayer(std::nullopt, &alpha);
  builder.DrawRect(DlRect::MakeXYWH(0, 0, 100, 100), red);
  builder.DrawRect(DlRect::MakeXYWH(200, 200, 100, 100), green);
  builder.DrawRect(DlRect::MakeXYWH(400, 400, 100, 100), blue);
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

TEST_P(AiksTest, CanRenderGroupOpacityToSavelayer) {
  DisplayListBuilder builder;

  DlPaint red;
  red.setColor(DlColor::kRed());

  DlPaint alpha;
  alpha.setColor(DlColor::kRed().modulateOpacity(0.7));

  // Create a saveLayer that will forward its opacity to another
  // saveLayer, to verify that we correctly distribute opacity.
  DlRect bounds = DlRect::MakeLTRB(0, 0, 500, 500);
  builder.SaveLayer(bounds, &alpha);
  builder.SaveLayer(bounds, &alpha);
  builder.DrawRect(DlRect::MakeXYWH(0, 0, 400, 400), red);
  builder.DrawRect(DlRect::MakeXYWH(0, 0, 450, 450), red);
  builder.Restore();
  builder.Restore();

  ASSERT_TRUE(OpenPlaygroundHere(builder.Build()));
}

}  // namespace testing
}  // namespace impeller
