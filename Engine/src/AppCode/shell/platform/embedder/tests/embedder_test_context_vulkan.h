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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_VULKAN_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_VULKAN_H_

#include <memory>
#include "appcode/shell/platform/embedder/tests/embedder_test_context.h"
#include "appcode/testing/test_vulkan_context.h"
#include "appcode/vulkan/vulkan_application.h"
#include "testing/test_vulkan_surface.h"

namespace appcode {
namespace testing {

class EmbedderTestContextVulkan : public EmbedderTestContext {
 public:
  explicit EmbedderTestContextVulkan(std::string assets_path = "");

  ~EmbedderTestContextVulkan() override;

  // |EmbedderTestContext|
  EmbedderTestContextType GetContextType() const override;

  // |EmbedderTestContext|
  size_t GetSurfacePresentCount() const override;

  VkImage GetNextImage(const SkISize& size);

  bool PresentImage(VkImage image);

  void SetVulkanInstanceProcAddressCallback(
      FlutterVulkanInstanceProcAddressCallback callback);

  static void* InstanceProcAddr(void* user_data,
                                FlutterVulkanInstanceHandle instance,
                                const char* name);

 private:
  // |EmbedderTestContext|
  void SetSurface(SkISize surface_size) override;

  // |EmbedderTestContext|
  void SetupCompositor() override;

  // The TestVulkanContext destructor must be called _after_ the compositor is
  // freed.
  fml::RefPtr<TestVulkanContext> vulkan_context_ = nullptr;

  std::unique_ptr<TestVulkanSurface> surface_;

  SkISize surface_size_ = SkISize::MakeEmpty();
  size_t present_count_ = 0;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderTestContextVulkan);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_CONTEXT_VULKAN_H_
