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

#ifndef APPCODE_TESTING_TEST_METAL_CONTEXT_H_
#define APPCODE_TESTING_TEST_METAL_CONTEXT_H_

#include <map>
#include <memory>
#include <mutex>

#include <Metal/Metal.h>

#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"
#include "third_party/skia/include/ports/SkCFObject.h"

namespace appcode::testing {

struct MetalObjCFields;

class TestMetalContext {
 public:
  struct TextureInfo {
    int64_t texture_id;
    void* texture;
  };

  TestMetalContext();

  ~TestMetalContext();

  id<MTLDevice> GetMetalDevice() const;

  id<MTLCommandQueue> GetMetalCommandQueue() const;

  sk_sp<GrDirectContext> GetSkiaContext() const;

  /// Returns texture_id = -1 when texture creation fails.
  TextureInfo CreateMetalTexture(const SkISize& size);

  bool Present(int64_t texture_id);

  TextureInfo GetTextureInfo(int64_t texture_id);

 private:
  id<MTLDevice> device_;
  id<MTLCommandQueue> command_queue_;
  sk_sp<GrDirectContext> skia_context_;
  std::mutex textures_mutex_;
  int64_t texture_id_ctr_ = 1;                 // guarded by textures_mutex
  std::map<int64_t, sk_cfp<void*>> textures_;  // guarded by textures_mutex
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_METAL_CONTEXT_H_
