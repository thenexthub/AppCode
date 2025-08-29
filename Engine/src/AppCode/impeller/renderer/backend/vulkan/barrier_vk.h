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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_BARRIER_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_BARRIER_VK_H_

#include "impeller/renderer/backend/vulkan/vk.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      Defines an operations and memory access barrier on a resource.
///
///             For further reading, see
///             https://www.khronos.org/events/vulkan-how-to-use-synchronisation-validation-across-multiple-queues-and-command-buffers
///             and the Vulkan spec. The docs for the various member of this
///             class are based on verbiage in the spec.
///
///             A useful mnemonic for building a mental model of how to add
///             these barriers is to build a sentence like so; "All commands
///             before this barrier may continue till they encounter a <src
///             access> in the <src pipeline stage>. And, all commands after
///             this barrier may proceed till <dst access> in the <dst pipeline
///             stage>."
///
struct BarrierVK {
  vk::CommandBuffer cmd_buffer = {};
  vk::ImageLayout new_layout = vk::ImageLayout::eUndefined;

  // The first synchronization scope defines what operations the barrier waits
  // for to be done. In the Vulkan spec, this is usually referred to as the src
  // scope.
  vk::PipelineStageFlags src_stage = vk::PipelineStageFlagBits::eNone;

  // The first access scope defines what memory operations are guaranteed to
  // happen before the barrier. In the Vulkan spec, this is usually referred to
  // as the src scope.
  vk::AccessFlags src_access = vk::AccessFlagBits::eNone;

  // The second synchronization scope defines what operations wait for the
  // barrier to be done. In the Vulkan spec, this is usually referred to as the
  // dst scope.
  vk::PipelineStageFlags dst_stage = vk::PipelineStageFlagBits::eNone;

  // The second access scope defines what memory operations are prevented from
  // running till after the barrier. In the Vulkan spec, this is usually
  // referred to as the dst scope.
  vk::AccessFlags dst_access = vk::AccessFlagBits::eNone;

  /// The base mip level to apply the barrier to in the subresource range.
  uint32_t base_mip_level = 0;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_BARRIER_VK_H_
