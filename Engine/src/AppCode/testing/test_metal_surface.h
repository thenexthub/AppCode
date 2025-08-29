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

#ifndef APPCODE_TESTING_TEST_METAL_SURFACE_H_
#define APPCODE_TESTING_TEST_METAL_SURFACE_H_

#include "appcode/fml/macros.h"
#include "appcode/testing/test_metal_context.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace appcode::testing {

//------------------------------------------------------------------------------
/// @brief      Creates a MTLTexture backed SkSurface and context that can be
///             used to render to in unit-tests.
///
class TestMetalSurface {
 public:
  static bool PlatformSupportsMetal();

  static std::unique_ptr<TestMetalSurface> Create(
      const TestMetalContext& test_metal_context,
      SkISize surface_size = SkISize::MakeEmpty());

  static std::unique_ptr<TestMetalSurface> Create(
      const TestMetalContext& test_metal_context,
      int64_t texture_id,
      SkISize surface_size = SkISize::MakeEmpty());

  virtual ~TestMetalSurface();

  virtual bool IsValid() const;

  virtual sk_sp<GrDirectContext> GetGrContext() const;

  virtual sk_sp<SkSurface> GetSurface() const;

  virtual sk_sp<SkImage> GetRasterSurfaceSnapshot();

  virtual TestMetalContext::TextureInfo GetTextureInfo();

 protected:
  TestMetalSurface();

 private:
  std::unique_ptr<TestMetalSurface> impl_;

  explicit TestMetalSurface(std::unique_ptr<TestMetalSurface> impl);

  FML_DISALLOW_COPY_AND_ASSIGN(TestMetalSurface);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_METAL_SURFACE_H_
