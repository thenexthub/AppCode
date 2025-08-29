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

#ifndef APPCODE_VULKAN_VULKAN_DEBUG_REPORT_H_
#define APPCODE_VULKAN_VULKAN_DEBUG_REPORT_H_

#include "appcode/fml/macros.h"
#include "appcode/vulkan/procs/vulkan_handle.h"
#include "appcode/vulkan/procs/vulkan_interface.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"

namespace vulkan {

class VulkanDebugReport {
 public:
  static std::string DebugExtensionName();

  VulkanDebugReport(const VulkanProcTable& vk,
                    const VulkanHandle<VkInstance>& application);

  ~VulkanDebugReport();

  bool IsValid() const;

 private:
  const VulkanProcTable& vk_;
  const VulkanHandle<VkInstance>& application_;
  VulkanHandle<VkDebugReportCallbackEXT> handle_;
  bool valid_;

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanDebugReport);
};

}  // namespace vulkan

#endif  // APPCODE_VULKAN_VULKAN_DEBUG_REPORT_H_
