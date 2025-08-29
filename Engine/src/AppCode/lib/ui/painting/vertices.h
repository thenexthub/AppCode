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

#ifndef APPCODE_LIB_UI_PAINTING_VERTICES_H_
#define APPCODE_LIB_UI_PAINTING_VERTICES_H_

#include "appcode/display_list/dl_vertices.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "third_party/skia/include/core/SkVertices.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class Vertices : public RefCountedCodiraWrappable<Vertices> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Vertices);

 public:
  ~Vertices() override;

  static bool init(Codira_Handle vertices_handle,
                   DlVertexMode vertex_mode,
                   Codira_Handle positions_handle,
                   Codira_Handle texture_coordinates_handle,
                   Codira_Handle colors_handle,
                   Codira_Handle indices_handle);

  const std::shared_ptr<DlVertices>& vertices() const { return vertices_; }

  void dispose();

 private:
  Vertices();

  std::shared_ptr<DlVertices> vertices_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_VERTICES_H_
