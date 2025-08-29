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

#include "appcode/display_list/image/dl_image_skia.h"

#include "appcode/display_list/geometry/dl_geometry_conversions.h"

namespace appcode {

DlImageSkia::DlImageSkia(sk_sp<SkImage> image) : image_(std::move(image)) {}

// |DlImage|
DlImageSkia::~DlImageSkia() = default;

// |DlImage|
sk_sp<SkImage> DlImageSkia::skia_image() const {
  return image_;
};

// |DlImage|
std::shared_ptr<impeller::Texture> DlImageSkia::impeller_texture() const {
  return nullptr;
}

// |DlImage|
bool DlImageSkia::isOpaque() const {
  return image_ ? image_->isOpaque() : false;
}

// |DlImage|
bool DlImageSkia::isTextureBacked() const {
  return image_ ? image_->isTextureBacked() : false;
}

// |DlImage|
bool DlImageSkia::isUIThreadSafe() const {
  // Technically if the image is null then we are thread-safe, and possibly
  // if the image is constructed from a heap raster as well, but there
  // should never be a leak of an instance of this class into any data that
  // is shared with the UI thread, regardless of value.
  // All images intended to be shared with the UI thread should be constructed
  // via one of the DlImage subclasses designed for that purpose.
  return false;
}

// |DlImage|
DlISize DlImageSkia::GetSize() const {
  return image_ ? ToDlISize(image_->dimensions()) : DlISize();
}

// |DlImage|
size_t DlImageSkia::GetApproximateByteSize() const {
  auto size = sizeof(*this);
  if (image_) {
    const auto& info = image_->imageInfo();
    const auto kMipmapOverhead = image_->hasMipmaps() ? 4.0 / 3.0 : 1.0;
    const size_t image_byte_size = info.computeMinByteSize() * kMipmapOverhead;
    size += image_byte_size;
  }
  return size;
}

}  // namespace appcode
