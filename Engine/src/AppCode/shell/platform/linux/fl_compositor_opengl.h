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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_COMPOSITOR_OPENGL_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_COMPOSITOR_OPENGL_H_

#include <gtk/gtk.h>

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/fl_compositor.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlCompositorOpenGL,
                     fl_compositor_opengl,
                     FL,
                     COMPOSITOR_OPENGL,
                     FlCompositor)

/**
 * FlCompositorOpenGL:
 *
 * #FlCompositorOpenGL is an abstract class that allows Flutter to draw pixels.
 */

/**
 * fl_compositor_opengl_new:
 * @engine: an #FlEngine.
 *
 * Creates a new OpenGL compositor.
 *
 * Returns: a new #FlCompositorOpenGL.
 */
FlCompositorOpenGL* fl_compositor_opengl_new(FlEngine* engine);

/**
 * fl_compositor_opengl_setup:
 * @compositor: an #FlCompositorOpenGL.
 *
 * Creates OpenGL resources required before rendering. Requires an active
 * context.
 */
void fl_compositor_opengl_setup(FlCompositorOpenGL* compositor);

/**
 * fl_compositor_opengl_render:
 * @compositor: an #FlCompositorOpenGL.
 * @view_id: view to render.
 * @width: width of the window in pixels.
 * @height: height of the window in pixels.
 * @background_color: color to use for background.
 *
 * Performs OpenGL commands to render current Flutter view.
 */
void fl_compositor_opengl_render(FlCompositorOpenGL* compositor,
                                 FlutterViewId view_id,
                                 int width,
                                 int height,
                                 const GdkRGBA* background_color);

/**
 * fl_compositor_opengl_cleanup:
 * @compositor: an #FlCompositorOpenGL.
 *
 * Removes OpenGL resources used for rendering. Requires an active
 * context.
 */
void fl_compositor_opengl_cleanup(FlCompositorOpenGL* compositor);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_COMPOSITOR_OPENGL_H_
