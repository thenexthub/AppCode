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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_REGISTRAR_PRIVATE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_REGISTRAR_PRIVATE_H_

#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_engine.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture_registrar.h"

G_BEGIN_DECLS

/**
 * fl_texture_registrar_new:
 * @engine: an #FlEngine.
 *
 * Creates a new #FlTextureRegistrar.
 *
 * Returns: a new #FlTextureRegistrar.
 */
FlTextureRegistrar* fl_texture_registrar_new(FlEngine* engine);

/**
 * fl_texture_registrar_lookup_texture:
 * @registrar: an #FlTextureRegistrar.
 * @texture_id: ID of texture.
 *
 * Looks for the texture with the given ID.
 *
 * Returns: an #FlTexture or %NULL if no texture with this ID.
 */
FlTexture* fl_texture_registrar_lookup_texture(FlTextureRegistrar* registrar,
                                               int64_t texture_id);

/**
 * fl_texture_registrar_shutdown:
 * @registrar: an #FlTextureRegistrar.
 *
 * Shutdown the registrary and unregister any textures.
 */
void fl_texture_registrar_shutdown(FlTextureRegistrar* registrar);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_FL_TEXTURE_REGISTRAR_PRIVATE_H_
