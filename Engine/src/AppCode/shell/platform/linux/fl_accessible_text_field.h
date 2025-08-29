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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_ACCESSIBLE_TEXT_FIELD_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_ACCESSIBLE_TEXT_FIELD_H_

#include <gtk/gtk.h>

#include "appcode/shell/platform/linux/fl_accessible_node.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlAccessibleTextField,
                     fl_accessible_text_field,
                     FL,
                     ACCESSIBLE_TEXT_FIELD,
                     FlAccessibleNode);

/**
 * fl_accessible_text_field_new:
 * @engine: the #FlEngine this node came from.
 * @view_id: the ID of the view that contains this semantics node.
 * @id: the semantics node ID this object represents.
 *
 * Creates a new accessibility object that exposes an editable Flutter text
 * field to ATK.
 *
 * Returns: a new #FlAccessibleNode.
 */
FlAccessibleNode* fl_accessible_text_field_new(FlEngine* engine,
                                               FlutterViewId view_id,
                                               int32_t id);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_ACCESSIBLE_TEXT_FIELD_H_
