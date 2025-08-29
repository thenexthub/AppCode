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

#include "impeller/renderer/backend/vulkan/shader_function_vk.h"

namespace impeller {

ShaderFunctionVK::ShaderFunctionVK(
    const std::weak_ptr<DeviceHolderVK>& device_holder,
    UniqueID parent_library_id,
    std::string name,
    ShaderStage stage,
    vk::UniqueShaderModule module)
    : ShaderFunction(parent_library_id, std::move(name), stage),
      module_(std::move(module)),
      device_holder_(device_holder) {}

ShaderFunctionVK::~ShaderFunctionVK() {
  std::shared_ptr<DeviceHolderVK> device_holder = device_holder_.lock();
  if (device_holder) {
    module_.reset();
  } else {
    module_.release();
  }
}

const vk::ShaderModule& ShaderFunctionVK::GetModule() const {
  return module_.get();
}

}  // namespace impeller
