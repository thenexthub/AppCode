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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_VULKAN_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_VULKAN_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/common/context_options.h"
#include "appcode/shell/gpu/gpu_surface_vulkan.h"
#include "appcode/shell/gpu/gpu_surface_vulkan_delegate.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/embedder_external_view_embedder.h"
#include "appcode/shell/platform/embedder/embedder_surface.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"

namespace appcode {

class EmbedderSurfaceVulkan final : public EmbedderSurface,
                                    public GPUSurfaceVulkanDelegate {
 public:
  struct VulkanDispatchTable {
    PFN_vkGetInstanceProcAddr get_instance_proc_address;  // required
    std::function<FlutterVulkanImage(const SkISize& frame_size)>
        get_next_image;  // required
    std::function<bool(VkImage image, VkFormat format)>
        present_image;  // required
  };

  EmbedderSurfaceVulkan(
      uint32_t version,
      VkInstance instance,
      size_t instance_extension_count,
      const char** instance_extensions,
      size_t device_extension_count,
      const char** device_extensions,
      VkPhysicalDevice physical_device,
      VkDevice device,
      uint32_t queue_family_index,
      VkQueue queue,
      const VulkanDispatchTable& vulkan_dispatch_table,
      std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder);

  ~EmbedderSurfaceVulkan() override;

  // |GPUSurfaceVulkanDelegate|
  const vulkan::VulkanProcTable& vk() override;

  // |GPUSurfaceVulkanDelegate|
  FlutterVulkanImage AcquireImage(const SkISize& size) override;

  // |GPUSurfaceVulkanDelegate|
  bool PresentImage(VkImage image, VkFormat format) override;

 private:
  bool valid_ = false;
  fml::RefPtr<vulkan::VulkanProcTable> vk_;
  vulkan::VulkanDevice device_;
  VulkanDispatchTable vulkan_dispatch_table_;
  std::shared_ptr<EmbedderExternalViewEmbedder> external_view_embedder_;
  sk_sp<GrDirectContext> main_context_;
  sk_sp<GrDirectContext> resource_context_;

  // |EmbedderSurface|
  bool IsValid() const override;

  // |EmbedderSurface|
  std::unique_ptr<Surface> CreateGPUSurface() override;

  // |EmbedderSurface|
  sk_sp<GrDirectContext> CreateResourceContext() const override;

  sk_sp<GrDirectContext> CreateGrContext(VkInstance instance,
                                         uint32_t version,
                                         size_t instance_extension_count,
                                         const char** instance_extensions,
                                         size_t device_extension_count,
                                         const char** device_extensions,
                                         ContextType context_type) const;

  void* GetInstanceProcAddress(VkInstance instance, const char* proc_name);

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderSurfaceVulkan);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_VULKAN_H_
