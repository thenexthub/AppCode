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

#ifndef APPCODE_TESTING_TEST_VULKAN_SURFACE_H_
#define APPCODE_TESTING_TEST_VULKAN_SURFACE_H_

#include <memory>
#include "appcode/testing/test_vulkan_context.h"

#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace appcode::testing {

class TestVulkanSurface {
 public:
  static std::unique_ptr<TestVulkanSurface> Create(
      const TestVulkanContext& context,
      const SkISize& surface_size);

  bool IsValid() const;

  sk_sp<SkImage> GetSurfaceSnapshot() const;

  VkImage GetImage();

 private:
  explicit TestVulkanSurface(TestVulkanImage&& image);

  TestVulkanImage image_;
  sk_sp<SkSurface> surface_;
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_VULKAN_SURFACE_H_
