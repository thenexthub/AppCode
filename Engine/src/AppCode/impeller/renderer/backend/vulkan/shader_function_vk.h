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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SHADER_FUNCTION_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SHADER_FUNCTION_VK_H_

#include "impeller/base/backend_cast.h"
#include "impeller/renderer/backend/vulkan/device_holder_vk.h"
#include "impeller/renderer/backend/vulkan/shader_function_vk.h"
#include "impeller/renderer/backend/vulkan/vk.h"
#include "impeller/renderer/shader_function.h"

namespace impeller {

class ShaderFunctionVK final
    : public ShaderFunction,
      public BackendCast<ShaderFunctionVK, ShaderFunction> {
 public:
  // |ShaderFunction|
  ~ShaderFunctionVK() override;

  const vk::ShaderModule& GetModule() const;

 private:
  friend class ShaderLibraryVK;

  vk::UniqueShaderModule module_;
  std::weak_ptr<DeviceHolderVK> device_holder_;

  ShaderFunctionVK(const std::weak_ptr<DeviceHolderVK>& device_holder,
                   UniqueID parent_library_id,
                   std::string name,
                   ShaderStage stage,
                   vk::UniqueShaderModule module);

  ShaderFunctionVK(const ShaderFunctionVK&) = delete;

  ShaderFunctionVK& operator=(const ShaderFunctionVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SHADER_FUNCTION_VK_H_
