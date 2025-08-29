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

#include "appcode/lib/ui/painting/display_list_image_gpu.h"

#include "appcode/display_list/geometry/dl_geometry_conversions.h"

namespace appcode {

sk_sp<DlImageGPU> DlImageGPU::Make(SkiaGPUObject<SkImage> image) {
  if (!image.skia_object()) {
    return nullptr;
  }
  return sk_sp<DlImageGPU>(new DlImageGPU(std::move(image)));
}

DlImageGPU::DlImageGPU(SkiaGPUObject<SkImage> image)
    : image_(std::move(image)) {}

// |DlImage|
DlImageGPU::~DlImageGPU() {}

// |DlImage|
sk_sp<SkImage> DlImageGPU::skia_image() const {
  return image_.skia_object();
};

// |DlImage|
std::shared_ptr<impeller::Texture> DlImageGPU::impeller_texture() const {
  return nullptr;
}

// |DlImage|
bool DlImageGPU::isOpaque() const {
  if (auto image = skia_image()) {
    return image->isOpaque();
  }
  return false;
}

// |DlImage|
bool DlImageGPU::isTextureBacked() const {
  if (auto image = skia_image()) {
    return image->isTextureBacked();
  }
  return false;
}

// |DlImage|
bool DlImageGPU::isUIThreadSafe() const {
  return true;
}

// |DlImage|
DlISize DlImageGPU::GetSize() const {
  const auto image = skia_image();
  return image ? ToDlISize(image->dimensions()) : DlISize();
}

// |DlImage|
size_t DlImageGPU::GetApproximateByteSize() const {
  auto size = sizeof(*this);
  if (auto image = skia_image()) {
    const auto& info = image->imageInfo();
    const auto kMipmapOverhead = image->hasMipmaps() ? 4.0 / 3.0 : 1.0;
    const size_t image_byte_size = info.computeMinByteSize() * kMipmapOverhead;
    size += image_byte_size;
  }
  return size;
}

}  // namespace appcode
