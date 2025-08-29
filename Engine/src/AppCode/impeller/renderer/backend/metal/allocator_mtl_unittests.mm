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

#include "flutter/testing/testing.h"
#include "impeller/core/device_buffer_descriptor.h"
#include "impeller/core/formats.h"
#include "impeller/core/texture_descriptor.h"
#include "impeller/playground/playground_test.h"
#include "impeller/renderer/backend/metal/allocator_mtl.h"
#include "impeller/renderer/backend/metal/context_mtl.h"
#include "impeller/renderer/backend/metal/formats_mtl.h"
#include "impeller/renderer/backend/metal/texture_mtl.h"
#include "impeller/renderer/capabilities.h"

#include <QuartzCore/CAMetalLayer.h>
#include <memory>
#include <thread>

#include "gtest/gtest.h"

namespace impeller {
namespace testing {

using AllocatorMTLTest = PlaygroundTest;
INSTANTIATE_METAL_PLAYGROUND_SUITE(AllocatorMTLTest);

TEST_P(AllocatorMTLTest, DebugTraceMemoryStatistics) {
  auto& context_mtl = ContextMTL::Cast(*GetContext());
  const auto& allocator = context_mtl.GetResourceAllocator();

  EXPECT_EQ(allocator->DebugGetHeapUsage().ConvertTo<MebiBytes>().GetSize(),
            0u);

  // Memoryless texture does not increase allocated size.
  {
    TextureDescriptor desc;
    desc.format = PixelFormat::kR8G8B8A8UNormInt;
    desc.storage_mode = StorageMode::kDeviceTransient;
    desc.size = {1024, 1024};
    auto texture_1 = allocator->CreateTexture(desc);

    // Private storage texture increases allocated size.
    desc.storage_mode = StorageMode::kDevicePrivate;
    auto texture_2 = allocator->CreateTexture(desc);

#ifdef IMPELLER_DEBUG
    EXPECT_EQ(allocator->DebugGetHeapUsage().ConvertTo<MebiBytes>().GetSize(),
              4u);
#else
    EXPECT_EQ(allocator->DebugGetHeapUsage().ConvertTo<MebiBytes>().GetSize(),
              0u);
#endif  // IMPELLER_DEBUG

    // Host storage texture increases allocated size.
    desc.storage_mode = StorageMode::kHostVisible;
    auto texture_3 = allocator->CreateTexture(desc);

#ifdef IMPELLER_DEBUG
    EXPECT_EQ(allocator->DebugGetHeapUsage().ConvertTo<MebiBytes>().GetSize(),
              8u);
#else
    EXPECT_EQ(allocator->DebugGetHeapUsage().ConvertTo<MebiBytes>().GetSize(),
              0u);
#endif  // IMPELLER_DEBUG
  }

  // After all textures are out of scope, memory has been decremented.
  EXPECT_EQ(allocator->DebugGetHeapUsage().ConvertTo<MebiBytes>().GetSize(),
            0u);
}

TEST_P(AllocatorMTLTest, ManagedMemory) {
  auto& context_mtl = ContextMTL::Cast(*GetContext());
  auto allocator = std::make_unique<AllocatorMTL>(context_mtl.GetMTLDevice(),
                                                  "test-allocator");
  allocator->DebugSetSupportsUMA(false);

  DeviceBufferDescriptor desc;
  desc.size = 100;
  desc.storage_mode = StorageMode::kHostVisible;

  auto buffer = allocator->CreateBuffer(desc);
  ASSERT_TRUE(buffer);

  EXPECT_NE(buffer->OnGetContents(), nullptr);
}

}  // namespace testing
}  // namespace impeller
