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

#include "impeller/renderer/backend/vulkan/formats_vk.h"

namespace impeller {

vk::PipelineDepthStencilStateCreateInfo ToVKPipelineDepthStencilStateCreateInfo(
    std::optional<DepthAttachmentDescriptor> depth,
    std::optional<StencilAttachmentDescriptor> front,
    std::optional<StencilAttachmentDescriptor> back) {
  vk::PipelineDepthStencilStateCreateInfo info;

  if (depth.has_value()) {
    info.depthTestEnable = true;
    info.depthWriteEnable = depth->depth_write_enabled;
    info.depthCompareOp = ToVKCompareOp(depth->depth_compare);
    info.minDepthBounds = 0.0f;
    info.maxDepthBounds = 1.0f;
  }

  if (front.has_value()) {
    info.stencilTestEnable = true;
    info.front = ToVKStencilOpState(*front);
  }

  if (back.has_value()) {
    info.stencilTestEnable = true;
    info.back = ToVKStencilOpState(*back);
  }

  return info;
}

}  // namespace impeller
