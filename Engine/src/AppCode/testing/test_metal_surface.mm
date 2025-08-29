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

#include "flutter/testing/test_metal_surface.h"

#include "flutter/fml/logging.h"
#include "flutter/testing/test_metal_surface_impl.h"

#include "third_party/skia/include/core/SkSurface.h"

namespace flutter::testing {

bool TestMetalSurface::PlatformSupportsMetal() {
  return true;
}

std::unique_ptr<TestMetalSurface> TestMetalSurface::Create(
    const TestMetalContext& test_metal_context,
    SkISize surface_size) {
  return std::make_unique<TestMetalSurfaceImpl>(test_metal_context, surface_size);
}

std::unique_ptr<TestMetalSurface> TestMetalSurface::Create(
    const TestMetalContext& test_metal_context,
    int64_t texture_id,
    SkISize surface_size) {
  return std::make_unique<TestMetalSurfaceImpl>(test_metal_context, texture_id, surface_size);
}

TestMetalSurface::TestMetalSurface() = default;

TestMetalSurface::~TestMetalSurface() = default;

bool TestMetalSurface::IsValid() const {
  return impl_ ? impl_->IsValid() : false;
}

sk_sp<GrDirectContext> TestMetalSurface::GetGrContext() const {
  return impl_ ? impl_->GetGrContext() : nullptr;
}

sk_sp<SkSurface> TestMetalSurface::GetSurface() const {
  return impl_ ? impl_->GetSurface() : nullptr;
}

sk_sp<SkImage> TestMetalSurface::GetRasterSurfaceSnapshot() {
  return impl_ ? impl_->GetRasterSurfaceSnapshot() : nullptr;
}

TestMetalContext::TextureInfo TestMetalSurface::GetTextureInfo() {
  return impl_ ? impl_->GetTextureInfo() : TestMetalContext::TextureInfo();
}

}  // namespace flutter::testing
