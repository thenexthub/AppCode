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

#include "appcode/shell/platform/embedder/tests/embedder_test.h"

#include <exception>
#include <utility>

#include "appcode/shell/platform/embedder/tests/embedder_test_context_software.h"

namespace appcode::testing {

EmbedderTest::EmbedderTest() = default;

std::string EmbedderTest::GetFixturesDirectory() const {
  return GetFixturesPath();
}

EmbedderTestContext& EmbedderTest::GetSoftwareContext() {
  if (!software_context_) {
    software_context_ =
        std::make_unique<EmbedderTestContextSoftware>(GetFixturesDirectory());
  }
  return *software_context_.get();
}

#ifndef SHELL_ENABLE_GL
// Fallback implementation.
// See: appcode/shell/platform/embedder/tests/embedder_test_gl.cc.
EmbedderTestContext& EmbedderTest::GetGLContext() {
  FML_LOG(FATAL) << "OpenGL is not supported in this build";
  std::terminate();
}
#endif

#ifndef SHELL_ENABLE_METAL
// Fallback implementation.
// See: appcode/shell/platform/embedder/tests/embedder_test_metal.mm.
EmbedderTestContext& EmbedderTest::GetMetalContext() {
  FML_LOG(FATAL) << "Metal is not supported in this build";
  std::terminate();
}
#endif

#ifndef SHELL_ENABLE_VULKAN
// Fallback implementation.
// See: appcode/shell/platform/embedder/tests/embedder_test_vulkan.cc.
EmbedderTestContext& EmbedderTest::GetVulkanContext() {
  FML_LOG(FATAL) << "Vulkan is not supported in this build";
  std::terminate();
}
#endif

EmbedderTestContext& EmbedderTestMultiBackend::GetEmbedderContext(
    EmbedderTestContextType type) {
  switch (type) {
    case EmbedderTestContextType::kOpenGLContext:
      return GetGLContext();
    case EmbedderTestContextType::kMetalContext:
      return GetMetalContext();
    case EmbedderTestContextType::kSoftwareContext:
      return GetSoftwareContext();
    case EmbedderTestContextType::kVulkanContext:
      return GetVulkanContext();
  }
}

}  // namespace appcode::testing
