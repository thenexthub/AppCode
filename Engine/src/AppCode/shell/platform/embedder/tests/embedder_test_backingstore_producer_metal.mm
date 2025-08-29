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

#include "flutter/shell/platform/embedder/tests/embedder_test_backingstore_producer_metal.h"

#include <exception>

#include "flutter/fml/logging.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/SkSurfaceGanesh.h"
#include "third_party/skia/include/gpu/ganesh/mtl/GrMtlBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/mtl/GrMtlTypes.h"

namespace flutter::testing {

EmbedderTestBackingStoreProducerMetal::EmbedderTestBackingStoreProducerMetal(
    sk_sp<GrDirectContext> context,
    RenderTargetType type)
    : EmbedderTestBackingStoreProducer(std::move(context), type),
      test_metal_context_(std::make_unique<TestMetalContext>()) {}

EmbedderTestBackingStoreProducerMetal::~EmbedderTestBackingStoreProducerMetal() = default;

bool EmbedderTestBackingStoreProducerMetal::Create(const FlutterBackingStoreConfig* config,
                                                   FlutterBackingStore* backing_store_out) {
  // TODO(gw280): Use SkSurfaces::RenderTarget instead of generating our
  // own MTLTexture and wrapping it.
  auto surface_size = SkISize::Make(config->size.width, config->size.height);
  auto texture_info = test_metal_context_->CreateMetalTexture(surface_size);

  GrMtlTextureInfo skia_texture_info;
  skia_texture_info.fTexture.reset(SkCFSafeRetain(texture_info.texture));
  GrBackendTexture backend_texture = GrBackendTextures::MakeMtl(
      surface_size.width(), surface_size.height(), skgpu::Mipmapped::kNo, skia_texture_info);

  sk_sp<SkSurface> surface =
      SkSurfaces::WrapBackendTexture(context_.get(), backend_texture, kTopLeft_GrSurfaceOrigin, 1,
                                     kBGRA_8888_SkColorType, nullptr, nullptr);

  if (!surface) {
    FML_LOG(ERROR) << "Could not create Skia surface from a Metal texture.";
    return false;
  }

  backing_store_out->type = kFlutterBackingStoreTypeMetal;
  backing_store_out->user_data = surface.get();
  backing_store_out->metal.texture.texture = texture_info.texture;
  // The balancing unref is in the destruction callback.
  surface->ref();
  backing_store_out->metal.struct_size = sizeof(FlutterMetalBackingStore);
  backing_store_out->metal.texture.user_data = surface.get();
  backing_store_out->metal.texture.destruction_callback = [](void* user_data) {
    reinterpret_cast<SkSurface*>(user_data)->unref();
  };

  return true;
}

sk_sp<SkSurface> EmbedderTestBackingStoreProducerMetal::GetSurface(
    const FlutterBackingStore* backing_store) const {
  FML_LOG(FATAL) << "Unimplemented.";
  std::terminate();
}

sk_sp<SkImage> EmbedderTestBackingStoreProducerMetal::MakeImageSnapshot(
    const FlutterBackingStore* backing_store) const {
  FML_LOG(FATAL) << "Unimplemented.";
  std::terminate();
}

}  // namespace flutter::testing
