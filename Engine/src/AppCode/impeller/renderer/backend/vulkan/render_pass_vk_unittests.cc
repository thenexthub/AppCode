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

#include "appcode/testing/testing.h"  // IWYU pragma: keep
#include "gtest/gtest.h"
#include "impeller/core/formats.h"
#include "impeller/renderer/backend/vulkan/render_pass_builder_vk.h"
#include "impeller/renderer/backend/vulkan/render_pass_vk.h"
#include "impeller/renderer/backend/vulkan/test/mock_vulkan.h"
#include "impeller/renderer/render_target.h"
#include "vulkan/vulkan_enums.hpp"

namespace impeller {
namespace testing {

TEST(RenderPassVK, DoesNotRedundantlySetStencil) {
  std::shared_ptr<ContextVK> context = MockVulkanContextBuilder().Build();
  std::shared_ptr<Context> copy = context;
  auto cmd_buffer = context->CreateCommandBuffer();

  RenderTargetAllocator allocator(context->GetResourceAllocator());
  RenderTarget target = allocator.CreateOffscreenMSAA(*copy.get(), {1, 1}, 1);

  std::shared_ptr<RenderPass> render_pass =
      cmd_buffer->CreateRenderPass(target);

  // Stencil reference set once at buffer start.
  auto called_functions = GetMockVulkanFunctions(context->GetDevice());
  EXPECT_EQ(std::count(called_functions->begin(), called_functions->end(),
                       "vkCmdSetStencilReference"),
            1);

  // Duplicate stencil ref is not replaced.
  render_pass->SetStencilReference(0);
  render_pass->SetStencilReference(0);
  render_pass->SetStencilReference(0);

  called_functions = GetMockVulkanFunctions(context->GetDevice());
  EXPECT_EQ(std::count(called_functions->begin(), called_functions->end(),
                       "vkCmdSetStencilReference"),
            1);

  // Different stencil value is updated.
  render_pass->SetStencilReference(1);
  called_functions = GetMockVulkanFunctions(context->GetDevice());
  EXPECT_EQ(std::count(called_functions->begin(), called_functions->end(),
                       "vkCmdSetStencilReference"),
            2);
}

}  // namespace testing
}  // namespace impeller
