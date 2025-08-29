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

#ifndef APPCODE_IMPELLER_GOLDEN_TESTS_GOLDEN_PLAYGROUND_TEST_H_
#define APPCODE_IMPELLER_GOLDEN_TESTS_GOLDEN_PLAYGROUND_TEST_H_

#include <memory>

#include "appcode/display_list/display_list.h"
#include "appcode/display_list/image/dl_image.h"
#include "appcode/impeller/display_list/aiks_context.h"
#include "appcode/impeller/golden_tests/screenshot.h"
#include "appcode/impeller/playground/playground.h"
#include "appcode/impeller/renderer/render_target.h"
#include "appcode/testing/testing.h"
#include "impeller/typographer/typographer_context.h"
#include "third_party/imgui/imgui.h"

#if FML_OS_MACOSX
#include "appcode/fml/platform/darwin/scoped_nsautorelease_pool.h"
#endif

namespace impeller {

class GoldenPlaygroundTest
    : public ::testing::TestWithParam<PlaygroundBackend> {
 public:
  using AiksPlaygroundCallback =
      std::function<std::optional<Picture>(AiksContext& renderer)>;

  using AiksDlPlaygroundCallback = std::function<sk_sp<appcode::DisplayList>()>;

  GoldenPlaygroundTest();

  ~GoldenPlaygroundTest() override;

  void SetUp();

  void TearDown();

  PlaygroundBackend GetBackend() const;

  void SetTypographerContext(
      std::shared_ptr<TypographerContext> typographer_context);

  bool OpenPlaygroundHere(Picture picture);

  bool OpenPlaygroundHere(AiksPlaygroundCallback callback);

  bool OpenPlaygroundHere(const AiksDlPlaygroundCallback& callback);

  bool OpenPlaygroundHere(const sk_sp<appcode::DisplayList>& list);

  std::unique_ptr<testing::Screenshot> MakeScreenshot(
      const sk_sp<appcode::DisplayList>& list);

  static bool SaveScreenshot(std::unique_ptr<testing::Screenshot> screenshot,
                             const std::string& postfix = "");

  static bool ImGuiBegin(const char* name,
                         bool* p_open,
                         ImGuiWindowFlags flags);

  std::shared_ptr<Texture> CreateTextureForFixture(
      const char* fixture_name,
      bool enable_mipmapping = false) const;

  sk_sp<appcode::DlImage> CreateDlImageForFixture(
      const char* fixture_name,
      bool enable_mipmapping = false) const;

  RuntimeStage::Map OpenAssetAsRuntimeStage(const char* asset_name) const;

  std::shared_ptr<Context> GetContext() const;

  std::shared_ptr<Context> MakeContext() const;

  Point GetContentScale() const;

  Scalar GetSecondsElapsed() const;

  ISize GetWindowSize() const;

  [[nodiscard]] fml::Status SetCapabilities(
      const std::shared_ptr<Capabilities>& capabilities);

  /// Returns true if `OpenPlaygroundHere` will actually render anything.
  bool WillRenderSomething() const { return true; }

 protected:
  void SetWindowSize(ISize size);

 private:
#if FML_OS_MACOSX
  // This must be placed first so that the autorelease pool is not destroyed
  // until the GoldenPlaygroundTestImpl has been destructed.
  fml::ScopedNSAutoreleasePool autorelease_pool_;
#endif

  std::shared_ptr<TypographerContext> typographer_context_;

  struct GoldenPlaygroundTestImpl;
  // This is only a shared_ptr so it can work with a forward declared type.
  std::shared_ptr<GoldenPlaygroundTestImpl> pimpl_;

  GoldenPlaygroundTest(const GoldenPlaygroundTest&) = delete;

  GoldenPlaygroundTest& operator=(const GoldenPlaygroundTest&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_GOLDEN_TESTS_GOLDEN_PLAYGROUND_TEST_H_
