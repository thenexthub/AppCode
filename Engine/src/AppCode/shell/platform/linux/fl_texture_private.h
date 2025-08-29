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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_PRIVATE_H_

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture.h"

G_BEGIN_DECLS

/**
 * fl_texture_set_id:
 * @texture: an #FlTexture.
 * @id: a texture ID.
 *
 * Set the ID for a texture.
 */
void fl_texture_set_id(FlTexture* texture, int64_t id);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_PRIVATE_H_
