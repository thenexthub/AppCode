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

#ifndef APPCODE_FLOW_LAYERS_OFFSCREEN_SURFACE_H_
#define APPCODE_FLOW_LAYERS_OFFSCREEN_SURFACE_H_

#if !SLIMPELLER

#include "appcode/fml/logging.h"
#include "appcode/fml/macros.h"

#include "appcode/display_list/dl_canvas.h"
#include "appcode/display_list/skia/dl_sk_canvas.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkSurface.h"

class GrDirectContext;

namespace appcode {

class OffscreenSurface {
 public:
  explicit OffscreenSurface(GrDirectContext* surface_context,
                            const DlISize& size);

  ~OffscreenSurface() = default;

  sk_sp<SkData> GetRasterData(bool compressed) const;

  DlCanvas* GetCanvas();

  bool IsValid() const;

 private:
  sk_sp<SkSurface> offscreen_surface_;
  DlSkCanvasAdapter adapter_;

  FML_DISALLOW_COPY_AND_ASSIGN(OffscreenSurface);
};

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_FLOW_LAYERS_OFFSCREEN_SURFACE_H_
