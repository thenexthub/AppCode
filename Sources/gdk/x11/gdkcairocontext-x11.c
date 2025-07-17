/* GDK - The GIMP Drawing Kit
 *
 * gdkcairocontext-x11.c: X11 specific Cairo wrappers
 *
 * Copyright © 2016  Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gdkcairocontext-x11.h"

#include "gdkprivate-x11.h"

#include "gdkcairoprivate.h"
#include "gdksurfaceprivate.h"

#include <X11/Xlib.h>

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

G_DEFINE_TYPE (GdkX11CairoContext, gdk_x11_cairo_context, GDK_TYPE_CAIRO_CONTEXT)

static cairo_surface_t *
create_cairo_surface_for_surface (GdkSurface *surface)
{
  GdkDisplay *display;
  cairo_surface_t *cairo_surface;
  Visual *visual;
  int scale;
    
  display = gdk_surface_get_display (surface);
  scale = gdk_surface_get_scale_factor (surface);

  visual = gdk_x11_display_get_window_visual (GDK_X11_DISPLAY (display));
  cairo_surface = cairo_xlib_surface_create (gdk_x11_display_get_xdisplay (display),
                                             GDK_SURFACE_XID (surface),
                                             visual,
                                             gdk_surface_get_width (surface) * scale,
                                             gdk_surface_get_height (surface) * scale);

  return cairo_surface;
}

static void
gdk_x11_cairo_context_begin_frame (GdkDrawContext  *draw_context,
                                   gpointer         context_data,
                                   GdkMemoryDepth   depth,
                                   cairo_region_t  *region,
                                   GdkColorState  **out_color_state,
                                   GdkMemoryDepth  *out_depth)
{
  GdkX11CairoContext *self = GDK_X11_CAIRO_CONTEXT (draw_context);
  GdkRectangle clip_box;
  GdkSurface *surface;
  cairo_format_t format;

  surface = gdk_draw_context_get_surface (draw_context);
  cairo_region_get_extents (region, &clip_box);

  self->window_surface = create_cairo_surface_for_surface (surface);

  format = gdk_cairo_format_for_content (cairo_surface_get_content (self->window_surface)),
  self->paint_surface = cairo_image_surface_create (format,
                                                    MAX (clip_box.width, 1),
                                                    MAX (clip_box.height, 1));

  cairo_surface_set_device_scale (self->paint_surface, 1.0, 1.0);
  cairo_surface_set_device_offset (self->paint_surface, -clip_box.x, -clip_box.y);

  *out_color_state = GDK_COLOR_STATE_SRGB;
  *out_depth = gdk_color_state_get_depth (GDK_COLOR_STATE_SRGB);
}

static void
gdk_x11_cairo_context_end_frame (GdkDrawContext *draw_context,
                                 gpointer        context_data,
                                 cairo_region_t *painted)
{
  GdkX11CairoContext *self = GDK_X11_CAIRO_CONTEXT (draw_context);
  cairo_t *cr;

  cr = cairo_create (self->window_surface);

  cairo_set_source_surface (cr, self->paint_surface, 0, 0);
  gdk_cairo_region (cr, painted);
  cairo_clip (cr);

  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);

  cairo_destroy (cr);

  cairo_surface_flush (self->window_surface);

  g_clear_pointer (&self->paint_surface, cairo_surface_destroy);
  g_clear_pointer (&self->window_surface, cairo_surface_destroy);
}

static cairo_t *
gdk_x11_cairo_context_cairo_create (GdkCairoContext *context)
{
  GdkX11CairoContext *self = GDK_X11_CAIRO_CONTEXT (context);

  return cairo_create (self->paint_surface);
}

static void
gdk_x11_cairo_context_class_init (GdkX11CairoContextClass *klass)
{
  GdkDrawContextClass *draw_context_class = GDK_DRAW_CONTEXT_CLASS (klass);
  GdkCairoContextClass *cairo_context_class = GDK_CAIRO_CONTEXT_CLASS (klass);

  draw_context_class->begin_frame = gdk_x11_cairo_context_begin_frame;
  draw_context_class->end_frame = gdk_x11_cairo_context_end_frame;

  cairo_context_class->cairo_create = gdk_x11_cairo_context_cairo_create;
}

static void
gdk_x11_cairo_context_init (GdkX11CairoContext *self)
{
}

