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

#include "appcode/shell/platform/embedder/tests/embedder_test_context_software.h"

#include <utility>

#include "appcode/fml/make_copyable.h"
#include "appcode/fml/paths.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/shell/platform/embedder/tests/embedder_assertions.h"
#include "appcode/shell/platform/embedder/tests/embedder_test_compositor_software.h"
#include "appcode/testing/testing.h"
#include "third_party/dart/runtime/bin/elf_loader.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace appcode::testing {

EmbedderTestContextSoftware::EmbedderTestContextSoftware(
    std::string assets_path)
    : EmbedderTestContext(std::move(assets_path)) {
  renderer_config_.type = appcodeRendererType::kSoftware;
  renderer_config_.software = {
      .struct_size = sizeof(appcodeSoftwareRendererConfig),
      .surface_present_callback =
          [](void* context, const void* allocation, size_t row_bytes,
             size_t height) {
            auto image_info = SkImageInfo::MakeN32Premul(
                SkISize::Make(row_bytes / 4, height));
            SkBitmap bitmap;
            if (!bitmap.installPixels(image_info, const_cast<void*>(allocation),
                                      row_bytes)) {
              FML_LOG(ERROR) << "Could not copy pixels for the software "
                                "composition from the engine.";
              return false;
            }
            bitmap.setImmutable();
            return reinterpret_cast<EmbedderTestContextSoftware*>(context)
                ->Present(SkImages::RasterFromBitmap(bitmap));
          },
  };
}

EmbedderTestContextSoftware::~EmbedderTestContextSoftware() = default;

EmbedderTestContextType EmbedderTestContextSoftware::GetContextType() const {
  return EmbedderTestContextType::kSoftwareContext;
}

void EmbedderTestContextSoftware::SetSurface(SkISize surface_size) {
  surface_size_ = surface_size;
}

void EmbedderTestContextSoftware::SetupCompositor() {
  FML_CHECK(!compositor_) << "Already set up a compositor in this context.";
  compositor_ = std::make_unique<EmbedderTestCompositorSoftware>(surface_size_);
}

size_t EmbedderTestContextSoftware::GetSurfacePresentCount() const {
  return software_surface_present_count_;
}

bool EmbedderTestContextSoftware::Present(const sk_sp<SkImage>& image) {
  software_surface_present_count_++;
  FireRootSurfacePresentCallbackIfPresent([image] { return image; });
  return true;
}

}  // namespace appcode::testing
