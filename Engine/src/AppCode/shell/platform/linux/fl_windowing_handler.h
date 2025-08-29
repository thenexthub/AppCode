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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOWING_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOWING_HANDLER_H_

#include <gtk/gtk.h>

#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_view.h"

G_BEGIN_DECLS

G_DECLARE_DERIVABLE_TYPE(FlWindowingHandler,
                         fl_windowing_handler,
                         FL,
                         WINDOWING_HANDLER,
                         GObject);

struct _FlWindowingHandlerClass {
  GObjectClass parent_class;

  GtkWindow* (*create_window)(FlWindowingHandler* handler, FlView* view);
};

/**
 * FlWindowingHandler:
 *
 * #FlWindowingHandler is a handler that implements the shell side
 * of SystemChannels.windowing from the Flutter services library.
 */

/**
 * fl_windowing_handler_new:
 * @engine: an #FlEngine.
 *
 * Creates a new handler that implements SystemChannels.windowing from the
 * Flutter services library.
 *
 * Returns: a new #FlWindowingHandler
 */
FlWindowingHandler* fl_windowing_handler_new(FlEngine* engine);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_WINDOWING_HANDLER_H_
