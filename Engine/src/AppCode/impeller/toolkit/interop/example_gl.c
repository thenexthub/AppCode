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

#include "GLFW/glfw3.h"
#include "impeller.h"

void GLFWErrorCallback(int error, const char* description) {
  // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
  fflush(stderr);
}

void* ProcAddressCallback(const char* proc_name, void* user_data) {
  return glfwGetProcAddress(proc_name);
}

int main(int argc, char const* argv[]) {
  glfwSetErrorCallback(GLFWErrorCallback);
  [[maybe_unused]] int result = glfwInit();
  assert(result == GLFW_TRUE);

  if (glfwGetPlatform() == GLFW_PLATFORM_COCOA) {
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    fprintf(stderr,
            "OpenGL(ES) is not available on macOS. Please use Metal or Vulkan "
            "instead.\n");
    fflush(stderr);
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

  GLFWwindow* window =
      glfwCreateWindow(800, 600, "Impeller Example (OpenGL)", NULL, NULL);
  assert(window != NULL);

  int framebuffer_width, framebuffer_height;
  glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

  // The GL context must be current on the calling thread.
  glfwMakeContextCurrent(window);

  ImpellerContext context = ImpellerContextCreateOpenGLESNew(
      IMPELLER_VERSION, ProcAddressCallback, NULL);
  assert(context != NULL);

  ImpellerISize surface_size = {};
  surface_size.width = framebuffer_width;
  surface_size.height = framebuffer_height;

  ImpellerSurface surface = ImpellerSurfaceCreateWrappedFBONew(
      context, 0u, kImpellerPixelFormatRGBA8888, &surface_size);
  assert(surface != NULL);

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
    ImpellerSurfaceDrawDisplayList(surface, dl);
    glfwSwapBuffers(window);
  }

  ImpellerDisplayListRelease(dl);
  ImpellerSurfaceRelease(surface);
  ImpellerContextRelease(context);

  glfwMakeContextCurrent(NULL);

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}
