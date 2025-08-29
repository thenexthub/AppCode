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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_VULKAN_CONTEXT_VK_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_VULKAN_CONTEXT_VK_H_

#include "impeller/renderer/backend/vulkan/vk.h"
#include "impeller/toolkit/interop/context.h"
#include "impeller/toolkit/interop/impeller.h"

namespace impeller::interop {

class ContextVK final : public Context {
 public:
  struct Settings {
    std::function<PFN_vkVoidFunction(VkInstance instance,
                                     const char* proc_name)>
        instance_proc_address_callback;
    bool enable_validation = false;

    explicit Settings(const ImpellerContextVulkanSettings& settings);

    bool IsValid() const;
  };

  static ScopedObject<Context> Create(const Settings& settings);

  static ScopedObject<Context> Create(
      std::shared_ptr<impeller::Context> context);

  // |Context|
  ~ContextVK() override;

  ContextVK(const ContextVK&) = delete;

  ContextVK& operator=(const ContextVK&) = delete;

  bool GetInfo(ImpellerContextVulkanInfo& info) const;

 private:
  explicit ContextVK(std::shared_ptr<impeller::Context> context);
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_VULKAN_CONTEXT_VK_H_
