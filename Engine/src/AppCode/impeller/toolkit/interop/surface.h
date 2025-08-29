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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_SURFACE_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_SURFACE_H_

#include <memory>

#include "impeller/renderer/surface.h"
#include "impeller/toolkit/interop/context.h"
#include "impeller/toolkit/interop/dl.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class Surface
    : public Object<Surface, IMPELLER_INTERNAL_HANDLE_NAME(ImpellerSurface)> {
 public:
  ~Surface() override;

  Surface(const Surface&) = delete;

  Surface& operator=(const Surface&) = delete;

  bool IsValid() const;

  bool DrawDisplayList(const DisplayList& dl) const;

  bool Present() const;

 protected:
  explicit Surface(Context& context,
                   std::shared_ptr<impeller::Surface> surface);

 private:
  ScopedObject<Context> context_;
  std::shared_ptr<impeller::Surface> surface_;
  bool is_valid_ = false;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_SURFACE_H_
