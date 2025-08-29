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

#include "appcode/shell/platform/android/image_lru.h"

namespace appcode {

sk_sp<appcode::DlImage> ImageLRU::FindImage(
    std::optional<HardwareBufferKey> key) {
  if (!key.has_value()) {
    return nullptr;
  }
  auto key_value = key.value();
  for (size_t i = 0u; i < kImageReaderSwapchainSize; i++) {
    if (images_[i].key == key_value) {
      auto result = images_[i].value;
      UpdateKey(result, key_value);
      return result;
    }
  }
  return nullptr;
}

void ImageLRU::UpdateKey(const sk_sp<appcode::DlImage>& image,
                         HardwareBufferKey key) {
  if (images_[0].key == key) {
    return;
  }
  size_t i = 1u;
  for (; i < kImageReaderSwapchainSize; i++) {
    if (images_[i].key == key) {
      break;
    }
  }
  for (auto j = i; j > 0; j--) {
    images_[j] = images_[j - 1];
  }
  images_[0] = Data{.key = key, .value = image};
}

HardwareBufferKey ImageLRU::AddImage(const sk_sp<appcode::DlImage>& image,
                                     HardwareBufferKey key) {
  HardwareBufferKey lru_key = images_[kImageReaderSwapchainSize - 1].key;
  bool updated_image = false;
  for (size_t i = 0u; i < kImageReaderSwapchainSize; i++) {
    if (images_[i].key == lru_key) {
      updated_image = true;
      images_[i] = Data{.key = key, .value = image};
      break;
    }
  }
  if (!updated_image) {
    images_[0] = Data{.key = key, .value = image};
  }
  UpdateKey(image, key);
  return lru_key;
}

void ImageLRU::Clear() {
  for (size_t i = 0u; i < kImageReaderSwapchainSize; i++) {
    images_[i] = Data{.key = 0u, .value = nullptr};
  }
}

}  // namespace appcode
