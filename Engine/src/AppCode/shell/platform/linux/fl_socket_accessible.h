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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_SOCKET_ACCESSIBLE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_SOCKET_ACCESSIBLE_H_

#include <gtk/gtk-a11y.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlSocketAccessible,
                     fl_socket_accessible,
                     FL,
                     SOCKET_ACCESSIBLE,
                     GtkContainerAccessible);

void fl_socket_accessible_embed(FlSocketAccessible* self, gchar* id);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_SOCKET_ACCESSIBLE_H_
