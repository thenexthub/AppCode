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

#include "impeller/base/timing.h"
#include "impeller/base/validation.h"
#include "impeller/playground/playground_test.h"

namespace impeller {

PlaygroundTest::PlaygroundTest()
    : Playground(PlaygroundSwitches{appcode::testing::GetArgsForProcess()}) {
  ImpellerValidationErrorsSetCallback(
      [](const char* message, const char* file, int line) -> bool {
        // GTEST_MESSAGE_AT_ can only be used in a function that returns void.
        // Hence the goofy lambda. The failure message and location will still
        // be correct however.
        //
        // https://google.github.io/googletest/advanced.html#assertion-placement
        [message, file, line]() -> void {
          GTEST_MESSAGE_AT_(file, line, "Impeller Validation Error",
                            ::testing::TestPartResult::kFatalFailure)
              << message;
        }();
        return true;
      });
}

PlaygroundTest::~PlaygroundTest() {
  ImpellerValidationErrorsSetCallback(nullptr);
}

namespace {
bool DoesSupportWideGamutTests() {
#ifdef __arm64__
  return true;
#else
  return false;
#endif
}
}  // namespace

void PlaygroundTest::SetUp() {
  if (!Playground::SupportsBackend(GetParam())) {
    GTEST_SKIP() << "Playground doesn't support this backend type.";
    return;
  }

  if (!Playground::ShouldOpenNewPlaygrounds()) {
    GTEST_SKIP() << "Skipping due to user action.";
    return;
  }

  // Test names that end with "WideGamut" will render with wide gamut support.
  std::string test_name = appcode::testing::GetCurrentTestName();
  PlaygroundSwitches switches = switches_;
  switches.enable_wide_gamut =
      test_name.find("WideGamut/") != std::string::npos;

  if (switches.enable_wide_gamut && (GetParam() != PlaygroundBackend::kMetal ||
                                     !DoesSupportWideGamutTests())) {
    GTEST_SKIP() << "This backend doesn't yet support wide gamut.";
    return;
  }

  switches.flags.antialiased_lines =
      test_name.find("ExperimentAntialiasLines/") != std::string::npos;

  SetupContext(GetParam(), switches);
  SetupWindow();
}

PlaygroundBackend PlaygroundTest::GetBackend() const {
  return GetParam();
}

void PlaygroundTest::TearDown() {
  TeardownWindow();
}

// |Playground|
std::unique_ptr<fml::Mapping> PlaygroundTest::OpenAssetAsMapping(
    std::string asset_name) const {
  return appcode::testing::OpenFixtureAsMapping(asset_name);
}

RuntimeStage::Map PlaygroundTest::OpenAssetAsRuntimeStage(
    const char* asset_name) const {
  const std::shared_ptr<fml::Mapping> fixture =
      appcode::testing::OpenFixtureAsMapping(asset_name);
  if (!fixture || fixture->GetSize() == 0) {
    return {};
  }
  return RuntimeStage::DecodeRuntimeStages(fixture);
}

// |Playground|
std::string PlaygroundTest::GetWindowTitle() const {
  std::stringstream stream;
  stream << "Impeller Playground for '"
         << appcode::testing::GetCurrentTestName() << "' ";
  switch (GetBackend()) {
    case PlaygroundBackend::kMetal:
      break;
    case PlaygroundBackend::kOpenGLES:
      if (switches_.use_angle) {
        stream << " (Angle) ";
      }
      break;
    case PlaygroundBackend::kVulkan:
      if (switches_.use_swiftshader) {
        stream << " (SwiftShader) ";
      }
      break;
  }
  stream << " (Press ESC to quit)";
  return stream.str();
}

// |Playground|
bool PlaygroundTest::ShouldKeepRendering() const {
  if (!switches_.timeout.has_value()) {
    return true;
  }

  if (SecondsF{GetSecondsElapsed()} > switches_.timeout.value()) {
    return false;
  }

  return true;
}

}  // namespace impeller
