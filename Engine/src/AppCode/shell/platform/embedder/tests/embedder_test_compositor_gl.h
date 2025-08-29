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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_COMPOSITOR_GL_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_COMPOSITOR_GL_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/tests/embedder_test_compositor.h"
#include "appcode/testing/test_gl_context.h"

namespace appcode {
namespace testing {

class EmbedderTestCompositorGL : public EmbedderTestCompositor {
 public:
  EmbedderTestCompositorGL(std::shared_ptr<TestEGLContext> egl_context,
                           SkISize surface_size,
                           sk_sp<GrDirectContext> context);

  ~EmbedderTestCompositorGL() override;

  void SetRenderTargetType(
      EmbedderTestBackingStoreProducer::RenderTargetType type,
      FlutterSoftwarePixelFormat software_pixfmt) override;

 private:
  std::shared_ptr<TestEGLContext> egl_context_;
  bool UpdateOffscrenComposition(const FlutterLayer** layers,
                                 size_t layers_count) override;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderTestCompositorGL);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_COMPOSITOR_GL_H_
