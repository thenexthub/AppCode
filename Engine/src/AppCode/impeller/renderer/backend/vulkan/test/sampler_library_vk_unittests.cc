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

#include <memory>
#include "appcode/testing/testing.h"  // IWYU pragma: keep
#include "gtest/gtest.h"
#include "impeller/core/formats.h"
#include "impeller/core/sampler_descriptor.h"
#include "impeller/renderer/backend/vulkan/command_pool_vk.h"
#include "impeller/renderer/backend/vulkan/sampler_library_vk.h"
#include "impeller/renderer/backend/vulkan/test/mock_vulkan.h"
#include "impeller/renderer/backend/vulkan/workarounds_vk.h"

namespace impeller {
namespace testing {

TEST(SamplerLibraryVK, WorkaroundsCanDisableReadingFromMipLevels) {
  auto const context = MockVulkanContextBuilder().Build();

  auto library_vk =
      std::make_shared<SamplerLibraryVK>(context->GetDeviceHolder());
  std::shared_ptr<SamplerLibrary> library = library_vk;

  SamplerDescriptor desc;
  desc.mip_filter = MipFilter::kLinear;

  auto sampler = library->GetSampler(desc);
  EXPECT_EQ(sampler->GetDescriptor().mip_filter, MipFilter::kLinear);

  // Apply mips disabled workaround.
  library_vk->ApplyWorkarounds(WorkaroundsVK{.broken_mipmap_generation = true});

  sampler = library->GetSampler(desc);
  EXPECT_EQ(sampler->GetDescriptor().mip_filter, MipFilter::kBase);
}

}  // namespace testing
}  // namespace impeller
