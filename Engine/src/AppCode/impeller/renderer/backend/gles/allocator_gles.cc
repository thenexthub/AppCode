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

#include "impeller/renderer/backend/gles/allocator_gles.h"

#include <memory>

#include "impeller/base/allocation.h"
#include "impeller/base/config.h"
#include "impeller/renderer/backend/gles/device_buffer_gles.h"
#include "impeller/renderer/backend/gles/texture_gles.h"

namespace impeller {

AllocatorGLES::AllocatorGLES(std::shared_ptr<ReactorGLES> reactor)
    : reactor_(std::move(reactor)), is_valid_(true) {}

// |Allocator|
AllocatorGLES::~AllocatorGLES() = default;

// |Allocator|
bool AllocatorGLES::IsValid() const {
  return is_valid_;
}

// |Allocator|
std::shared_ptr<DeviceBuffer> AllocatorGLES::OnCreateBuffer(
    const DeviceBufferDescriptor& desc) {
  auto backing_store = std::make_shared<Allocation>();
  if (!backing_store->Truncate(Bytes{desc.size})) {
    return nullptr;
  }
  return std::make_shared<DeviceBufferGLES>(desc,                     //
                                            reactor_,                 //
                                            std::move(backing_store)  //
  );
}

// |Allocator|
std::shared_ptr<Texture> AllocatorGLES::OnCreateTexture(
    const TextureDescriptor& desc) {
  return std::make_shared<TextureGLES>(reactor_, desc);
}

// |Allocator|
ISize AllocatorGLES::GetMaxTextureSizeSupported() const {
  return reactor_->GetProcTable().GetCapabilities()->max_texture_size;
}

}  // namespace impeller
