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

#ifndef APPCODE_IMPELLER_RENDERER_SURFACE_H_
#define APPCODE_IMPELLER_RENDERER_SURFACE_H_

#include "impeller/renderer/render_target.h"

namespace impeller {

class Surface {
 public:
  Surface();

  explicit Surface(const RenderTarget& target_desc);

  virtual ~Surface();

  const ISize& GetSize() const;

  bool IsValid() const;

  const RenderTarget& GetRenderTarget() const;

  virtual bool Present() const;

 private:
  RenderTarget desc_;
  ISize size_;

  bool is_valid_ = false;

  Surface(const Surface&) = delete;

  Surface& operator=(const Surface&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_SURFACE_H_
