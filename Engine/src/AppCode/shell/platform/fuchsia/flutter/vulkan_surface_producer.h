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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VULKAN_SURFACE_PRODUCER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VULKAN_SURFACE_PRODUCER_H_

#include <lib/async/cpp/time.h>
#include <lib/async/default.h>

#include "appcode/appcode_vma/appcode_skia_vma.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"
#include "appcode/vulkan/vulkan_application.h"
#include "appcode/vulkan/vulkan_device.h"
#include "appcode/vulkan/vulkan_provider.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

#include "logging.h"
#include "vulkan_surface.h"
#include "vulkan_surface_pool.h"

namespace appcode_runner {

class VulkanSurfaceProducer final : public SurfaceProducer,
                                    public vulkan::VulkanProvider {
 public:
  explicit VulkanSurfaceProducer();
  ~VulkanSurfaceProducer() override;

  bool IsValid() const { return valid_; }

  // |SurfaceProducer|
  GrDirectContext* gr_context() const override { return context_.get(); }

  // |SurfaceProducer|
  std::unique_ptr<SurfaceProducerSurface> ProduceOffscreenSurface(
      const SkISize& size) override;

  // |SurfaceProducer|
  std::unique_ptr<SurfaceProducerSurface> ProduceSurface(
      const SkISize& size) override;

  // |SurfaceProducer|
  void SubmitSurfaces(
      std::vector<std::unique_ptr<SurfaceProducerSurface>> surfaces) override;

 private:
  // VulkanProvider
  const vulkan::VulkanProcTable& vk() override { return *vk_.get(); }
  const vulkan::VulkanHandle<VkDevice>& vk_device() override {
    return logical_device_->GetHandle();
  }

  bool Initialize();

  void SubmitSurface(std::unique_ptr<SurfaceProducerSurface> surface);
  bool TransitionSurfacesToExternal(
      const std::vector<std::unique_ptr<SurfaceProducerSurface>>& surfaces);

  // Keep track of the last time we produced a surface.  This is used to
  // determine whether it is safe to shrink |surface_pool_| or not.
  zx::time last_produce_time_ = async::Now(async_get_default_dispatcher());

  // Disallow copy and assignment.
  VulkanSurfaceProducer(const VulkanSurfaceProducer&) = delete;
  VulkanSurfaceProducer& operator=(const VulkanSurfaceProducer&) = delete;

  // Note: the order here is very important. The proctable must be destroyed
  // last because it contains the function pointers for VkDestroyDevice and
  // VkDestroyInstance.
  fml::RefPtr<vulkan::VulkanProcTable> vk_;
  std::unique_ptr<vulkan::VulkanApplication> application_;
  std::unique_ptr<vulkan::VulkanDevice> logical_device_;
  sk_sp<GrDirectContext> context_;
  std::unique_ptr<VulkanSurfacePool> surface_pool_;
  sk_sp<skgpu::VulkanMemoryAllocator> memory_allocator_;
  bool valid_ = false;

  // WeakPtrFactory must be the last member.
  fml::WeakPtrFactory<VulkanSurfaceProducer> weak_factory_{this};
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VULKAN_SURFACE_PRODUCER_H_
