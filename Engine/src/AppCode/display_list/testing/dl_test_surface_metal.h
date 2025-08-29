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

#ifndef APPCODE_DISPLAY_LIST_TESTING_DL_TEST_SURFACE_METAL_H_
#define APPCODE_DISPLAY_LIST_TESTING_DL_TEST_SURFACE_METAL_H_

#include "appcode/display_list/testing/dl_test_surface_provider.h"
#include "appcode/fml/platform/darwin/scoped_nsautorelease_pool.h"
#include "appcode/impeller/golden_tests/metal_screenshotter.h"
#include "appcode/testing/test_metal_surface.h"

namespace appcode {
namespace testing {

using MetalScreenshotter = impeller::testing::MetalScreenshotter;

class DlMetalSurfaceProvider : public DlSurfaceProvider {
 public:
  explicit DlMetalSurfaceProvider() : DlSurfaceProvider() {}
  virtual ~DlMetalSurfaceProvider() = default;

  bool InitializeSurface(size_t width,
                         size_t height,
                         PixelFormat format) override;
  std::shared_ptr<DlSurfaceInstance> GetPrimarySurface() const override;
  std::shared_ptr<DlSurfaceInstance> MakeOffscreenSurface(
      size_t width,
      size_t height,
      PixelFormat format) const override;
  const std::string backend_name() const override { return "Metal"; }
  BackendType backend_type() const override { return kMetalBackend; }
  bool supports(PixelFormat format) const override {
    return format == kN32PremulPixelFormat;
  }
  bool supports_impeller() const override { return true; }
  sk_sp<DlPixelData> ImpellerSnapshot(const sk_sp<DisplayList>& list,
                                      int width,
                                      int height) const override;
  virtual sk_sp<DlImage> MakeImpellerImage(const sk_sp<DisplayList>& list,
                                           int width,
                                           int height) const override;

 private:
  // This must be placed before any other members that may use the
  // autorelease pool.
  fml::ScopedNSAutoreleasePool autorelease_pool_;

  std::unique_ptr<TestMetalContext> metal_context_;
  std::shared_ptr<DlSurfaceInstance> metal_surface_;
  mutable std::unique_ptr<MetalScreenshotter> snapshotter_;
  mutable std::unique_ptr<impeller::AiksContext> aiks_context_;

  void InitScreenShotter() const;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_TESTING_DL_TEST_SURFACE_METAL_H_
