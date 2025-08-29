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

#ifndef APPCODE_TESTING_TEST_METAL_SURFACE_IMPL_H_
#define APPCODE_TESTING_TEST_METAL_SURFACE_IMPL_H_

#include "appcode/fml/macros.h"
#include "appcode/testing/test_metal_context.h"
#include "appcode/testing/test_metal_surface.h"

#include "third_party/skia/include/core/SkSurface.h"

namespace appcode::testing {

class TestMetalSurfaceImpl : public TestMetalSurface {
 public:
  TestMetalSurfaceImpl(const TestMetalContext& test_metal_context,
                       const SkISize& surface_size);

  TestMetalSurfaceImpl(const TestMetalContext& test_metal_context,
                       int64_t texture_id,
                       const SkISize& surface_size);

  // |TestMetalSurface|
  ~TestMetalSurfaceImpl() override;

 private:
  void Init(const TestMetalContext::TextureInfo& texture_info,
            const SkISize& surface_size);

  const TestMetalContext& test_metal_context_;
  bool is_valid_ = false;
  sk_sp<SkSurface> surface_;
  TestMetalContext::TextureInfo texture_info_;

  // |TestMetalSurface|
  bool IsValid() const override;

  // |TestMetalSurface|
  sk_sp<GrDirectContext> GetGrContext() const override;

  // |TestMetalSurface|
  sk_sp<SkSurface> GetSurface() const override;

  // |TestMetalSurface|
  sk_sp<SkImage> GetRasterSurfaceSnapshot() override;

  // |TestMetalSurface|
  TestMetalContext::TextureInfo GetTextureInfo() override;

  FML_DISALLOW_COPY_AND_ASSIGN(TestMetalSurfaceImpl);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_METAL_SURFACE_IMPL_H_
