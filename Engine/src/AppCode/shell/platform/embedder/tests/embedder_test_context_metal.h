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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_METAL_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_METAL_H_

#include "appcode/shell/platform/embedder/tests/embedder_test_context.h"
#include "appcode/testing/test_metal_context.h"
#include "appcode/testing/test_metal_surface.h"

namespace appcode {
namespace testing {

class EmbedderTestContextMetal : public EmbedderTestContext {
 public:
  using TestExternalTextureCallback =
      std::function<bool(int64_t texture_id,
                         size_t w,
                         size_t h,
                         FlutterMetalExternalTexture* output)>;

  using NextDrawableCallback =
      std::function<FlutterMetalTexture(const FlutterFrameInfo* frame_info)>;

  using PresentCallback = std::function<bool(int64_t texture_id)>;

  explicit EmbedderTestContextMetal(std::string assets_path = "");

  ~EmbedderTestContextMetal() override;

  // |EmbedderTestContext|
  EmbedderTestContextType GetContextType() const override;

  // |EmbedderTestContext|
  size_t GetSurfacePresentCount() const override;

  void SetExternalTextureCallback(
      TestExternalTextureCallback external_texture_frame_callback);

  // Override the default handling for Present.
  void SetPresentCallback(PresentCallback present_callback);

  bool Present(int64_t texture_id);

  bool PopulateExternalTexture(int64_t texture_id,
                               size_t w,
                               size_t h,
                               FlutterMetalExternalTexture* output);

  TestMetalContext* GetTestMetalContext();

  TestMetalSurface* GetTestMetalSurface();

  // Override the default handling for GetNextDrawable.
  void SetNextDrawableCallback(NextDrawableCallback next_drawable_callback);

  FlutterMetalTexture GetNextDrawable(const FlutterFrameInfo* frame_info);

 private:
  // |EmbedderTestContext|
  void SetSurface(SkISize surface_size) override;

  // |EmbedderTestContext|
  void SetupCompositor() override;

  TestExternalTextureCallback external_texture_frame_callback_ = nullptr;
  SkISize surface_size_ = SkISize::MakeEmpty();
  std::unique_ptr<TestMetalContext> metal_context_;
  std::unique_ptr<TestMetalSurface> metal_surface_;
  size_t present_count_ = 0;
  PresentCallback present_callback_ = nullptr;
  NextDrawableCallback next_drawable_callback_ = nullptr;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderTestContextMetal);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_METAL_H_
