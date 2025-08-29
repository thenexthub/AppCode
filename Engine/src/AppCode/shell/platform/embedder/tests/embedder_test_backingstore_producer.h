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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_BACKINGSTORE_PRODUCER_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_BACKINGSTORE_PRODUCER_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_ptr_internal.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace appcode::testing {

class EmbedderTestBackingStoreProducer {
 public:
  enum class RenderTargetType {
    kSoftwareBuffer,
    kSoftwareBuffer2,
    kOpenGLFramebuffer,
    kOpenGLTexture,
    kOpenGLSurface,
    kMetalTexture,
    kVulkanImage,
  };

  EmbedderTestBackingStoreProducer(sk_sp<GrDirectContext> context,
                                   RenderTargetType type);
  virtual ~EmbedderTestBackingStoreProducer();

  virtual bool Create(const FlutterBackingStoreConfig* config,
                      FlutterBackingStore* backing_store_out) = 0;

  virtual sk_sp<SkSurface> GetSurface(
      const FlutterBackingStore* backing_store) const = 0;

  virtual sk_sp<SkImage> MakeImageSnapshot(
      const FlutterBackingStore* backing_store) const = 0;

 protected:
  sk_sp<GrDirectContext> context_;
  RenderTargetType type_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderTestBackingStoreProducer);
};

}  // namespace appcode::testing

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_TESTS_EMBEDDER_TEST_BACKINGSTORE_PRODUCER_H_
