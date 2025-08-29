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

#ifndef APPCODE_TESTING_TEST_VULKAN_IMAGE_H_
#define APPCODE_TESTING_TEST_VULKAN_IMAGE_H_

#include "appcode/fml/macros.h"

#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/vulkan/procs/vulkan_handle.h"
#include "third_party/skia/include/core/SkSize.h"

namespace appcode::testing {

class TestVulkanContext;

/// Captures the lifetime of a test VkImage along with its bound memory.
class TestVulkanImage {
 public:
  TestVulkanImage(TestVulkanImage&& other);
  TestVulkanImage& operator=(TestVulkanImage&& other);

  ~TestVulkanImage();

  VkImage GetImage();

 private:
  TestVulkanImage();

  // The lifetime of the Vulkan state must exceed memory/image handles.
  fml::RefPtr<TestVulkanContext> context_;

  vulkan::VulkanHandle<VkImage> image_;
  vulkan::VulkanHandle<VkDeviceMemory> memory_;

  FML_DISALLOW_COPY_AND_ASSIGN(TestVulkanImage);

  friend TestVulkanContext;
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_VULKAN_IMAGE_H_
