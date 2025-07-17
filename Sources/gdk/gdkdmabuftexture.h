/* gdkdmabuftexture.h
 *
 * Copyright 2023 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#if !defined (__GDK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gdk/gdk.h> can be included directly."
#endif

#include <gdk/gdktypes.h>
#include <gdk/gdktexture.h>

G_BEGIN_DECLS

#define GDK_TYPE_DMABUF_TEXTURE (gdk_dmabuf_texture_get_type ())

#define GDK_DMABUF_TEXTURE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDK_TYPE_DMABUF_TEXTURE, GdkDmabufTexture))
#define GDK_IS_DMABUF_TEXTURE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDK_TYPE_DMABUF_TEXTURE))

#define GDK_DMABUF_ERROR       (gdk_dmabuf_error_quark ())

typedef struct _GdkDmabufTextureClass       GdkDmabufTextureClass;

GDK_AVAILABLE_IN_4_14
GType                   gdk_dmabuf_texture_get_type                (void) G_GNUC_CONST;

GDK_AVAILABLE_IN_4_14
GQuark                  gdk_dmabuf_error_quark                     (void) G_GNUC_CONST;

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GdkDmabufTexture, g_object_unref)

G_END_DECLS
