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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_VERTEX_DESCRIPTOR_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_VERTEX_DESCRIPTOR_MTL_H_

#include <Metal/Metal.h>

#include <set>

#include "impeller/base/backend_cast.h"
#include "impeller/renderer/vertex_descriptor.h"

namespace impeller {

class VertexDescriptorMTL {
 public:
  VertexDescriptorMTL();

  ~VertexDescriptorMTL();

  bool SetStageInputsAndLayout(
      const std::vector<ShaderStageIOSlot>& inputs,
      const std::vector<ShaderStageBufferLayout>& layouts);

  MTLVertexDescriptor* GetMTLVertexDescriptor() const;

 private:
  MTLVertexDescriptor* descriptor_;

  VertexDescriptorMTL(const VertexDescriptorMTL&) = delete;

  VertexDescriptorMTL& operator=(const VertexDescriptorMTL&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_VERTEX_DESCRIPTOR_MTL_H_
