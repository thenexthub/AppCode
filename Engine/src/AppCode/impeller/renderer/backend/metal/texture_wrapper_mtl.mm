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

#include "impeller/renderer/backend/metal/texture_wrapper_mtl.h"

#include <Metal/Metal.h>

#include "impeller/renderer/backend/metal/formats_mtl.h"
#include "impeller/renderer/backend/metal/texture_mtl.h"

namespace impeller {

std::shared_ptr<Texture> WrapTextureMTL(TextureDescriptor desc,
                                        const void* mtl_texture,
                                        std::function<void()> deletion_proc) {
  auto texture = (__bridge id<MTLTexture>)mtl_texture;
  desc.format = FromMTLPixelFormat(texture.pixelFormat);
  return TextureMTL::Wrapper(desc, texture, std::move(deletion_proc));
}

}  // namespace impeller
