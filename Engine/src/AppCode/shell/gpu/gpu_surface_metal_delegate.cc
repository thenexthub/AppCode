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

#include "appcode/shell/gpu/gpu_surface_metal_delegate.h"

namespace appcode {

GPUSurfaceMetalDelegate::GPUSurfaceMetalDelegate(
    MTLRenderTargetType render_target_type)
    : render_target_type_(render_target_type) {}

GPUSurfaceMetalDelegate::~GPUSurfaceMetalDelegate() = default;

MTLRenderTargetType GPUSurfaceMetalDelegate::GetRenderTargetType() {
  return render_target_type_;
}

bool GPUSurfaceMetalDelegate::AllowsDrawingWhenGpuDisabled() const {
  return true;
}

}  // namespace appcode
