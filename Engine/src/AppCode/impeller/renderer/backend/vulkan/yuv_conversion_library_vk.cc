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

#include "impeller/renderer/backend/vulkan/yuv_conversion_library_vk.h"

#include "impeller/base/validation.h"
#include "impeller/renderer/backend/vulkan/device_holder_vk.h"

namespace impeller {

YUVConversionLibraryVK::YUVConversionLibraryVK(
    std::weak_ptr<DeviceHolderVK> device_holder)
    : device_holder_(std::move(device_holder)) {}

YUVConversionLibraryVK::~YUVConversionLibraryVK() = default;

std::shared_ptr<YUVConversionVK> YUVConversionLibraryVK::GetConversion(
    const YUVConversionDescriptorVK& desc) {
  Lock lock(conversions_mutex_);
  auto found = conversions_.find(desc);
  if (found != conversions_.end()) {
    return found->second;
  }
  auto device_holder = device_holder_.lock();
  if (!device_holder) {
    VALIDATION_LOG << "Context loss during creation of YUV conversion.";
    return nullptr;
  }
  return (conversions_[desc] = std::shared_ptr<YUVConversionVK>(
              new YUVConversionVK(device_holder->GetDevice(), desc)));
}

}  // namespace impeller
