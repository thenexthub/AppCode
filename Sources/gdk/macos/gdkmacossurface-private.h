/*
 * Copyright © 2020 Red Hat, Inc.
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
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <AppKit/AppKit.h>
#include <cairo.h>

#include "gdksurfaceprivate.h"

#include "gdkmacosbuffer-private.h"
#include "gdkmacosdisplay.h"
#include "gdkmacossurface.h"

#import "GdkMacosWindow.h"

G_BEGIN_DECLS

#define GDK_MACOS_SURFACE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GDK_TYPE_MACOS_SURFACE, GdkMacosSurfaceClass))
#define GDK_IS_MACOS_SURFACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GDK_TYPE_MACOS_SURFACE))
#define GDK_MACOS_SURFACE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GDK_TYPE_MACOS_SURFACE, GdkMacosSurfaceClass))

struct _GdkMacosSurface
{
  GdkSurface parent_instance;

  GList main;
  GList sorted;
  GList frame;

  GdkMacosWindow *window;
  GdkMacosBuffer *buffer;
  GdkMacosBuffer *front;
  GPtrArray *monitors;
  GdkMonitor *best_monitor;
  char *title;

  int root_x;
  int root_y;

  struct {
    int root_x;
    int root_y;
    int width;
    int height;
  } next_layout;

  cairo_rectangle_int_t next_frame;

  gint64 pending_frame_counter;

  guint did_initial_present : 1;
  guint geometry_dirty : 1;
  guint next_frame_set : 1;
  guint show_on_next_swap : 1;
  guint in_change_monitor : 1;
  guint in_frame : 1;
  guint awaiting_frame : 1;
};

struct _GdkMacosSurfaceClass
{
  GdkSurfaceClass parent_class;
};

NSWindow          *_gdk_macos_surface_get_native              (GdkMacosSurface      *this);
void               _gdk_macos_surface_set_native              (GdkMacosSurface      *this,
                                                               GdkMacosWindow       *window);
CGDirectDisplayID  _gdk_macos_surface_get_screen_id           (GdkMacosSurface      *this);
const char        *_gdk_macos_surface_get_title               (GdkMacosSurface      *this);
void               _gdk_macos_surface_set_title               (GdkMacosSurface      *this,
                                                               const char           *title);
NSView            *_gdk_macos_surface_get_view                (GdkMacosSurface      *this);
gboolean           _gdk_macos_surface_get_modal_hint          (GdkMacosSurface      *this);
void               _gdk_macos_surface_set_modal_hint          (GdkMacosSurface      *this,
                                                               gboolean              modal_hint);
void               _gdk_macos_surface_set_geometry_hints      (GdkMacosSurface      *this,
                                                               const GdkGeometry    *geometry,
                                                               GdkSurfaceHints       geom_mask);
void               _gdk_macos_surface_resize                  (GdkMacosSurface      *this,
                                                               int                   width,
                                                               int                   height);
void               _gdk_macos_surface_update_fullscreen_state (GdkMacosSurface      *this);
void               _gdk_macos_surface_request_frame           (GdkMacosSurface      *this);
void               _gdk_macos_surface_frame_presented         (GdkMacosSurface      *this,
                                                               gint64                predicted_presentation_time,
                                                               gint64                refresh_interval);
void               _gdk_macos_surface_show                    (GdkMacosSurface      *this);
void               _gdk_macos_surface_synthesize_null_key     (GdkMacosSurface      *this);
void               _gdk_macos_surface_move                    (GdkMacosSurface      *this,
                                                               int                   x,
                                                               int                   y);
void               _gdk_macos_surface_move_resize             (GdkMacosSurface      *this,
                                                               int                   x,
                                                               int                   y,
                                                               int                   width,
                                                               int                   height);
void               _gdk_macos_surface_configure               (GdkMacosSurface    *this);
void               _gdk_macos_surface_user_resize             (GdkMacosSurface    *this,
                                                               CGRect              new_frame);
gboolean           _gdk_macos_surface_is_tracking             (GdkMacosSurface      *this,
                                                               NSTrackingArea       *area);
void               _gdk_macos_surface_monitor_changed         (GdkMacosSurface      *this);
GdkMonitor        *_gdk_macos_surface_get_best_monitor        (GdkMacosSurface      *this);
void               _gdk_macos_surface_reposition_children     (GdkMacosSurface      *this);
void               _gdk_macos_surface_set_opacity             (GdkMacosSurface      *this,
                                                               double                opacity);
void               _gdk_macos_surface_get_root_coords         (GdkMacosSurface      *this,
                                                               int                  *x,
                                                               int                  *y);
GdkMacosBuffer    *_gdk_macos_surface_get_buffer              (GdkMacosSurface      *this);
void               _gdk_macos_surface_swap_buffers            (GdkMacosSurface      *this,
                                                               const cairo_region_t *damage);

G_END_DECLS

