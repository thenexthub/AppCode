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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_GLES_ALLOCATOR_GLES_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_GLES_ALLOCATOR_GLES_H_

#include "impeller/core/allocator.h"
#include "impeller/renderer/backend/gles/reactor_gles.h"

namespace impeller {

class AllocatorGLES final : public Allocator {
 public:
  // |Allocator|
  ~AllocatorGLES() override;

 private:
  friend class ContextGLES;

  std::shared_ptr<ReactorGLES> reactor_;
  bool is_valid_ = false;

  explicit AllocatorGLES(std::shared_ptr<ReactorGLES> reactor);

  // |Allocator|
  bool IsValid() const;

  // |Allocator|
  std::shared_ptr<DeviceBuffer> OnCreateBuffer(
      const DeviceBufferDescriptor& desc) override;

  // |Allocator|
  std::shared_ptr<Texture> OnCreateTexture(
      const TextureDescriptor& desc) override;

  // |Allocator|
  ISize GetMaxTextureSizeSupported() const override;

  AllocatorGLES(const AllocatorGLES&) = delete;

  AllocatorGLES& operator=(const AllocatorGLES&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_GLES_ALLOCATOR_GLES_H_
