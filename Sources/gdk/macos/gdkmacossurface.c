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

#include "config.h"

#include <AppKit/AppKit.h>
#include <float.h>
#include <gdk/gdk.h>

#import "GdkMacosView.h"

#include "gdkmacossurface-private.h"

#include "gdkdebugprivate.h"
#include "gdkdeviceprivate.h"
#include "gdkdisplay.h"
#include "gdkeventsprivate.h"
#include "gdkframeclockprivate.h"
#include "gdkseatprivate.h"
#include "gdksurfaceprivate.h"

#include "gdkmacosdevice.h"
#include "gdkmacosdevice-private.h"
#include "gdkmacosdisplay-private.h"
#include "gdkmacosdrag-private.h"
#include "gdkmacosdragsurface-private.h"
#include "gdkmacosglcontext-private.h"
#include "gdkmacosmonitor-private.h"
#include "gdkmacospopupsurface-private.h"
#include "gdkmacosutils-private.h"
#include "gdkmacostoplevelsurface-private.h"

G_DEFINE_ABSTRACT_TYPE (GdkMacosSurface, gdk_macos_surface, GDK_TYPE_SURFACE)

enum {
  PROP_0,
  PROP_NATIVE,
  LAST_PROP
};

static GParamSpec *properties [LAST_PROP];

void
_gdk_macos_surface_request_frame (GdkMacosSurface *this)
{
  g_assert (GDK_IS_MACOS_SURFACE (this));

  if (this->awaiting_frame)
    return;

  if (this->best_monitor != NULL)
    {
      this->awaiting_frame = TRUE;
      _gdk_macos_monitor_add_frame_callback (GDK_MACOS_MONITOR (this->best_monitor), this);
      gdk_surface_freeze_updates (GDK_SURFACE (this));
    }
}

static void
_gdk_macos_surface_cancel_frame (GdkMacosSurface *this)
{
  g_assert (GDK_IS_MACOS_SURFACE (this));

  if (!this->awaiting_frame)
    return;

  if (this->best_monitor != NULL)
    {
      this->awaiting_frame = FALSE;
      _gdk_macos_monitor_remove_frame_callback (GDK_MACOS_MONITOR (this->best_monitor), this);
      gdk_surface_thaw_updates (GDK_SURFACE (this));
    }
}

void
_gdk_macos_surface_frame_presented (GdkMacosSurface *this,
                                    gint64           presentation_time,
                                    gint64           refresh_interval)
{
  GdkFrameTimings *timings;
  GdkFrameClock *frame_clock;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  this->awaiting_frame = FALSE;

  if (GDK_SURFACE_DESTROYED (this))
    return;

  frame_clock = gdk_surface_get_frame_clock (GDK_SURFACE (this));

  if (this->pending_frame_counter)
    {
      timings = gdk_frame_clock_get_timings (frame_clock, this->pending_frame_counter);

      if (timings != NULL)
        {
          timings->presentation_time = presentation_time - refresh_interval;
          timings->complete = TRUE;
        }

      this->pending_frame_counter = 0;
    }

  timings = gdk_frame_clock_get_current_timings (frame_clock);

  if (timings != NULL)
    {
      timings->refresh_interval = refresh_interval;
      timings->predicted_presentation_time = presentation_time;
    }

  if (GDK_SURFACE_IS_MAPPED (GDK_SURFACE (this)))
    gdk_surface_thaw_updates (GDK_SURFACE (this));
}

void
_gdk_macos_surface_reposition_children (GdkMacosSurface *this)
{
  g_assert (GDK_IS_MACOS_SURFACE (this));


  if (GDK_SURFACE_DESTROYED (this))
    return;

  for (const GList *iter = GDK_SURFACE (this)->children;
       iter != NULL;
       iter = iter->next)
    {
      GdkMacosSurface *child = iter->data;

      g_assert (GDK_IS_MACOS_SURFACE (child));

      if (GDK_IS_MACOS_POPUP_SURFACE (child))
        _gdk_macos_popup_surface_reposition (GDK_MACOS_POPUP_SURFACE (child));
    }
}

static void
gdk_macos_surface_set_input_region (GdkSurface     *surface,
                                    cairo_region_t *region)
{
  GdkMacosSurface *this = (GdkMacosSurface *)surface;
  cairo_rectangle_int_t rect;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  if (this->window == NULL)
    return;

  cairo_region_get_extents (region, &rect);

  [(GdkMacosBaseView *)[this->window contentView] setInputArea:&rect];
}

static void
gdk_macos_surface_set_opaque_region (GdkSurface     *surface,
                                     cairo_region_t *region)
{
  GdkMacosSurface *this = (GdkMacosSurface *)surface;
  NSView *nsview;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  if ((nsview = _gdk_macos_surface_get_view (GDK_MACOS_SURFACE (surface))))
    [(GdkMacosView *)nsview setOpaqueRegion:region];
}

static void
gdk_macos_surface_hide (GdkSurface *surface)
{
  GdkMacosSurface *this = (GdkMacosSurface *)surface;
  GdkSeat *seat;
  gboolean was_key;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  this->show_on_next_swap = FALSE;

  _gdk_macos_surface_cancel_frame (this);

  was_key = [this->window isKeyWindow];

  seat = gdk_display_get_default_seat (surface->display);
  gdk_seat_ungrab (seat);

  [this->window hide];

  _gdk_surface_clear_update_area (surface);

  g_clear_object (&this->buffer);
  g_clear_object (&this->front);

  if (was_key)
    {
      GdkSurface *parent;

      if (GDK_IS_TOPLEVEL (surface))
        parent = surface->transient_for;
      else
        parent = surface->parent;

      /* Return key input to the parent window if necessary */
      if (parent != NULL && GDK_SURFACE_IS_MAPPED (parent))
        {
          GdkMacosWindow *parentWindow = GDK_MACOS_SURFACE (parent)->window;

          [parentWindow showAndMakeKey:YES];
        }
    }
}

static double
gdk_macos_surface_get_scale (GdkSurface *surface)
{
  GdkMacosSurface *this = (GdkMacosSurface *)surface;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  return [this->window backingScaleFactor];
}

static void
gdk_macos_surface_begin_frame (GdkMacosSurface *this)
{
  g_assert (GDK_IS_MACOS_SURFACE (this));

  this->in_frame = TRUE;
}

static void
gdk_macos_surface_end_frame (GdkMacosSurface *this)
{
  GdkFrameTimings *timings;
  GdkFrameClock *frame_clock;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  if (GDK_SURFACE_DESTROYED (this))
    return;

  frame_clock = gdk_surface_get_frame_clock (GDK_SURFACE (this));

  if ((timings = gdk_frame_clock_get_current_timings (frame_clock)))
    this->pending_frame_counter = timings->frame_counter;

  this->in_frame = FALSE;

  _gdk_macos_surface_request_frame (this);
}

static void
gdk_macos_surface_before_paint (GdkMacosSurface *this,
                                GdkFrameClock   *frame_clock)
{
  GdkSurface *surface = (GdkSurface *)this;

  g_assert (GDK_IS_MACOS_SURFACE (this));
  g_assert (GDK_IS_FRAME_CLOCK (frame_clock));

  if (GDK_SURFACE_DESTROYED (this))
    return;

  if (surface->update_freeze_count == 0)
    gdk_macos_surface_begin_frame (this);
}

static void
gdk_macos_surface_after_paint (GdkMacosSurface *this,
                               GdkFrameClock   *frame_clock)
{
  GdkSurface *surface = (GdkSurface *)this;

  g_assert (GDK_IS_MACOS_SURFACE (this));
  g_assert (GDK_IS_FRAME_CLOCK (frame_clock));

  if (GDK_SURFACE_DESTROYED (this))
    return;

  if (surface->update_freeze_count == 0)
    gdk_macos_surface_end_frame (this);
}

static void
gdk_macos_surface_get_root_coords (GdkSurface *surface,
                                   int         x,
                                   int         y,
                                   int        *root_x,
                                   int        *root_y)
{
  GdkMacosSurface *this = (GdkMacosSurface *)surface;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  if (root_x)
    *root_x = this->root_x + x;

  if (root_y)
    *root_y = this->root_y + y;
}

static gboolean
gdk_macos_surface_get_device_state (GdkSurface      *surface,
                                    GdkDevice       *device,
                                    double          *x,
                                    double          *y,
                                    GdkModifierType *mask)
{
  GdkDisplay *display;
  NSWindow *nswindow;
  NSPoint point;

  g_assert (GDK_IS_MACOS_SURFACE (surface));
  g_assert (GDK_IS_MACOS_DEVICE (device));
  g_assert (x != NULL);
  g_assert (y != NULL);
  g_assert (mask != NULL);

  if (GDK_SURFACE_DESTROYED (surface))
    return FALSE;

  display = gdk_surface_get_display (surface);
  nswindow = _gdk_macos_surface_get_native (GDK_MACOS_SURFACE (surface));
  point = [nswindow mouseLocationOutsideOfEventStream];

  *mask = _gdk_macos_display_get_current_keyboard_modifiers (GDK_MACOS_DISPLAY (display))
        | _gdk_macos_display_get_current_mouse_modifiers (GDK_MACOS_DISPLAY (display));

  *x = point.x;
  *y = surface->height - point.y;

  return *x >= 0 && *y >= 0 && *x < surface->width && *y < surface->height;
}

static void
gdk_macos_surface_get_geometry (GdkSurface *surface,
                                int        *x,
                                int        *y,
                                int        *width,
                                int        *height)
{
  g_assert (GDK_IS_MACOS_SURFACE (surface));

  if (x != NULL)
    *x = surface->x;

  if (y != NULL)
    *y = surface->y;

  if (width != NULL)
    *width = surface->width;

  if (height != NULL)
    *height = surface->height;
}

static GdkDrag *
gdk_macos_surface_drag_begin (GdkSurface         *surface,
                              GdkDevice          *device,
                              GdkContentProvider *content,
                              GdkDragAction       actions,
                              double              dx,
                              double              dy)
{
  GdkMacosSurface *this = (GdkMacosSurface *)surface;
  GdkMacosSurface *drag_surface;
  GdkMacosDrag *drag;
  GdkCursor *cursor;

  g_assert (GDK_IS_MACOS_SURFACE (this));
  g_assert (GDK_IS_MACOS_TOPLEVEL_SURFACE (this) ||
            GDK_IS_MACOS_POPUP_SURFACE (this));
  g_assert (GDK_IS_MACOS_DEVICE (device));
  g_assert (GDK_IS_CONTENT_PROVIDER (content));

  drag_surface = _gdk_macos_drag_surface_new (GDK_MACOS_DISPLAY (surface->display));
  drag = g_object_new (GDK_TYPE_MACOS_DRAG,
                       "drag-surface", drag_surface,
                       "surface", surface,
                       "device", device,
                       "content", content,
                       "actions", actions,
                       NULL);
  g_clear_object (&drag_surface);

  cursor = gdk_drag_get_cursor (GDK_DRAG (drag),
                                gdk_drag_get_selected_action (GDK_DRAG (drag)));
  gdk_macos_drag_set_cursor (drag, cursor);

  if (!_gdk_macos_drag_begin (drag, content, this->window))
    {
      g_object_unref (drag);
      return NULL;
    }

  /* Hold a reference until drop_done is called */
  g_object_ref (drag);

  return GDK_DRAG (g_steal_pointer (&drag));
}

static void
gdk_macos_surface_destroy (GdkSurface *surface,
                           gboolean    foreign_destroy)
{
  GDK_BEGIN_MACOS_ALLOC_POOL;

  GdkMacosSurface *this = (GdkMacosSurface *)surface;
  GdkMacosWindow *window = g_steal_pointer (&this->window);
  GdkFrameClock *frame_clock;

  _gdk_macos_surface_cancel_frame (this);
  g_clear_object (&this->best_monitor);

  if ((frame_clock = gdk_surface_get_frame_clock (GDK_SURFACE (this))))
    {
      g_signal_handlers_disconnect_by_func (frame_clock,
                                            G_CALLBACK (gdk_macos_surface_before_paint),
                                            this);
      g_signal_handlers_disconnect_by_func (frame_clock,
                                            G_CALLBACK (gdk_macos_surface_after_paint),
                                            this);
    }

  g_clear_pointer (&this->title, g_free);

  if (window != NULL)
    [window close];

  _gdk_macos_display_surface_removed (GDK_MACOS_DISPLAY (surface->display), this);

  g_clear_pointer (&this->monitors, g_ptr_array_unref);

  g_clear_object (&this->buffer);
  g_clear_object (&this->front);

  g_assert (this->sorted.prev == NULL);
  g_assert (this->sorted.next == NULL);
  g_assert (this->frame.prev == NULL);
  g_assert (this->frame.next == NULL);
  g_assert (this->main.prev == NULL);
  g_assert (this->main.next == NULL);

  GDK_END_MACOS_ALLOC_POOL;
}

static void
gdk_macos_surface_constructed (GObject *object)
{
  GdkMacosSurface *this = (GdkMacosSurface *)object;
  GdkFrameClock *frame_clock;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  G_OBJECT_CLASS (gdk_macos_surface_parent_class)->constructed (object);

  if ((frame_clock = gdk_surface_get_frame_clock (GDK_SURFACE (this))))
    {
      g_signal_connect_object (frame_clock,
                               "before-paint",
                               G_CALLBACK (gdk_macos_surface_before_paint),
                               this,
                               G_CONNECT_SWAPPED);
      g_signal_connect_object (frame_clock,
                               "after-paint",
                               G_CALLBACK (gdk_macos_surface_after_paint),
                               this,
                               G_CONNECT_SWAPPED);
    }

  gdk_surface_freeze_updates (GDK_SURFACE (this));
  _gdk_macos_surface_monitor_changed (this);

  if (this->window != NULL)
    _gdk_macos_surface_configure (this);

  _gdk_macos_display_surface_added (GDK_MACOS_DISPLAY (gdk_surface_get_display (GDK_SURFACE (this))),
                                    this);
}

static void
gdk_macos_surface_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GdkMacosSurface *this = GDK_MACOS_SURFACE (object);

  switch (prop_id)
    {
    case PROP_NATIVE:
      g_value_set_pointer (value, this->window);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gdk_macos_surface_class_init (GdkMacosSurfaceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GdkSurfaceClass *surface_class = GDK_SURFACE_CLASS (klass);

  object_class->constructed = gdk_macos_surface_constructed;
  object_class->get_property = gdk_macos_surface_get_property;

  surface_class->destroy = gdk_macos_surface_destroy;
  surface_class->drag_begin = gdk_macos_surface_drag_begin;
  surface_class->get_device_state = gdk_macos_surface_get_device_state;
  surface_class->get_geometry = gdk_macos_surface_get_geometry;
  surface_class->get_root_coords = gdk_macos_surface_get_root_coords;
  surface_class->get_scale = gdk_macos_surface_get_scale;
  surface_class->hide = gdk_macos_surface_hide;
  surface_class->set_input_region = gdk_macos_surface_set_input_region;
  surface_class->set_opaque_region = gdk_macos_surface_set_opaque_region;

  /**
   * GdkMacosSurface:native:
   *
   * The "native" property contains the underlying NSWindow.
   */
  properties [PROP_NATIVE] =
    g_param_spec_pointer ("native", NULL, NULL,
                          G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, LAST_PROP, properties);
}

static void
gdk_macos_surface_init (GdkMacosSurface *this)
{
  this->frame.data = this;
  this->main.data = this;
  this->sorted.data = this;
  this->monitors = g_ptr_array_new_with_free_func (g_object_unref);
}

const char *
_gdk_macos_surface_get_title (GdkMacosSurface *this)
{

  return this->title;
}

void
_gdk_macos_surface_set_title (GdkMacosSurface *this,
                              const char      *title)
{
  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  if (title == NULL)
    title = "";

  if (g_strcmp0 (this->title, title) != 0)
    {
      g_free (this->title);
      this->title = g_strdup (title);

      GDK_BEGIN_MACOS_ALLOC_POOL;
      [this->window setTitle:[NSString stringWithUTF8String:title]];
      GDK_END_MACOS_ALLOC_POOL;
    }
}

CGDirectDisplayID
_gdk_macos_surface_get_screen_id (GdkMacosSurface *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_SURFACE (this), (CGDirectDisplayID)-1);

  if (this->window != NULL)
    {
      NSScreen *screen = [this->window screen];
      return [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];
    }

  return (CGDirectDisplayID)-1;
}

NSWindow *
_gdk_macos_surface_get_native (GdkMacosSurface *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_SURFACE (this), NULL);

  return (NSWindow *)this->window;
}

void
_gdk_macos_surface_set_native (GdkMacosSurface *this,
                               GdkMacosWindow  *window)
{
  g_assert (this->window == NULL);

  this->window = window;
  [this->window setGdkSurface:this];
}

/**
 * gdk_macos_surface_get_native_window:
 * @this: a #GdkMacosSurface
 *
 * Gets the underlying NSWindow used by the surface.
 *
 * The NSWindow's contentView is an implementation detail and may change
 * between releases of GTK.
 *
 * Returns: (nullable): a #NSWindow or %NULL
 *
 * Since: 4.8
 */
gpointer
gdk_macos_surface_get_native_window (GdkMacosSurface *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_SURFACE (this), NULL);

  return _gdk_macos_surface_get_native (this);
}

void
_gdk_macos_surface_set_geometry_hints (GdkMacosSurface   *this,
                                       const GdkGeometry *geometry,
                                       GdkSurfaceHints    geom_mask)
{
  NSSize max_size;
  NSSize min_size;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));
  g_return_if_fail (geometry != NULL);
  g_return_if_fail (this->window != NULL);

  if (geom_mask & GDK_HINT_MAX_SIZE)
    max_size = NSMakeSize (geometry->max_width, geometry->max_height);
  else
    max_size = NSMakeSize (FLT_MAX, FLT_MAX);
  [this->window setContentMaxSize:max_size];

  if (geom_mask & GDK_HINT_MIN_SIZE)
    min_size = NSMakeSize (geometry->min_width, geometry->min_height);
  else
    min_size = NSMakeSize (1, 1);
  [this->window setContentMinSize:min_size];
}

void
_gdk_macos_surface_resize (GdkMacosSurface *this,
                           int              width,
                           int              height)
{
  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  _gdk_macos_surface_move_resize (this, -1, -1, width, height);
}

void
_gdk_macos_surface_update_fullscreen_state (GdkMacosSurface *this)
{
  GdkToplevelState state;
  gboolean is_fullscreen;
  gboolean was_fullscreen;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  state = GDK_SURFACE (this)->state;
  is_fullscreen = ([this->window styleMask] & NSWindowStyleMaskFullScreen) != 0;
  was_fullscreen = (state & GDK_TOPLEVEL_STATE_FULLSCREEN) != 0;

  if (is_fullscreen != was_fullscreen)
    {
      if (is_fullscreen)
        gdk_synthesize_surface_state (GDK_SURFACE (this), 0, GDK_TOPLEVEL_STATE_FULLSCREEN);
      else
        gdk_synthesize_surface_state (GDK_SURFACE (this), GDK_TOPLEVEL_STATE_FULLSCREEN, 0);
    }
}

void
_gdk_macos_surface_configure (GdkMacosSurface *this)
{
  GdkSurface *surface = (GdkSurface *)this;
  GdkMacosDisplay *display;
  GdkMacosSurface *parent;
  NSRect frame_rect;
  NSRect content_rect;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  if (GDK_SURFACE_DESTROYED (this))
    return;

  if (surface->parent != NULL)
    parent = GDK_MACOS_SURFACE (surface->parent);
  else if (surface->transient_for != NULL)
    parent = GDK_MACOS_SURFACE (surface->transient_for);
  else
    parent = NULL;

  display = GDK_MACOS_DISPLAY (GDK_SURFACE (this)->display);
  frame_rect = [this->window frame];
  content_rect = [this->window contentRectForFrameRect:frame_rect];

  _gdk_macos_display_from_display_coords (GDK_MACOS_DISPLAY (display),
                                          content_rect.origin.x,
                                          content_rect.origin.y + content_rect.size.height,
                                          &this->root_x, &this->root_y);

  if (parent != NULL)
    {
      surface->x = this->root_x - parent->root_x;
      surface->y = this->root_y - parent->root_y;
    }
  else
    {
      surface->x = this->root_x;
      surface->y = this->root_y;
    }

  if (surface->width != content_rect.size.width ||
      surface->height != content_rect.size.height)
    {
      surface->width = content_rect.size.width;
      surface->height = content_rect.size.height;

      g_clear_object (&this->buffer);
      g_clear_object (&this->front);

      _gdk_surface_update_size (surface);
      gdk_surface_invalidate_rect (surface, NULL);
    }

  _gdk_macos_surface_reposition_children (this);
}

void
_gdk_macos_surface_show (GdkMacosSurface *this)
{
  gboolean was_mapped;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  if (GDK_SURFACE_DESTROYED (this))
    return;

  _gdk_macos_display_clear_sorting (GDK_MACOS_DISPLAY (GDK_SURFACE (this)->display));
  this->show_on_next_swap = TRUE;

  was_mapped = GDK_SURFACE_IS_MAPPED (GDK_SURFACE (this));

  if (!was_mapped)
    {
      gdk_surface_set_is_mapped (GDK_SURFACE (this), TRUE);
      gdk_surface_request_layout (GDK_SURFACE (this));
      gdk_surface_invalidate_rect (GDK_SURFACE (this), NULL);
      gdk_surface_thaw_updates (GDK_SURFACE (this));
    }
}

void
_gdk_macos_surface_synthesize_null_key (GdkMacosSurface *this)
{
  GdkTranslatedKey translated = {0};
  GdkTranslatedKey no_lock = {0};
  GdkDisplay *display;
  GdkEvent *event;
  GdkSeat *seat;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  translated.keyval = GDK_KEY_VoidSymbol;
  no_lock.keyval = GDK_KEY_VoidSymbol;

  display = gdk_surface_get_display (GDK_SURFACE (this));
  seat = gdk_display_get_default_seat (display);
  event = gdk_key_event_new (GDK_KEY_PRESS,
                             GDK_SURFACE (this),
                             gdk_seat_get_keyboard (seat),
                             GDK_CURRENT_TIME,
                             0,
                             0,
                             FALSE,
                             &translated,
                             &no_lock,
                             NULL);
  _gdk_event_queue_append (display, event);
}

void
_gdk_macos_surface_move (GdkMacosSurface *this,
                         int              x,
                         int              y)
{
  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  _gdk_macos_surface_move_resize (this, x, y, -1, -1);
}

void
_gdk_macos_surface_move_resize (GdkMacosSurface *this,
                                int              x,
                                int              y,
                                int              width,
                                int              height)
{
  GdkSurface *surface = (GdkSurface *)this;
  GdkDisplay *display;
  NSRect content_rect;
  NSRect frame_rect;
  gboolean ignore_move;
  gboolean ignore_size;
  GdkRectangle current;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  /* Query for up-to-date values in case we're racing against
   * an incoming frame notify which could be queued behind whatever
   * we're processing right now.
   */
  frame_rect = [this->window frame];
  content_rect = [this->window contentRectForFrameRect:frame_rect];
  _gdk_macos_display_from_display_coords (GDK_MACOS_DISPLAY (GDK_SURFACE (this)->display),
                                          content_rect.origin.x, content_rect.origin.y,
                                          &current.x, &current.y);
  current.width = content_rect.size.width;
  current.height = content_rect.size.height;

  /* Check if we can ignore the operation all together */
  ignore_move = (x == -1 || (x == current.x)) &&
                (y == -1 || (y == current.y));
  ignore_size = (width == -1 || (width == current.width)) &&
                (height == -1 || (height == current.height));

  if (ignore_move && ignore_size)
    return;

  display = gdk_surface_get_display (surface);

  if (width == -1)
    width = current.width;

  if (height == -1)
    height = current.height;

  if (x == -1)
    x = current.x;

  if (y == -1)
    y = current.y;

  _gdk_macos_display_to_display_coords (GDK_MACOS_DISPLAY (display),
                                        x, y + height,
                                        &x, &y);

  if (!ignore_move)
    content_rect.origin = NSMakePoint (x, y);

  if (!ignore_size)
    content_rect.size = NSMakeSize (width, height);

  frame_rect = [this->window frameRectForContentRect:content_rect];
  [this->window setFrame:frame_rect display:NO];
}

void
_gdk_macos_surface_user_resize (GdkMacosSurface *this,
                                CGRect           new_frame)
{
  GdkMacosDisplay *display;
  CGRect content_rect;
  int root_x, root_y;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));
  g_return_if_fail (GDK_IS_TOPLEVEL (this));

  if (GDK_SURFACE_DESTROYED (this))
    return;

  display = GDK_MACOS_DISPLAY (GDK_SURFACE (this)->display);
  content_rect = [this->window contentRectForFrameRect:new_frame];

  _gdk_macos_display_from_display_coords (display,
                                          new_frame.origin.x,
                                          new_frame.origin.y + new_frame.size.height,
                                          &root_x, &root_y);

  this->next_layout.root_x = root_x;
  this->next_layout.root_y = root_y;
  this->next_layout.width = content_rect.size.width;
  this->next_layout.height = content_rect.size.height;

  gdk_surface_request_layout (GDK_SURFACE (this));
}

gboolean
_gdk_macos_surface_is_tracking (GdkMacosSurface *this,
                                NSTrackingArea  *area)
{
  GdkMacosBaseView *view;

  g_return_val_if_fail (GDK_IS_MACOS_SURFACE (this), FALSE);

  if (this->window == NULL)
    return FALSE;

  view = (GdkMacosBaseView *)[this->window contentView];
  if (view == NULL)
    return FALSE;

  return [view trackingArea] == area;
}

void
_gdk_macos_surface_monitor_changed (GdkMacosSurface *this)
{
  GListModel *monitors;
  GdkMonitor *best = NULL;
  GdkRectangle rect;
  GdkRectangle intersect;
  GdkDisplay *display;
  GdkMonitor *monitor;
  guint n_monitors;
  int best_area = 0;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  if (this->in_change_monitor)
    return;

  this->in_change_monitor = TRUE;

  _gdk_macos_surface_cancel_frame (this);
  _gdk_macos_surface_configure (this);

  rect.x = this->root_x;
  rect.y = this->root_y;
  rect.width = GDK_SURFACE (this)->width;
  rect.height = GDK_SURFACE (this)->height;

  for (guint i = this->monitors->len; i > 0; i--)
    {
      monitor = g_ptr_array_index (this->monitors, i-1);

      if (!gdk_rectangle_intersect (&monitor->geometry, &rect, &intersect))
        {
          g_object_ref (monitor);
          g_ptr_array_remove_index (this->monitors, i-1);
          gdk_surface_leave_monitor (GDK_SURFACE (this), monitor);
          g_object_unref (monitor);
        }
    }

  display = gdk_surface_get_display (GDK_SURFACE (this));
  monitors = gdk_display_get_monitors (display);
  n_monitors = g_list_model_get_n_items (monitors);

  for (guint i = 0; i < n_monitors; i++)
    {
      monitor = g_list_model_get_item (monitors, i);

      if (!g_ptr_array_find (this->monitors, monitor, NULL))
        {
          gdk_surface_enter_monitor (GDK_SURFACE (this), monitor);
          g_ptr_array_add (this->monitors, g_object_ref (monitor));
        }

      g_object_unref (monitor);
    }

  /* We need to create a new IOSurface for this monitor */
  g_clear_object (&this->buffer);
  g_clear_object (&this->front);

  /* Determine the best-fit monitor */
  for (guint i = 0; i < this->monitors->len; i++)
    {
      monitor = g_ptr_array_index (this->monitors, i);

      if (gdk_rectangle_intersect (&monitor->geometry, &rect, &intersect))
        {
          int area = intersect.width * intersect.height;

          if (area > best_area)
            {
              best_area = area;
              best = monitor;
            }
        }
    }

  if (g_set_object (&this->best_monitor, best))
    {
      GDK_DEBUG (MISC, "Surface \"%s\" moved to monitor \"%s\"",
                       this->title ? this->title : "unknown",
                       gdk_monitor_get_connector (best));

      _gdk_macos_surface_configure (this);

      if (GDK_SURFACE_IS_MAPPED (GDK_SURFACE (this)))
        {
          _gdk_macos_surface_request_frame (this);
          gdk_surface_request_layout (GDK_SURFACE (this));
        }

      for (const GList *iter = GDK_SURFACE (this)->children;
           iter != NULL;
           iter = iter->next)
        {
          GdkMacosSurface *child = iter->data;
          GdkRectangle area;

          g_set_object (&child->best_monitor, best);

          area.x = this->root_x + GDK_SURFACE (child)->x;
          area.y = this->root_y + GDK_SURFACE (child)->y;
          area.width = GDK_SURFACE (child)->width;
          area.height = GDK_SURFACE (child)->height;

          _gdk_macos_monitor_clamp (GDK_MACOS_MONITOR (best), &area);
          _gdk_macos_surface_move (child, area.x, area.y);
          gdk_surface_invalidate_rect (GDK_SURFACE (child), NULL);
        }
    }

  gdk_surface_invalidate_rect (GDK_SURFACE (this), NULL);

  this->in_change_monitor = FALSE;
}

GdkMonitor *
_gdk_macos_surface_get_best_monitor (GdkMacosSurface *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_SURFACE (this), NULL);

  return this->best_monitor;
}

NSView *
_gdk_macos_surface_get_view (GdkMacosSurface *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_SURFACE (this), NULL);

  if (this->window == NULL)
    return NULL;

  return [this->window contentView];
}

void
_gdk_macos_surface_set_opacity (GdkMacosSurface *this,
                                double           opacity)
{
  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  if (this->window != NULL)
    [this->window setAlphaValue:opacity];
}

void
_gdk_macos_surface_get_root_coords (GdkMacosSurface *this,
                                    int             *x,
                                    int             *y)
{
  GdkSurface *surface;
  int out_x = 0;
  int out_y = 0;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));

  for (surface = GDK_SURFACE (this); surface; surface = surface->parent)
    {
      out_x += surface->x;
      out_y += surface->y;
    }

  if (x)
    *x = out_x;

  if (y)
    *y = out_y;
}

GdkMacosBuffer *
_gdk_macos_surface_get_buffer (GdkMacosSurface *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_SURFACE (this), NULL);

  if (GDK_SURFACE_DESTROYED (this))
    return NULL;

  if (this->buffer == NULL)
    {
      /* Create replacement buffer. We always use 4-byte and 32-bit BGRA for
       * our surface as that can work with both Cairo and GL. The GdkMacosTile
       * handles opaque regions for the compositor, so using 3-byte/24-bit is
       * not a necessary optimization.
       */
      double scale = gdk_surface_get_scale_factor (GDK_SURFACE (this));
      guint width = GDK_SURFACE (this)->width * scale;
      guint height = GDK_SURFACE (this)->height * scale;

      this->buffer = _gdk_macos_buffer_new (width, height, scale, 4, 32);
    }

  return this->buffer;
}

static void
_gdk_macos_surface_do_delayed_show (GdkMacosSurface *this)
{
  GdkSurface *surface = (GdkSurface *)this;

  g_assert (GDK_IS_MACOS_SURFACE (this));

  this->show_on_next_swap = FALSE;
  [this->window showAndMakeKey:YES];

  _gdk_macos_display_clear_sorting (GDK_MACOS_DISPLAY (surface->display));
  gdk_surface_request_motion (surface);
}

void
_gdk_macos_surface_swap_buffers (GdkMacosSurface      *this,
                                 const cairo_region_t *damage)
{
  GdkMacosBuffer *swap;

  g_return_if_fail (GDK_IS_MACOS_SURFACE (this));
  g_return_if_fail (damage != NULL);

  swap = this->buffer;
  this->buffer = this->front;
  this->front = swap;

  /* This code looks like it swaps buffers, but since the IOSurfaceRef
   * appears to be retained on the other side, we really just ask all
   * of the GdkMacosTile CALayer's to update their contents.
   */
  [this->window swapBuffer:swap withDamage:damage];

  /* We might have delayed actually showing the window until the buffer
   * contents are ready to be displayed. Doing so ensures that we don't
   * get a point where we might have invalid buffer contents before we
   * have content to display to the user.
   */
  if G_UNLIKELY (this->show_on_next_swap)
    _gdk_macos_surface_do_delayed_show (this);
}
