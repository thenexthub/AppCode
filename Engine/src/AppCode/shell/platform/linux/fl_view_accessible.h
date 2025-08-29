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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_VIEW_ACCESSIBLE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_VIEW_ACCESSIBLE_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <atk/atk.h>

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"

G_BEGIN_DECLS

// ATK g_autoptr macros weren't added until 2.37. Add them manually.
// https://gitlab.gnome.org/GNOME/atk/-/issues/10
#if !ATK_CHECK_VERSION(2, 37, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(AtkPlug, g_object_unref)
#endif

G_DECLARE_FINAL_TYPE(FlViewAccessible,
                     fl_view_accessible,
                     FL,
                     VIEW_ACCESSIBLE,
                     AtkPlug)

/**
 * FlViewAccessible:
 *
 * #FlViewAccessible is an object that exposes accessibility information for an
 * #FlView.
 */

/**
 * fl_view_accessible_new:
 * @engine: the #FlEngine.
 * @view_id: the Flutter view id.
 *
 * Creates a new accessibility object that exposes Flutter accessibility
 * information to ATK.
 *
 * Returns: a new #FlViewAccessible.
 */
FlViewAccessible* fl_view_accessible_new(FlEngine* engine,
                                         FlutterViewId view_id);

/**
 * fl_view_accessible_handle_update_semantics:
 * @accessible: an #FlViewAccessible.
 * @update: semantic update information.
 *
 * Handle a semantics update from Flutter.
 */
void fl_view_accessible_handle_update_semantics(
    FlViewAccessible* accessible,
    const FlutterSemanticsUpdate2* update);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_VIEW_ACCESSIBLE_H_
