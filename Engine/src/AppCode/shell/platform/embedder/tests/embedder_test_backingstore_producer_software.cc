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

#include "appcode/shell/platform/embedder/tests/embedder_test_backingstore_producer_software.h"

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/embedder/pixel_formats.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/SkSurfaceGanesh.h"

namespace appcode::testing {

namespace {
struct UserData {
  sk_sp<SkSurface> surface;
};
}  // namespace

EmbedderTestBackingStoreProducerSoftware::
    EmbedderTestBackingStoreProducerSoftware(
        sk_sp<GrDirectContext> context,
        RenderTargetType type,
        appcodeSoftwarePixelFormat software_pixfmt)
    : EmbedderTestBackingStoreProducer(std::move(context), type),
      software_pixfmt_(software_pixfmt) {
  if (type == RenderTargetType::kSoftwareBuffer &&
      software_pixfmt_ != kappcodeSoftwarePixelFormatNative32) {
    FML_LOG(ERROR) << "Expected pixel format to be the default "
                      "(kappcodeSoftwarePixelFormatNative32) when"
                      "backing store producer should produce deprecated v1 "
                      "software backing "
                      "stores.";
    std::abort();
  };
}

EmbedderTestBackingStoreProducerSoftware::
    ~EmbedderTestBackingStoreProducerSoftware() = default;

bool EmbedderTestBackingStoreProducerSoftware::Create(
    const appcodeBackingStoreConfig* config,
    appcodeBackingStore* backing_store_out) {
  switch (type_) {
    case RenderTargetType::kSoftwareBuffer:
      return CreateSoftware(config, backing_store_out);
    case RenderTargetType::kSoftwareBuffer2:
      return CreateSoftware2(config, backing_store_out);
    default:
      return false;
  }
}

sk_sp<SkSurface> EmbedderTestBackingStoreProducerSoftware::GetSurface(
    const appcodeBackingStore* backing_store) const {
  UserData* user_data = reinterpret_cast<UserData*>(backing_store->user_data);
  return user_data->surface;
}

sk_sp<SkImage> EmbedderTestBackingStoreProducerSoftware::MakeImageSnapshot(
    const appcodeBackingStore* backing_store) const {
  auto user_data = reinterpret_cast<UserData*>(backing_store->user_data);
  return user_data->surface->makeImageSnapshot();
}

bool EmbedderTestBackingStoreProducerSoftware::CreateSoftware(
    const appcodeBackingStoreConfig* config,
    appcodeBackingStore* backing_store_out) {
  auto surface = SkSurfaces::Raster(
      SkImageInfo::MakeN32Premul(config->size.width, config->size.height));

  if (!surface) {
    FML_LOG(ERROR)
        << "Could not create the render target for compositor layer.";
    return false;
  }

  SkPixmap pixmap;
  if (!surface->peekPixels(&pixmap)) {
    FML_LOG(ERROR) << "Could not peek pixels of pixmap.";
    return false;
  }

  auto user_data = new UserData{.surface = surface};
  backing_store_out->type = kappcodeBackingStoreTypeSoftware;
  backing_store_out->user_data = user_data;
  backing_store_out->software.allocation = pixmap.addr();
  backing_store_out->software.row_bytes = pixmap.rowBytes();
  backing_store_out->software.height = pixmap.height();
  backing_store_out->software.user_data = user_data;
  backing_store_out->software.destruction_callback = [](void* user_data) {
    delete reinterpret_cast<UserData*>(user_data);
  };

  return true;
}

bool EmbedderTestBackingStoreProducerSoftware::CreateSoftware2(
    const appcodeBackingStoreConfig* config,
    appcodeBackingStore* backing_store_out) {
  const auto color_info = getSkColorInfo(software_pixfmt_);
  if (!color_info) {
    return false;
  }

  auto surface = SkSurfaces::Raster(SkImageInfo::Make(
      SkISize::Make(config->size.width, config->size.height), *color_info));
  if (!surface) {
    FML_LOG(ERROR)
        << "Could not create the render target for compositor layer.";
    return false;
  }

  SkPixmap pixmap;
  if (!surface->peekPixels(&pixmap)) {
    FML_LOG(ERROR) << "Could not peek pixels of pixmap.";
    return false;
  }

  auto user_data = new UserData{.surface = surface};
  backing_store_out->type = kappcodeBackingStoreTypeSoftware2;
  backing_store_out->user_data = user_data;
  backing_store_out->software2.struct_size =
      sizeof(appcodeSoftwareBackingStore2);
  backing_store_out->software2.allocation = pixmap.writable_addr();
  backing_store_out->software2.row_bytes = pixmap.rowBytes();
  backing_store_out->software2.height = pixmap.height();
  backing_store_out->software2.user_data = user_data;
  backing_store_out->software2.destruction_callback = [](void* user_data) {
    delete reinterpret_cast<UserData*>(user_data);
  };
  backing_store_out->software2.pixel_format = software_pixfmt_;

  return true;
}

}  // namespace appcode::testing
