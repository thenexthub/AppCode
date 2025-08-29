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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_DEBUG_REPORT_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_DEBUG_REPORT_VK_H_

#include "impeller/renderer/backend/vulkan/vk.h"

namespace impeller {

class CapabilitiesVK;

class DebugReportVK {
 public:
  DebugReportVK(const CapabilitiesVK& caps, const vk::Instance& instance);

  ~DebugReportVK();

  bool IsValid() const;

 private:
  vk::UniqueDebugUtilsMessengerEXT messenger_;
  bool is_valid_ = false;

  enum class Result {
    kContinue,
    kAbort,
  };

  Result OnDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                         vk::DebugUtilsMessageTypeFlagsEXT type,
                         const vk::DebugUtilsMessengerCallbackDataEXT* data);

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
      vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
      vk::DebugUtilsMessageTypeFlagsEXT type,
      const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
      void* user_data);

  DebugReportVK(const DebugReportVK&) = delete;

  DebugReportVK& operator=(const DebugReportVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_DEBUG_REPORT_VK_H_
