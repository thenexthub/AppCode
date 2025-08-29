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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SOFTWARE_SURFACE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SOFTWARE_SURFACE_H_

#include <fuchsia/sysmem2/cpp/fidl.h>
#include <fuchsia/ui/composition/cpp/fidl.h>
#include <lib/async/cpp/wait.h>
#include <lib/zx/event.h>
#include <lib/zx/vmo.h>

#include <array>
#include <cstdint>
#include <memory>

#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"

#include "surface_producer.h"

namespace appcode_runner {

class SoftwareSurface final : public SurfaceProducerSurface {
 public:
  SoftwareSurface(fuchsia::sysmem2::AllocatorSyncPtr& sysmem_allocator,
                  fuchsia::ui::composition::AllocatorPtr& flatland_allocator,
                  const SkISize& size);

  ~SoftwareSurface() override;

  size_t GetAllocationSize() const { return surface_size_bytes_; }

  // |SurfaceProducerSurface|
  size_t AdvanceAndGetAge() override;

  // |SurfaceProducerSurface|
  bool FlushSessionAcquireAndReleaseEvents() override;

  // |SurfaceProducerSurface|
  bool IsValid() const override;

  // |SurfaceProducerSurface|
  SkISize GetSize() const override;

  // |SurfaceProducerSurface|
  void SignalWritesFinished(
      const std::function<void(void)>& on_surface_read_finished) override;

  // |SurfaceProducerSurface|
  void SetImageId(uint32_t image_id) override;

  // |SurfaceProducerSurface|
  uint32_t GetImageId() override;

  // |SurfaceProducerSurface|
  sk_sp<SkSurface> GetSkiaSurface() const override;

  // |SurfaceProducerSurface|
  fuchsia::ui::composition::BufferCollectionImportToken
  GetBufferCollectionImportToken() override;

  // |SurfaceProducerSurface|
  zx::event GetAcquireFence() override;

  // |SurfaceProducerSurface|
  zx::event GetReleaseFence() override;

  // |SurfaceProducerSurface|
  void SetReleaseImageCallback(
      ReleaseImageCallback release_image_callback) override;

 private:
  void OnSurfaceReadFinished(async_dispatcher_t* dispatcher,
                             async::WaitBase* wait,
                             zx_status_t status,
                             const zx_packet_signal_t* signal);

  bool SetupSkiaSurface(
      fuchsia::sysmem2::AllocatorSyncPtr& sysmem_allocator,
      fuchsia::ui::composition::AllocatorPtr& flatland_allocator,
      const SkISize& size);

  bool CreateFences();

  void Reset();
  uint32_t image_id_ = 0;

  sk_sp<SkSurface> sk_surface_;

  // This is associated with `release_event_` and allows detection of when
  // scenic has finished reading from the surface (and thus it is safe to re-use
  // for writing).
  async::WaitMethod<SoftwareSurface, &SoftwareSurface::OnSurfaceReadFinished>
      wait_for_surface_read_finished_;
  // Called when scenic has finished reading from the surface, to allow
  // `SoftwareSurfaceProducer` to re-use the surface.
  std::function<void()> surface_read_finished_callback_;
  // Called when the surface is destroyed, to allow
  // `ExternalViewEmbedder` to release the associated Flatland image.
  ReleaseImageCallback release_image_callback_;

  // Allows Flatland to associate this surface with a Flatland Image.
  fuchsia::ui::composition::BufferCollectionImportToken import_token_;
  zx::event acquire_event_;  // Signals to scenic that writing is finished.
  zx::event release_event_;  // Signalled by scenic that reading is finished.
  zx::vmo surface_vmo_;      // VMO that is backing the surface memory.

  uint32_t surface_size_bytes_;  // Size of the surface memory, in bytes.
  size_t age_{0};  // Number of frames since surface was last written to.

  bool needs_cache_clean_{false};
  bool valid_{false};

  FML_DISALLOW_COPY_AND_ASSIGN(SoftwareSurface);
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SOFTWARE_SURFACE_H_
