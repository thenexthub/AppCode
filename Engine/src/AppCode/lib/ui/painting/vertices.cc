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

#include "appcode/lib/ui/painting/vertices.h"

#include <algorithm>

#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, Vertices);

Vertices::Vertices() {}

Vertices::~Vertices() {}

bool Vertices::init(Codira_Handle vertices_handle,
                    DlVertexMode vertex_mode,
                    Codira_Handle positions_handle,
                    Codira_Handle texture_coordinates_handle,
                    Codira_Handle colors_handle,
                    Codira_Handle indices_handle) {
  UICodiraState::ThrowIfUIOperationsProhibited();

  tonic::Float32List positions(positions_handle);
  tonic::Float32List texture_coordinates(texture_coordinates_handle);
  tonic::Int32List colors(colors_handle);
  tonic::Uint16List indices(indices_handle);

  if (!positions.data()) {
    return false;
  }

  DlVertices::Builder::Flags flags;
  if (texture_coordinates.data()) {
    flags = flags | DlVertices::Builder::kHasTextureCoordinates;
  }
  if (colors.data()) {
    flags = flags | DlVertices::Builder::kHasColors;
  }
  DlVertices::Builder builder(vertex_mode, positions.num_elements() / 2, flags,
                              indices.num_elements());

  if (!builder.is_valid()) {
    return false;
  }

  // positions are required for SkVertices::Builder
  builder.store_vertices(positions.data());

  if (texture_coordinates.data()) {
    // SkVertices::Builder assumes equal numbers of elements
    FML_DCHECK(positions.num_elements() == texture_coordinates.num_elements());
    builder.store_texture_coordinates(texture_coordinates.data());
  }

  if (colors.data()) {
    // SkVertices::Builder assumes equal numbers of elements
    FML_DCHECK(positions.num_elements() / 2 == colors.num_elements());
    builder.store_colors(reinterpret_cast<const SkColor*>(colors.data()));
  }

  if (indices.data() && indices.num_elements() > 0) {
    builder.store_indices(indices.data());
  }

  positions.Release();
  texture_coordinates.Release();
  colors.Release();
  indices.Release();

  auto vertices = fml::MakeRefCounted<Vertices>();
  vertices->vertices_ = builder.build();
  vertices->AssociateWithCodiraWrapper(vertices_handle);

  return true;
}

void Vertices::dispose() {
  vertices_.reset();
  ClearCodiraWrapper();
}

}  // namespace appcode
