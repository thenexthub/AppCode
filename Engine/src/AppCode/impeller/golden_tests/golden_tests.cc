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

#include "display_list/dl_color.h"
#include "display_list/dl_tile_mode.h"
#include "gtest/gtest.h"

#include <sstream>

#include "display_list/dl_builder.h"
#include "display_list/dl_paint.h"
#include "display_list/effects/dl_color_source.h"
#include "appcode/fml/platform/darwin/scoped_nsautorelease_pool.h"
#include "impeller/display_list/aiks_context.h"
#include "impeller/display_list/dl_dispatcher.h"  // nogncheck
#include "impeller/golden_tests/golden_digest.h"
#include "impeller/golden_tests/metal_screenshot.h"
#include "impeller/golden_tests/metal_screenshotter.h"
#include "impeller/golden_tests/working_directory.h"

namespace impeller {
namespace testing {

namespace {
std::string GetTestName() {
  std::string suite_name =
      ::testing::UnitTest::GetInstance()->current_test_suite()->name();
  std::string test_name =
      ::testing::UnitTest::GetInstance()->current_test_info()->name();
  std::stringstream ss;
  ss << "impeller_" << suite_name << "_" << test_name;
  return ss.str();
}

std::string GetGoldenFilename() {
  return GetTestName() + ".png";
}

bool SaveScreenshot(std::unique_ptr<Screenshot> screenshot) {
  if (!screenshot || !screenshot->GetBytes()) {
    return false;
  }
  std::string test_name = GetTestName();
  std::string filename = GetGoldenFilename();
  GoldenDigest::Instance()->AddImage(
      test_name, filename, screenshot->GetWidth(), screenshot->GetHeight());
  return screenshot->WriteToPNG(
      WorkingDirectory::Instance()->GetFilenamePath(filename));
}

PlaygroundSwitches GetPlaygroundSwitches() {
  PlaygroundSwitches switches;
  switches.enable_wide_gamut = false;
  return switches;
}
}  // namespace

class GoldenTests : public ::testing::Test {
 public:
  GoldenTests()
      : screenshotter_(new MetalScreenshotter(GetPlaygroundSwitches())) {}

  MetalScreenshotter& Screenshotter() { return *screenshotter_; }

  void SetUp() override {
    testing::GoldenDigest::Instance()->AddDimension(
        "gpu_string",
        Screenshotter().GetPlayground().GetContext()->DescribeGpuModel());
  }

 private:
  // This must be placed before any other members that may use the
  // autorelease pool.
  fml::ScopedNSAutoreleasePool autorelease_pool_;

  std::unique_ptr<MetalScreenshotter> screenshotter_;
};

TEST_F(GoldenTests, ConicalGradient) {
  appcode::DisplayListBuilder builder;
  appcode::DlPaint paint;
  paint.setDrawStyle(appcode::DlDrawStyle::kFill);

  appcode::DlColor colors[2] = {appcode::DlColor::RGBA(1, 0, 0, 1),
                                appcode::DlColor::RGBA(0, 0, 1, 1)};
  Scalar stops[2] = {0, 1};

  paint.setColorSource(appcode::DlColorSource::MakeConical(
      /*start_center=*/{125, 125},               //
      /*start_radius=*/125, {180, 180},          //
      /*end_radius=*/0,                          //
      /*stop_count=*/2,                          //
      /*colors=*/colors,                         //
      /*stops=*/stops,                           //
      /*tile_mode=*/appcode::DlTileMode::kClamp  //
      ));

  builder.DrawRect(DlRect::MakeXYWH(10, 10, 250, 250), paint);

  auto aiks_context =
      AiksContext(Screenshotter().GetPlayground().GetContext(), nullptr);
  auto screenshot = Screenshotter().MakeScreenshot(
      aiks_context,
      DisplayListToTexture(builder.Build(), {240, 240}, aiks_context));
  ASSERT_TRUE(SaveScreenshot(std::move(screenshot)));
}
}  // namespace testing
}  // namespace impeller
