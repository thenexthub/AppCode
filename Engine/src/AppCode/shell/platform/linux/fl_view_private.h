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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_VIEW_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_VIEW_PRIVATE_H_

#include "appcode/shell/platform/linux/fl_view_accessible.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_view.h"

G_BEGIN_DECLS

/**
 * fl_view_get_accessible:
 * @view: an #FlView.
 *
 * Get the accessible object for this view.
 *
 * Returns: an #FlViewAccessible.
 */
FlViewAccessible* fl_view_get_accessible(FlView* view);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_VIEW_PRIVATE_H_
