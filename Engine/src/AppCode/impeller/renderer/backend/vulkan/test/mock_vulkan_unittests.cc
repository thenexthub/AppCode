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
#include "impeller/renderer/backend/vulkan/command_pool_vk.h"
#include "impeller/renderer/backend/vulkan/test/mock_vulkan.h"
#include "vulkan/vulkan_enums.hpp"

namespace impeller {
namespace testing {

TEST(MockVulkanContextTest, IsThreadSafe) {
  // In a typical app, there is a single ContextVK per app, shared b/w threads.
  //
  // This test ensures that the (mock) ContextVK is thread-safe.
  auto const context = MockVulkanContextBuilder().Build();

  // Spawn two threads, and have them create a CommandPoolVK each.
  std::thread thread1([&context]() {
    auto const pool = context->GetCommandPoolRecycler()->Get();
    EXPECT_TRUE(pool);
  });

  std::thread thread2([&context]() {
    auto const pool = context->GetCommandPoolRecycler()->Get();
    EXPECT_TRUE(pool);
  });

  thread1.join();
  thread2.join();

  context->Shutdown();
}

TEST(MockVulkanContextTest, DefaultFenceAlwaysReportsSuccess) {
  auto const context = MockVulkanContextBuilder().Build();
  auto const device = context->GetDevice();

  auto fence = device.createFenceUnique({}).value;
  EXPECT_EQ(vk::Result::eSuccess, device.getFenceStatus(*fence));
}

TEST(MockVulkanContextTest, MockedFenceReportsStatus) {
  auto const context = MockVulkanContextBuilder().Build();

  auto const device = context->GetDevice();
  auto fence = device.createFenceUnique({}).value;
  MockFence::SetStatus(fence, vk::Result::eNotReady);

  EXPECT_EQ(vk::Result::eNotReady, device.getFenceStatus(fence.get()));

  MockFence::SetStatus(fence, vk::Result::eSuccess);
  EXPECT_EQ(vk::Result::eSuccess, device.getFenceStatus(*fence));
}

}  // namespace testing
}  // namespace impeller
