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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SURFACE_PRODUCER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SURFACE_PRODUCER_H_

#include <fuchsia/sysmem2/cpp/fidl.h>
#include <fuchsia/ui/composition/cpp/fidl.h>
#include <lib/zx/event.h>

#include <functional>
#include <memory>
#include <vector>

#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace appcode_runner {

using ReleaseImageCallback = std::function<void()>;

// This represents an abstract notion of a "rendering surface", which is a
// destination for pixels drawn by some rendering engine.  In this case, the
// rendering engine is Skia combined with one of its rendering backends.
//
// In addition to allowing Skia-based drawing via `GetSkiaSurface`, this
// rendering surface can be shared with Scenic via `SetImageId`,
// `GetBufferCollectionImportToken`, `GetAcquireFene`, and `GetReleaseFence`.
class SurfaceProducerSurface {
 public:
  virtual ~SurfaceProducerSurface() = default;

  virtual bool IsValid() const = 0;

  virtual SkISize GetSize() const = 0;

  virtual void SetImageId(uint32_t image_id) = 0;

  virtual uint32_t GetImageId() = 0;

  virtual sk_sp<SkSurface> GetSkiaSurface() const = 0;

  virtual fuchsia::ui::composition::BufferCollectionImportToken
  GetBufferCollectionImportToken() = 0;

  virtual zx::event GetAcquireFence() = 0;

  virtual zx::event GetReleaseFence() = 0;

  virtual void SetReleaseImageCallback(
      ReleaseImageCallback release_image_callback) = 0;

  virtual size_t AdvanceAndGetAge() = 0;

  virtual bool FlushSessionAcquireAndReleaseEvents() = 0;

  virtual void SignalWritesFinished(
      const std::function<void(void)>& on_writes_committed) = 0;
};

// This represents an abstract notion of "surface producer", which serves as a
// source for `SurfaceProducerSurface`s.  Produces surfaces should be returned
// to this `SurfaceProducer` via `SubmitSurfaces`, at which point they will be
// shared with Scenic.
class SurfaceProducer {
 public:
  virtual ~SurfaceProducer() = default;

  virtual GrDirectContext* gr_context() const = 0;

  virtual std::unique_ptr<SurfaceProducerSurface> ProduceOffscreenSurface(
      const SkISize& size) = 0;
  virtual std::unique_ptr<SurfaceProducerSurface> ProduceSurface(
      const SkISize& size) = 0;

  virtual void SubmitSurfaces(
      std::vector<std::unique_ptr<SurfaceProducerSurface>> surfaces) = 0;
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_SURFACE_PRODUCER_H_
