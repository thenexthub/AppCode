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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SOFTWARE_SURFACE_PRODUCER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SOFTWARE_SURFACE_PRODUCER_H_

#include <fuchsia/sysmem2/cpp/fidl.h>
#include <fuchsia/ui/composition/cpp/fidl.h>

#include <unordered_map>

#include "appcode/fml/macros.h"

#include "software_surface.h"

namespace appcode_runner {

class SoftwareSurfaceProducer final : public SurfaceProducer {
 public:
  // Only keep 12 surfaces at a time.
  static constexpr int kMaxSurfaces = 12;
  // If a surface doesn't get used for 3 or more generations, we discard it.
  static constexpr int kMaxSurfaceAge = 3;

  explicit SoftwareSurfaceProducer();
  ~SoftwareSurfaceProducer() override;

  bool IsValid() const { return valid_; }

  // |SurfaceProducer|
  GrDirectContext* gr_context() const override { return nullptr; }

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
  void SubmitSurface(std::unique_ptr<SurfaceProducerSurface> surface);
  std::unique_ptr<SoftwareSurface> CreateSurface(const SkISize& size);
  void RecycleSurface(std::unique_ptr<SoftwareSurface> surface);

  void RecyclePendingSurface(uintptr_t surface_key);

  void AgeAndCollectOldBuffers();

  void TraceStats();

  fuchsia::sysmem2::AllocatorSyncPtr sysmem_allocator_;
  fuchsia::ui::composition::AllocatorPtr flatland_allocator_;

  // These surfaces are available for re-use.
  std::vector<std::unique_ptr<SoftwareSurface>> available_surfaces_;
  // These surfaces have been written to, but scenic is not finished reading
  // from them yet.
  std::unordered_map<uintptr_t, std::unique_ptr<SoftwareSurface>>
      pending_surfaces_;

  size_t trace_surfaces_created_ = 0;
  size_t trace_surfaces_reused_ = 0;

  bool valid_ = false;

  FML_DISALLOW_COPY_AND_ASSIGN(SoftwareSurfaceProducer);
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SOFTWARE_SURFACE_PRODUCER_H_
