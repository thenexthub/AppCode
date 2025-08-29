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

#include "appcode/testing/testing.h"
#include "gtest/gtest.h"
#include "impeller/core/formats.h"
#include "impeller/playground/playground_test.h"
#include "impeller/renderer/backend/vulkan/texture_vk.h"
#include "impeller/renderer/render_target.h"

namespace impeller {
namespace testing {

using RendererTest = PlaygroundTest;

TEST_P(RendererTest, CachesRenderPassAndFramebuffer) {
  if (GetBackend() != PlaygroundBackend::kVulkan) {
    GTEST_SKIP() << "Test only applies to Vulkan";
  }

  auto allocator = std::make_shared<RenderTargetAllocator>(
      GetContext()->GetResourceAllocator());

  RenderTarget render_target =
      allocator->CreateOffscreenMSAA(*GetContext(), {100, 100}, 1);
  std::shared_ptr<Texture> resolve_texture =
      render_target.GetColorAttachment(0).resolve_texture;
  TextureVK& texture_vk = TextureVK::Cast(*resolve_texture);

  EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount4).framebuffer,
            nullptr);
  EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount4).render_pass,
            nullptr);

  auto buffer = GetContext()->CreateCommandBuffer();
  auto render_pass = buffer->CreateRenderPass(render_target);

  EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount4).framebuffer,
            nullptr);
  EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount4).render_pass,
            nullptr);

  render_pass->EncodeCommands();
  EXPECT_TRUE(GetContext()->GetCommandQueue()->Submit({buffer}).ok());

  // Can be reused without error.
  auto buffer_2 = GetContext()->CreateCommandBuffer();
  auto render_pass_2 = buffer_2->CreateRenderPass(render_target);

  EXPECT_TRUE(render_pass_2->EncodeCommands());
  EXPECT_TRUE(GetContext()->GetCommandQueue()->Submit({buffer_2}).ok());
}

TEST_P(RendererTest, CachesRenderPassAndFramebufferNonMSAA) {
  if (GetBackend() != PlaygroundBackend::kVulkan) {
    GTEST_SKIP() << "Test only applies to Vulkan";
  }

  auto allocator = std::make_shared<RenderTargetAllocator>(
      GetContext()->GetResourceAllocator());

  RenderTarget render_target =
      allocator->CreateOffscreen(*GetContext(), {100, 100}, 1);
  std::shared_ptr<Texture> color_texture =
      render_target.GetColorAttachment(0).texture;
  TextureVK& texture_vk = TextureVK::Cast(*color_texture);

  EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount1).framebuffer,
            nullptr);
  EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount1).render_pass,
            nullptr);

  auto buffer = GetContext()->CreateCommandBuffer();
  auto render_pass = buffer->CreateRenderPass(render_target);

  EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount1).framebuffer,
            nullptr);
  EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount1).render_pass,
            nullptr);

  render_pass->EncodeCommands();
  EXPECT_TRUE(GetContext()->GetCommandQueue()->Submit({buffer}).ok());

  // Can be reused without error.
  auto buffer_2 = GetContext()->CreateCommandBuffer();
  auto render_pass_2 = buffer_2->CreateRenderPass(render_target);

  EXPECT_TRUE(render_pass_2->EncodeCommands());
  EXPECT_TRUE(GetContext()->GetCommandQueue()->Submit({buffer_2}).ok());
}

TEST_P(RendererTest, CachesRenderPassAndFramebufferMixed) {
  if (GetBackend() != PlaygroundBackend::kVulkan) {
    GTEST_SKIP() << "Test only applies to Vulkan";
  }

  auto allocator = std::make_shared<RenderTargetAllocator>(
      GetContext()->GetResourceAllocator());

  RenderTarget render_target =
      allocator->CreateOffscreenMSAA(*GetContext(), {100, 100}, 1);
  std::shared_ptr<Texture> resolve_texture =
      render_target.GetColorAttachment(0).resolve_texture;
  TextureVK& texture_vk = TextureVK::Cast(*resolve_texture);

  EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount4).framebuffer,
            nullptr);
  EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount4).render_pass,
            nullptr);

  auto buffer = GetContext()->CreateCommandBuffer();
  auto render_pass = buffer->CreateRenderPass(render_target);

  EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount4).framebuffer,
            nullptr);
  EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount4).render_pass,
            nullptr);

  render_pass->EncodeCommands();
  EXPECT_TRUE(GetContext()->GetCommandQueue()->Submit({buffer}).ok());

  // Can be reused without error.
  auto buffer_2 = GetContext()->CreateCommandBuffer();
  auto render_pass_2 = buffer_2->CreateRenderPass(render_target);

  EXPECT_TRUE(render_pass_2->EncodeCommands());
  EXPECT_TRUE(GetContext()->GetCommandQueue()->Submit({buffer_2}).ok());

  // Now switch to single sample count and demonstrate no validation errors.
  {
    RenderTarget other_target;
    ColorAttachment color0;
    color0.load_action = LoadAction::kLoad;
    color0.store_action = StoreAction::kStore;
    color0.texture = resolve_texture;
    other_target.SetColorAttachment(color0, 0);
    other_target.SetDepthAttachment(std::nullopt);
    other_target.SetStencilAttachment(std::nullopt);

    EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount1).framebuffer,
              nullptr);
    EXPECT_EQ(texture_vk.GetCachedFrameData(SampleCount::kCount1).render_pass,
              nullptr);

    auto buffer_3 = GetContext()->CreateCommandBuffer();
    auto render_pass_3 = buffer_3->CreateRenderPass(other_target);

    EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount1).framebuffer,
              nullptr);
    EXPECT_NE(texture_vk.GetCachedFrameData(SampleCount::kCount1).render_pass,
              nullptr);

    EXPECT_TRUE(render_pass_3->EncodeCommands());
    EXPECT_TRUE(GetContext()->GetCommandQueue()->Submit({buffer_3}).ok());
  }
}

}  // namespace testing
}  // namespace impeller
