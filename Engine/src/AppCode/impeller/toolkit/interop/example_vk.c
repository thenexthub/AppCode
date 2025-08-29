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

#include <assert.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "impeller.h"

void GLFWErrorCallback(int error, const char* description) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
  fflush(stderr);
}

void* ProcAddressCallback(void* vulkan_instance,
                          const char* vulkan_proc_name,
                          void* user_data) {
  return glfwGetInstanceProcAddress(vulkan_instance, vulkan_proc_name);
}

int main(int argc, char const* argv[]) {
  glfwSetErrorCallback(GLFWErrorCallback);
  [[maybe_unused]] int result = glfwInit();
  assert(result == GLFW_TRUE);

  if (!glfwVulkanSupported()) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    fprintf(stderr, "Vulkan is not supported on this platform.\n");
    fflush(stderr);
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow* window =
      glfwCreateWindow(800, 600, "Impeller Example (Vulkan)", NULL, NULL);
  assert(window != NULL);

  ImpellerContextVulkanSettings vulkan_settings = {};
  vulkan_settings.proc_address_callback = &ProcAddressCallback;
  vulkan_settings.enable_vulkan_validation = true;
  ImpellerContext context =
      ImpellerContextCreateVulkanNew(IMPELLER_VERSION, &vulkan_settings);
  assert(context != NULL);

  ImpellerContextVulkanInfo info = {};
  [[maybe_unused]] bool info_result =
      ImpellerContextGetVulkanInfo(context, &info);
  assert(!!info_result);

  if (glfwGetPhysicalDevicePresentationSupport(
          info.vk_instance, info.vk_physical_device,
          info.graphics_queue_family_index) != GLFW_TRUE) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    fprintf(stderr, "Queue does not support presentation.\n");
    fflush(stderr);
    return -1;
  }

  VkSurfaceKHR vulkan_surface_khr;
  VkResult error = glfwCreateWindowSurface(info.vk_instance, window, NULL,
                                           &vulkan_surface_khr);
  if (error) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    fprintf(stderr, "Could not create Vulkan surface for presentation.\n");
    fflush(stderr);
    return -1;
  }

  int framebuffer_width, framebuffer_height;
  glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
  ImpellerVulkanSwapchain swapchain =
      ImpellerVulkanSwapchainCreateNew(context, vulkan_surface_khr);
  assert(swapchain != NULL);

  ImpellerDisplayList dl = NULL;

  {
    ImpellerDisplayListBuilder builder = ImpellerDisplayListBuilderNew(NULL);
    ImpellerPaint paint = ImpellerPaintNew();

    // Clear the background to a white color.
    ImpellerColor clear_color = {1.0, 1.0, 1.0, 1.0};
    ImpellerPaintSetColor(paint, &clear_color);
    ImpellerDisplayListBuilderDrawPaint(builder, paint);

    // Draw a red box.
    ImpellerColor box_color = {1.0, 0.0, 0.0, 1.0};
    ImpellerPaintSetColor(paint, &box_color);
    ImpellerRect box_rect = {10, 10, 100, 100};
    ImpellerDisplayListBuilderDrawRect(builder, &box_rect, paint);

    dl = ImpellerDisplayListBuilderCreateDisplayListNew(builder);

    ImpellerPaintRelease(paint);
    ImpellerDisplayListBuilderRelease(builder);
  }

  assert(dl != NULL);

  while (!glfwWindowShouldClose(window)) {
    glfwWaitEvents();

    ImpellerSurface surface =
        ImpellerVulkanSwapchainAcquireNextSurfaceNew(swapchain);
    assert(surface != NULL);
    ImpellerSurfaceDrawDisplayList(surface, dl);
    ImpellerSurfacePresent(surface);
    ImpellerSurfaceRelease(surface);
  }

  ImpellerDisplayListRelease(dl);
  ImpellerVulkanSwapchainRelease(swapchain);
  ImpellerContextRelease(context);

  glfwMakeContextCurrent(NULL);

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}
