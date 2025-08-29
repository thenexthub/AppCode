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

#include "appcode/shell/platform/linux/fl_renderable.h"

G_DEFINE_INTERFACE(FlRenderable, fl_renderable, G_TYPE_OBJECT)

static void fl_renderable_default_init(FlRenderableInterface* iface) {}

void fl_renderable_redraw(FlRenderable* self) {
  g_return_if_fail(FL_IS_RENDERABLE(self));

  FL_RENDERABLE_GET_IFACE(self)->redraw(self);
}

void fl_renderable_make_current(FlRenderable* self) {
  g_return_if_fail(FL_IS_RENDERABLE(self));

  FL_RENDERABLE_GET_IFACE(self)->make_current(self);
}
