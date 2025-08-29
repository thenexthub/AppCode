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

#ifndef APPCODE_IMPELLER_CORE_VERTEX_BUFFER_H_
#define APPCODE_IMPELLER_CORE_VERTEX_BUFFER_H_

#include "impeller/core/buffer_view.h"
#include "impeller/core/formats.h"

namespace impeller {

constexpr size_t kMaxVertexBuffers = 16;

struct VertexBuffer {
  BufferView vertex_buffer;

  //----------------------------------------------------------------------------
  /// The index buffer binding used by the vertex shader stage.
  BufferView index_buffer;

  //----------------------------------------------------------------------------
  /// The total count of vertices, either in the vertex_buffer if the
  /// index_type is IndexType::kNone or in the index_buffer otherwise.
  size_t vertex_count = 0u;

  //----------------------------------------------------------------------------
  /// The type of indices in the index buffer. The indices must be tightly
  /// packed in the index buffer.
  ///
  IndexType index_type = IndexType::kUnknown;

  constexpr explicit operator bool() const {
    return static_cast<bool>(vertex_buffer) &&
           (index_type == IndexType::kNone || static_cast<bool>(index_buffer));
  }
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_VERTEX_BUFFER_H_
