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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_BACKINGSTORE_PRODUCER_GL_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_BACKINGSTORE_PRODUCER_GL_H_

#include "appcode/shell/platform/embedder/tests/embedder_test_backingstore_producer.h"

#include <memory>

#include "appcode/testing/test_gl_context.h"

namespace appcode::testing {

class EmbedderTestBackingStoreProducerGL
    : public EmbedderTestBackingStoreProducer {
 public:
  EmbedderTestBackingStoreProducerGL(
      sk_sp<GrDirectContext> context,
      RenderTargetType type,
      std::shared_ptr<TestEGLContext> egl_context);

  virtual ~EmbedderTestBackingStoreProducerGL();

  bool Create(const FlutterBackingStoreConfig* config,
              FlutterBackingStore* backing_store_out) override;

  sk_sp<SkSurface> GetSurface(
      const FlutterBackingStore* backing_store) const override;

  sk_sp<SkImage> MakeImageSnapshot(
      const FlutterBackingStore* backing_store) const override;

 private:
  bool CreateFramebuffer(const FlutterBackingStoreConfig* config,
                         FlutterBackingStore* renderer_out);

  bool CreateTexture(const FlutterBackingStoreConfig* config,
                     FlutterBackingStore* renderer_out);

  bool CreateSurface(const FlutterBackingStoreConfig* config,
                     FlutterBackingStore* renderer_out);

  std::shared_ptr<TestEGLContext> test_egl_context_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderTestBackingStoreProducerGL);
};

}  // namespace appcode::testing

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_BACKINGSTORE_PRODUCER_GL_H_
