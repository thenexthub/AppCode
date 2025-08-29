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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_RENDERABLE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_RENDERABLE_H_

#include <gdk/gdk.h>

#include "appcode/shell/platform/embedder/embedder.h"

G_BEGIN_DECLS

G_DECLARE_INTERFACE(FlRenderable, fl_renderable, FL, RENDERABLE, GObject);

/**
 * FlRenderable:
 *
 * An interface for a class that can render views from #FlRenderer.
 *
 * This interface is typically implemented by #FlView and is provided to make
 * #FlRenderer easier to test.
 */

struct _FlRenderableInterface {
  GTypeInterface g_iface;

  void (*redraw)(FlRenderable* renderable);
  void (*make_current)(FlRenderable* renderable);
};

/**
 * fl_renderable_redraw:
 * @renderable: an #FlRenderable
 *
 * Indicate the renderable needs to redraw. When ready, the renderable should
 * call fl_compositor_draw().
 */
void fl_renderable_redraw(FlRenderable* renderable);

/**
 * fl_renderable_make_current:
 * @renderable: an #FlRenderable
 *
 * Make this renderable the current OpenGL context.
 */
void fl_renderable_make_current(FlRenderable* renderable);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_RENDERABLE_H_
