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

#include "impeller/playground/playground_impl.h"
#include "appcode/testing/testing.h"

#define GLFW_INCLUDE_NONE
#include "third_party/glfw/include/GLFW/glfw3.h"

#if IMPELLER_ENABLE_METAL
#include "impeller/playground/backend/metal/playground_impl_mtl.h"
#endif  // IMPELLER_ENABLE_METAL

#if IMPELLER_ENABLE_OPENGLES
#include "impeller/playground/backend/gles/playground_impl_gles.h"
#endif  // IMPELLER_ENABLE_OPENGLES

#if IMPELLER_ENABLE_VULKAN
#include "impeller/playground/backend/vulkan/playground_impl_vk.h"
#endif  // IMPELLER_ENABLE_VULKAN

namespace impeller {

std::unique_ptr<PlaygroundImpl> PlaygroundImpl::Create(
    PlaygroundBackend backend,
    PlaygroundSwitches switches) {
  switch (backend) {
#if IMPELLER_ENABLE_METAL
    case PlaygroundBackend::kMetal:
      return std::make_unique<PlaygroundImplMTL>(switches);
#endif  // IMPELLER_ENABLE_METAL
#if IMPELLER_ENABLE_OPENGLES
    case PlaygroundBackend::kOpenGLES:
      return std::make_unique<PlaygroundImplGLES>(switches);
#endif  // IMPELLER_ENABLE_OPENGLES
#if IMPELLER_ENABLE_VULKAN
    case PlaygroundBackend::kVulkan:
      if (!PlaygroundImplVK::IsVulkanDriverPresent()) {
        FML_CHECK(false) << "Attempted to create playground with backend that "
                            "isn't available or was disabled on this platform: "
                         << PlaygroundBackendToString(backend);
      }
      switches.enable_vulkan_validation = true;
      return std::make_unique<PlaygroundImplVK>(switches);
#endif  // IMPELLER_ENABLE_VULKAN
    default:
      FML_CHECK(false) << "Attempted to create playground with backend that "
                          "isn't available or was disabled on this platform: "
                       << PlaygroundBackendToString(backend);
  }
  FML_UNREACHABLE();
}

PlaygroundImpl::PlaygroundImpl(PlaygroundSwitches switches)
    : switches_(switches) {}

PlaygroundImpl::~PlaygroundImpl() = default;

Vector2 PlaygroundImpl::GetContentScale() const {
  auto window = reinterpret_cast<GLFWwindow*>(GetWindowHandle());

  Vector2 scale(1, 1);
  ::glfwGetWindowContentScale(window, &scale.x, &scale.y);

  return scale;
}

Playground::GLProcAddressResolver PlaygroundImpl::CreateGLProcAddressResolver()
    const {
  return nullptr;
}

Playground::VKProcAddressResolver PlaygroundImpl::CreateVKProcAddressResolver()
    const {
  return nullptr;
}

}  // namespace impeller
