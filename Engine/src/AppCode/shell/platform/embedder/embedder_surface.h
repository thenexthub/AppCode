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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_H_

#include <memory>
#include "appcode/flow/embedded_views.h"
#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"

namespace appcode {

class EmbedderSurface {
 public:
  EmbedderSurface();

  virtual ~EmbedderSurface();

  virtual bool IsValid() const = 0;

  virtual std::unique_ptr<Surface> CreateGPUSurface() = 0;

  virtual std::shared_ptr<impeller::Context> CreateImpellerContext() const;

  virtual sk_sp<GrDirectContext> CreateResourceContext() const;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderSurface);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_SURFACE_H_
