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

#ifndef APPCODE_TESTING_TEST_VULKAN_CONTEXT_H_
#define APPCODE_TESTING_TEST_VULKAN_CONTEXT_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/testing/test_vulkan_image.h"
#include "appcode/vulkan/procs/vulkan_proc_table.h"
#include "appcode/vulkan/vulkan_application.h"
#include "appcode/vulkan/vulkan_device.h"

#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace appcode::testing {

class TestVulkanContext : public fml::RefCountedThreadSafe<TestVulkanContext> {
 public:
  TestVulkanContext();
  ~TestVulkanContext();

  std::optional<TestVulkanImage> CreateImage(const SkISize& size) const;

  sk_sp<GrDirectContext> GetGrDirectContext() const;

 private:
  fml::RefPtr<vulkan::VulkanProcTable> vk_;
  std::unique_ptr<vulkan::VulkanApplication> application_;
  std::unique_ptr<vulkan::VulkanDevice> device_;

  sk_sp<GrDirectContext> context_;

  friend class EmbedderTestContextVulkan;
  friend class EmbedderConfigBuilder;

  FML_FRIEND_MAKE_REF_COUNTED(TestVulkanContext);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(TestVulkanContext);
  FML_DISALLOW_COPY_AND_ASSIGN(TestVulkanContext);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_VULKAN_CONTEXT_H_
