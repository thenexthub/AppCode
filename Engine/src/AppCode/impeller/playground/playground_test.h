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

#ifndef APPCODE_IMPELLER_PLAYGROUND_PLAYGROUND_TEST_H_
#define APPCODE_IMPELLER_PLAYGROUND_PLAYGROUND_TEST_H_

#include <memory>

#include "appcode/testing/test_args.h"
#include "appcode/testing/testing.h"
#include "impeller/playground/playground.h"
#include "impeller/playground/switches.h"

#if FML_OS_MACOSX
#include "appcode/fml/platform/darwin/scoped_nsautorelease_pool.h"
#endif

namespace impeller {

class PlaygroundTest : public Playground,
                       public ::testing::TestWithParam<PlaygroundBackend> {
 public:
  PlaygroundTest();

  virtual ~PlaygroundTest();

  void SetUp() override;

  void TearDown() override;

  PlaygroundBackend GetBackend() const;

  // |Playground|
  std::unique_ptr<fml::Mapping> OpenAssetAsMapping(
      std::string asset_name) const override;

  RuntimeStage::Map OpenAssetAsRuntimeStage(const char* asset_name) const;

  // |Playground|
  std::string GetWindowTitle() const override;

 private:
  // |Playground|
  bool ShouldKeepRendering() const override;

#if FML_OS_MACOSX
  fml::ScopedNSAutoreleasePool autorelease_pool_;
#endif

  PlaygroundTest(const PlaygroundTest&) = delete;

  PlaygroundTest& operator=(const PlaygroundTest&) = delete;
};

#define INSTANTIATE_PLAYGROUND_SUITE(playground)                             \
  [[maybe_unused]] const char* kYouInstantiated##playground##MultipleTimes = \
      "";                                                                    \
  INSTANTIATE_TEST_SUITE_P(                                                  \
      Play, playground,                                                      \
      ::testing::Values(PlaygroundBackend::kMetal,                           \
                        PlaygroundBackend::kOpenGLES,                        \
                        PlaygroundBackend::kVulkan),                         \
      [](const ::testing::TestParamInfo<PlaygroundTest::ParamType>& info) {  \
        return PlaygroundBackendToString(info.param);                        \
      });

#define INSTANTIATE_METAL_PLAYGROUND_SUITE(playground)                       \
  [[maybe_unused]] const char* kYouInstantiated##playground##MultipleTimes = \
      "";                                                                    \
  INSTANTIATE_TEST_SUITE_P(                                                  \
      Play, playground, ::testing::Values(PlaygroundBackend::kMetal),        \
      [](const ::testing::TestParamInfo<PlaygroundTest::ParamType>& info) {  \
        return PlaygroundBackendToString(info.param);                        \
      });

#define INSTANTIATE_VULKAN_PLAYGROUND_SUITE(playground)                      \
  [[maybe_unused]] const char* kYouInstantiated##playground##MultipleTimes = \
      "";                                                                    \
  INSTANTIATE_TEST_SUITE_P(                                                  \
      Play, playground, ::testing::Values(PlaygroundBackend::kVulkan),       \
      [](const ::testing::TestParamInfo<PlaygroundTest::ParamType>& info) {  \
        return PlaygroundBackendToString(info.param);                        \
      });

#define INSTANTIATE_OPENGLES_PLAYGROUND_SUITE(playground)                    \
  [[maybe_unused]] const char* kYouInstantiated##playground##MultipleTimes = \
      "";                                                                    \
  INSTANTIATE_TEST_SUITE_P(                                                  \
      Play, playground, ::testing::Values(PlaygroundBackend::kOpenGLES),     \
      [](const ::testing::TestParamInfo<PlaygroundTest::ParamType>& info) {  \
        return PlaygroundBackendToString(info.param);                        \
      });

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_PLAYGROUND_TEST_H_
