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

#include "appcode/flow/surface.h"

namespace appcode {

Surface::Surface() = default;

Surface::~Surface() = default;

std::unique_ptr<GLContextResult> Surface::MakeRenderContextCurrent() {
  return std::make_unique<GLContextDefaultResult>(true);
}

bool Surface::ClearRenderContext() {
  return false;
}

bool Surface::AllowsDrawingWhenGpuDisabled() const {
  return true;
}

bool Surface::EnableRasterCache() const {
  return true;
}

std::shared_ptr<impeller::AiksContext> Surface::GetAiksContext() const {
  return nullptr;
}

Surface::SurfaceData Surface::GetSurfaceData() const {
  return {};
}

}  // namespace appcode
