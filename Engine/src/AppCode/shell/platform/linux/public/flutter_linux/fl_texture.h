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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_TEXTURE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_TEXTURE_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <glib-object.h>
#include <gmodule.h>
#include <stdint.h>

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_INTERFACE(FlTexture, fl_texture, FL, TEXTURE, GObject)

/**
 * FlTexture:
 *
 * #FlTexture represents a texture.
 *
 * You can derive #FlTextureGL for populating hardware-accelerated textures or
 * instantiate #FlPixelBufferTexture for populating pixel buffers. Do NOT
 * directly implement this interface.
 */

struct _FlTextureInterface {
  GTypeInterface g_iface;

  void (*set_id)(FlTexture* texture, int64_t id);

  int64_t (*get_id)(FlTexture* texture);
};

/**
 * fl_texture_get_id:
 * @texture: a #FlTexture.
 *
 * Get the ID for this texture, which can be passed to Flutter code to refer to
 * this texture.
 *
 * Returns: a texture ID.
 */
int64_t fl_texture_get_id(FlTexture* texture);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_TEXTURE_H_
