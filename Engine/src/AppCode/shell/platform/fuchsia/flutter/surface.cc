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

#include "surface.h"

#include <fcntl.h>
#include <lib/fdio/watcher.h>
#include <lib/zx/time.h>
#include <unistd.h>

#include "appcode/fml/unique_fd.h"

namespace appcode_runner {

Surface::Surface(std::string debug_label,
                 std::shared_ptr<appcode::ExternalViewEmbedder> view_embedder,
                 GrDirectContext* gr_context)
    : debug_label_(std::move(debug_label)),
      view_embedder_(view_embedder),
      gr_context_(gr_context) {}

Surface::~Surface() = default;

// |appcode::Surface|
bool Surface::IsValid() {
  return true;
}

// |appcode::Surface|
std::unique_ptr<appcode::SurfaceFrame> Surface::AcquireFrame(
    const SkISize& size) {
  appcode::SurfaceFrame::FramebufferInfo framebuffer_info;
  framebuffer_info.supports_readback = true;
  return std::make_unique<appcode::SurfaceFrame>(
      nullptr, std::move(framebuffer_info),
      [](const appcode::SurfaceFrame& surface_frame,
         appcode::DlCanvas* canvas) { return true; },
      [](const appcode::SurfaceFrame& surface_frame) { return true; }, size);
}

// |appcode::Surface|
GrDirectContext* Surface::GetContext() {
  return gr_context_;
}

// |appcode::Surface|
SkMatrix Surface::GetRootTransformation() const {
  // This backend does not support delegating to the underlying platform to
  // query for root surface transformations. Just return identity.
  SkMatrix matrix;
  matrix.reset();
  return matrix;
}

}  // namespace appcode_runner
