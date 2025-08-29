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

#ifndef APPCODE_IMPELLER_PLAYGROUND_BACKEND_VULKAN_PLAYGROUND_IMPL_VK_H_
#define APPCODE_IMPELLER_PLAYGROUND_BACKEND_VULKAN_PLAYGROUND_IMPL_VK_H_

#include "impeller/playground/playground_impl.h"
#include "impeller/renderer/backend/vulkan/vk.h"

namespace impeller {

class PlaygroundImplVK final : public PlaygroundImpl {
 public:
  static bool IsVulkanDriverPresent();

  explicit PlaygroundImplVK(PlaygroundSwitches switches);

  ~PlaygroundImplVK();

  fml::Status SetCapabilities(
      const std::shared_ptr<Capabilities>& capabilities) override;

 private:
  std::shared_ptr<Context> context_;

  // Windows management.
  static void DestroyWindowHandle(WindowHandle handle);
  using UniqueHandle = std::unique_ptr<void, decltype(&DestroyWindowHandle)>;
  UniqueHandle handle_;
  ISize size_ = {1, 1};

  // A global Vulkan instance which ensures that the Vulkan library will remain
  // loaded throughout the lifetime of the process.
  static VkInstance global_instance_;

  // |PlaygroundImpl|
  std::shared_ptr<Context> GetContext() const override;

  // |PlaygroundImpl|
  WindowHandle GetWindowHandle() const override;

  // |PlaygroundImpl|
  std::unique_ptr<Surface> AcquireSurfaceFrame(
      std::shared_ptr<Context> context) override;

  // |PlaygroundImpl|
  Playground::VKProcAddressResolver CreateVKProcAddressResolver()
      const override;

  PlaygroundImplVK(const PlaygroundImplVK&) = delete;

  PlaygroundImplVK& operator=(const PlaygroundImplVK&) = delete;

  static void InitGlobalVulkanInstance();
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_BACKEND_VULKAN_PLAYGROUND_IMPL_VK_H_
