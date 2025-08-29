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

#include "appcode/fml/time/time_point.h"

#include "appcode/testing/test_args.h"
#include "impeller/playground/compute_playground_test.h"

namespace impeller {

ComputePlaygroundTest::ComputePlaygroundTest()
    : Playground(PlaygroundSwitches{appcode::testing::GetArgsForProcess()}) {}

ComputePlaygroundTest::~ComputePlaygroundTest() = default;

void ComputePlaygroundTest::SetUp() {
  if (!Playground::SupportsBackend(GetParam())) {
    GTEST_SKIP() << "Playground doesn't support this backend type.";
    return;
  }

  if (!Playground::ShouldOpenNewPlaygrounds()) {
    GTEST_SKIP() << "Skipping due to user action.";
    return;
  }

  SetupContext(GetParam(), switches_);
  SetupWindow();

  start_time_ = fml::TimePoint::Now().ToEpochDelta();
}

void ComputePlaygroundTest::TearDown() {
  TeardownWindow();
}

// |Playground|
std::unique_ptr<fml::Mapping> ComputePlaygroundTest::OpenAssetAsMapping(
    std::string asset_name) const {
  return appcode::testing::OpenFixtureAsMapping(asset_name);
}

static std::string FormatWindowTitle(const std::string& test_name) {
  std::stringstream stream;
  stream << "Impeller Playground for '" << test_name << "' (Press ESC to quit)";
  return stream.str();
}

// |Playground|
std::string ComputePlaygroundTest::GetWindowTitle() const {
  return FormatWindowTitle(appcode::testing::GetCurrentTestName());
}

}  // namespace impeller
