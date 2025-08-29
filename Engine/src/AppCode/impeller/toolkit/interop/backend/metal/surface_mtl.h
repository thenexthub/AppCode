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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_METAL_SURFACE_MTL_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_METAL_SURFACE_MTL_H_

#include "impeller/toolkit/interop/surface.h"

namespace impeller::interop {

class SurfaceMTL final : public Surface {
 public:
  SurfaceMTL(Context& context, void* metal_drawable);

  SurfaceMTL(Context& context, std::shared_ptr<impeller::Surface> surface);

  ~SurfaceMTL();

  SurfaceMTL(const SurfaceMTL&) = delete;

  SurfaceMTL& operator=(const SurfaceMTL&) = delete;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_BACKEND_METAL_SURFACE_MTL_H_
