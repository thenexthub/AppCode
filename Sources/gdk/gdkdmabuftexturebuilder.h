/*
 * Copyright © 2023 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Matthias Clasen <mclasen@redhat.com>
 */

#pragma once

#if !defined (__GDK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gdk/gdk.h> can be included directly."
#endif

#include <gdk/gdktypes.h>

G_BEGIN_DECLS

#define GDK_TYPE_DMABUF_TEXTURE_BUILDER (gdk_dmabuf_texture_builder_get_type ())
GDK_AVAILABLE_IN_4_14
GDK_DECLARE_INTERNAL_TYPE (GdkDmabufTextureBuilder, gdk_dmabuf_texture_builder, GDK, DMABUF_TEXTURE_BUILDER, GObject)

GDK_AVAILABLE_IN_4_14
GdkDmabufTextureBuilder *gdk_dmabuf_texture_builder_new              (void);

GDK_AVAILABLE_IN_4_14
GdkDisplay *             gdk_dmabuf_texture_builder_get_display      (GdkDmabufTextureBuilder    *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_display      (GdkDmabufTextureBuilder    *this,
                                                                      GdkDisplay                 *display);

GDK_AVAILABLE_IN_4_14
unsigned int             gdk_dmabuf_texture_builder_get_width        (GdkDmabufTextureBuilder    *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_width        (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                width);

GDK_AVAILABLE_IN_4_14
unsigned int             gdk_dmabuf_texture_builder_get_height       (GdkDmabufTextureBuilder    *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_height       (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                height);

GDK_AVAILABLE_IN_4_14
guint32                  gdk_dmabuf_texture_builder_get_fourcc       (GdkDmabufTextureBuilder    *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_fourcc       (GdkDmabufTextureBuilder    *this,
                                                                      guint32                     fourcc);

GDK_AVAILABLE_IN_4_14
guint64                  gdk_dmabuf_texture_builder_get_modifier     (GdkDmabufTextureBuilder    *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_modifier     (GdkDmabufTextureBuilder    *this,
                                                                      guint64                     modifier);

GDK_AVAILABLE_IN_4_14
gboolean                 gdk_dmabuf_texture_builder_get_premultiplied (GdkDmabufTextureBuilder    *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_premultiplied (GdkDmabufTextureBuilder    *this,
                                                                       gboolean                    premultiplied);

GDK_AVAILABLE_IN_4_14
unsigned int             gdk_dmabuf_texture_builder_get_n_planes     (GdkDmabufTextureBuilder    *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_n_planes     (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                n_planes);

GDK_AVAILABLE_IN_4_14
int                      gdk_dmabuf_texture_builder_get_fd           (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                plane) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_fd           (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                plane,
                                                                      int                         fd);

GDK_AVAILABLE_IN_4_14
unsigned int             gdk_dmabuf_texture_builder_get_stride       (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                plane) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_stride       (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                plane,
                                                                      unsigned int                stride);

GDK_AVAILABLE_IN_4_14
unsigned int             gdk_dmabuf_texture_builder_get_offset       (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                plane) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_offset       (GdkDmabufTextureBuilder    *this,
                                                                      unsigned int                plane,
                                                                      unsigned int                offset);

GDK_AVAILABLE_IN_4_16
GdkColorState *          gdk_dmabuf_texture_builder_get_color_state    (GdkDmabufTextureBuilder  *this);
GDK_AVAILABLE_IN_4_16
void                     gdk_dmabuf_texture_builder_set_color_state    (GdkDmabufTextureBuilder  *this,
                                                                        GdkColorState            *color_state);

GDK_AVAILABLE_IN_4_14
GdkTexture *             gdk_dmabuf_texture_builder_get_update_texture (GdkDmabufTextureBuilder  *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_update_texture (GdkDmabufTextureBuilder  *this,
                                                                        GdkTexture               *texture);

GDK_AVAILABLE_IN_4_14
cairo_region_t *         gdk_dmabuf_texture_builder_get_update_region (GdkDmabufTextureBuilder   *this) G_GNUC_PURE;
GDK_AVAILABLE_IN_4_14
void                     gdk_dmabuf_texture_builder_set_update_region (GdkDmabufTextureBuilder   *this,
                                                                       cairo_region_t            *region);

GDK_AVAILABLE_IN_4_14
GdkTexture *             gdk_dmabuf_texture_builder_build            (GdkDmabufTextureBuilder    *this,
                                                                      GDestroyNotify              destroy,
                                                                      gpointer                    data,
                                                                      GError                    **error);

G_END_DECLS

