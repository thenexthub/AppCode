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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VULKAN_SURFACE_POOL_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VULKAN_SURFACE_POOL_H_

#include <fuchsia/ui/composition/cpp/fidl.h>

#include <unordered_map>
#include <vector>

#include "appcode/fml/macros.h"
#include "vulkan_surface.h"

namespace appcode_runner {

class VulkanSurfacePool final {
 public:
  // Only keep 12 surfaces at a time.  This value was based on how many
  // surfaces got cached in the old, exact-match-only caching logic.
  static constexpr int kMaxSurfaces = 12;
  // If a surface doesn't get used for 3 or more generations, we discard it.
  static constexpr int kMaxSurfaceAge = 3;

  VulkanSurfacePool(vulkan::VulkanProvider& vulkan_provider,
                    sk_sp<GrDirectContext> context);

  ~VulkanSurfacePool();

  std::unique_ptr<VulkanSurface> CreateSurface(const SkISize& size);
  std::unique_ptr<VulkanSurface> AcquireSurface(const SkISize& size);

  void SubmitSurface(std::unique_ptr<SurfaceProducerSurface> surface);

  void AgeAndCollectOldBuffers();

  // Shrink all oversized |VulkanSurfaces| in |available_surfaces_| to as
  // small as they can be.
  void ShrinkToFit();

 private:
  vulkan::VulkanProvider& vulkan_provider_;
  sk_sp<GrDirectContext> context_;
  fuchsia::sysmem2::AllocatorSyncPtr sysmem_allocator_;
  fuchsia::ui::composition::AllocatorPtr flatland_allocator_;
  std::vector<std::unique_ptr<VulkanSurface>> available_surfaces_;
  std::unordered_map<uintptr_t, std::unique_ptr<VulkanSurface>>
      pending_surfaces_;

  size_t trace_surfaces_created_ = 0;
  size_t trace_surfaces_reused_ = 0;

  std::unique_ptr<VulkanSurface> GetCachedOrCreateSurface(const SkISize& size);

  void RecycleSurface(std::unique_ptr<VulkanSurface> surface);

  void RecyclePendingSurface(uintptr_t surface_key);

  void TraceStats();

  FML_DISALLOW_COPY_AND_ASSIGN(VulkanSurfacePool);
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_VULKAN_SURFACE_POOL_H_
