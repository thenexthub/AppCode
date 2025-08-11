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

#define VK_USE_PLATFORM_METAL_EXT

#include <AppKit/AppKit.h>

#import "GdkMacosWindow.h"

#include "gdkdisplayprivate.h"
#include "gdkeventsprivate.h"

#include "gdkdisplaylinksource.h"
#include "gdkmacosclipboard-private.h"
#include "gdkmacoscairocontext-private.h"
#include "gdkmacoseventsource-private.h"
#include "gdkmacosdisplay-private.h"
#include "gdkmacosdrag-private.h"
#include "gdkmacosdrop-private.h"
#include "gdkmacosglcontext-private.h"
#include "gdkmacoskeymap-private.h"
#include "gdkmacosmonitor-private.h"
#include "gdkmacospopupsurface-private.h"
#include "gdkmacosseat-private.h"
#include "gdkmacossurface-private.h"
#include "gdkmacostoplevelsurface-private.h"
#include "gdkmacosutils-private.h"
#include "gdkmacosvulkancontext-private.h"

G_DEFINE_TYPE (GdkMacosDisplay, gdk_macos_display, GDK_TYPE_DISPLAY)

#define EVENT_MAP_MAX_SIZE 10

typedef struct
{
  GList     link;
  GdkEvent *gdk_event;
  NSEvent  *nsevent;
} GdkToNSEventMap;

static GSource *event_source;
static GQueue event_map = G_QUEUE_INIT;

static GdkMacosMonitor *
get_monitor (GdkMacosDisplay *this,
             guint            position)
{
  GdkMacosMonitor *monitor;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  /* Get the monitor but return a borrowed reference */
  monitor = g_list_model_get_item (G_LIST_MODEL (this->monitors), position);
  if (monitor != NULL)
    g_object_unref (monitor);

  return monitor;
}

static gboolean
gdk_macos_display_get_setting (GdkDisplay  *display,
                               const char *setting,
                               GValue      *value)
{
  if (gdk_display_get_debug_flags (display) & GDK_DEBUG_DEFAULT_SETTINGS)
    return FALSE;

  return _gdk_macos_display_get_setting (GDK_MACOS_DISPLAY (display), setting, value);
}

static GListModel *
gdk_macos_display_get_monitors (GdkDisplay *display)
{
  return G_LIST_MODEL (GDK_MACOS_DISPLAY (display)->monitors);
}

static GdkMonitor *
gdk_macos_display_get_monitor_at_surface (GdkDisplay *display,
                                          GdkSurface *surface)
{
  GdkMacosDisplay *this = (GdkMacosDisplay *)display;
  CGDirectDisplayID screen_id;
  guint n_monitors;

  g_assert (GDK_IS_MACOS_DISPLAY (this));
  g_assert (GDK_IS_MACOS_SURFACE (surface));

  screen_id = _gdk_macos_surface_get_screen_id (GDK_MACOS_SURFACE (surface));
  n_monitors = g_list_model_get_n_items (G_LIST_MODEL (this->monitors));

  for (guint i = 0; i < n_monitors; i++)
    {
      GdkMacosMonitor *monitor = get_monitor (this, i);

      if (screen_id == _gdk_macos_monitor_get_screen_id (monitor))
        return GDK_MONITOR (monitor);
    }

  return GDK_MONITOR (get_monitor (this, 0));
}

static GdkMacosMonitor *
gdk_macos_display_find_monitor (GdkMacosDisplay   *this,
                                CGDirectDisplayID  screen_id)
{
  guint n_monitors;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  n_monitors = g_list_model_get_n_items (G_LIST_MODEL (this->monitors));

  for (guint i = 0; i < n_monitors; i++)
    {
      GdkMacosMonitor *monitor = get_monitor (this, i);

      if (screen_id == _gdk_macos_monitor_get_screen_id (monitor))
        return monitor;
    }

  return NULL;
}

static void
gdk_macos_display_update_bounds (GdkMacosDisplay *this)
{
  GDK_BEGIN_MACOS_ALLOC_POOL;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  this->min_x = G_MAXINT;
  this->min_y = G_MAXINT;

  this->max_x = G_MININT;
  this->max_y = G_MININT;

  for (id obj in [NSScreen screens])
    {
      NSRect geom = [(NSScreen *)obj frame];

      this->min_x = MIN (this->min_x, geom.origin.x);
      this->min_y = MIN (this->min_y, geom.origin.y);
      this->max_x = MAX (this->max_x, geom.origin.x + geom.size.width);
      this->max_y = MAX (this->max_y, geom.origin.y + geom.size.height);
    }

  this->width = this->max_x - this->min_x;
  this->height = this->max_y - this->min_y;

  GDK_DEBUG (MISC, "Displays reconfigured to bounds %d,%d %dx%d",
                   this->min_x, this->min_y, this->width, this->height);

  GDK_END_MACOS_ALLOC_POOL;
}

void
_gdk_macos_display_reload_monitors (GdkMacosDisplay *this)
{
  GDK_BEGIN_MACOS_ALLOC_POOL;

  GArray *seen;
  guint n_monitors;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  gdk_macos_display_update_bounds (this);

  seen = g_array_new (FALSE, FALSE, sizeof (CGDirectDisplayID));

  for (id obj in [NSScreen screens])
    {
      CGDirectDisplayID screen_id;
      GdkMacosMonitor *monitor;

      screen_id = [[[obj deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue];
      g_array_append_val (seen, screen_id);

      if ((monitor = gdk_macos_display_find_monitor (this, screen_id)))
        {
          _gdk_macos_monitor_reconfigure (monitor);
        }
      else
        {
          monitor = _gdk_macos_monitor_new (this, screen_id);
          g_list_store_append (this->monitors, monitor);
          g_object_unref (monitor);
        }
    }

  n_monitors = g_list_model_get_n_items (G_LIST_MODEL (this->monitors));

  for (guint i = n_monitors; i > 0; i--)
    {
      GdkMacosMonitor *monitor = get_monitor (this, i - 1);
      CGDirectDisplayID screen_id = _gdk_macos_monitor_get_screen_id (monitor);
      gboolean found = FALSE;

      for (guint j = 0; j < seen->len; j++)
        {
          if (screen_id == g_array_index (seen, CGDirectDisplayID, j))
            {
              found = TRUE;
              break;
            }
        }

      if (!found)
        g_list_store_remove (this->monitors, i - 1);
    }

  g_array_unref (seen);

  GDK_END_MACOS_ALLOC_POOL;
}

static void
gdk_macos_display_load_seat (GdkMacosDisplay *this)
{
  GdkSeat *seat;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  seat = _gdk_macos_seat_new (this);
  gdk_display_add_seat (GDK_DISPLAY (this), seat);
  g_object_unref (seat);
}

static const char *
gdk_macos_display_get_name (GdkDisplay *display)
{
  return GDK_MACOS_DISPLAY (display)->name;
}

static void
gdk_macos_display_beep (GdkDisplay *display)
{
  NSBeep ();
}

static void
gdk_macos_display_flush (GdkDisplay *display)
{
  /* Not Supported */
}

static void
gdk_macos_display_sync (GdkDisplay *display)
{
  /* Not Supported */
}

static gulong
gdk_macos_display_get_next_serial (GdkDisplay *display)
{
  static gulong serial = 0;
  return ++serial;
}

static void
gdk_macos_display_notify_startup_complete (GdkDisplay  *display,
                                           const char *startup_notification_id)
{
  /* Not Supported */
}

static void
push_nsevent (GdkEvent *gdk_event,
              NSEvent  *nsevent)
{
  GdkToNSEventMap *map = g_new0 (GdkToNSEventMap, 1);

  map->link.data = map;
  map->gdk_event = gdk_event_ref (gdk_event);
  map->nsevent = g_steal_pointer (&nsevent);

  g_queue_push_tail_link (&event_map, &map->link);

  if (event_map.length > EVENT_MAP_MAX_SIZE)
    {
      map = g_queue_pop_head_link (&event_map)->data;

      gdk_event_unref (map->gdk_event);
      [map->nsevent release];
      g_free (map);
    }
}

static void
gdk_macos_display_queue_events (GdkDisplay *display)
{
  GdkMacosDisplay *this = (GdkMacosDisplay *)display;
  NSEvent *nsevent;

  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));

  while ((nsevent = _gdk_macos_event_source_get_pending ()))
    {
      GdkEvent *event = _gdk_macos_display_translate (this, nsevent);

      if (event == GDK_MACOS_EVENT_DROP)
        {
          [nsevent release];
        }
      else if (event != NULL)
        {
          push_nsevent (event, nsevent);
          _gdk_windowing_got_event (GDK_DISPLAY (this),
                                    _gdk_event_queue_append (GDK_DISPLAY (this), event),
                                    event,
                                    _gdk_display_get_next_serial (GDK_DISPLAY (this)));
        }
      else
        {
          [NSApp sendEvent:nsevent];
          [nsevent release];
        }
    }
}

void
_gdk_macos_display_surface_added (GdkMacosDisplay *this,
                                  GdkMacosSurface *surface)
{
  g_assert (GDK_IS_MACOS_DISPLAY (this));
  g_assert (GDK_IS_MACOS_SURFACE (surface));
  g_assert (!queue_contains (&this->sorted_surfaces, &surface->sorted));
  g_assert (!queue_contains (&this->main_surfaces, &surface->main));
  g_assert (surface->sorted.data == surface);
  g_assert (surface->main.data == surface);
  g_assert (surface->frame.data == surface);

  if (GDK_IS_TOPLEVEL (surface))
    g_queue_push_tail_link (&this->main_surfaces, &surface->main);

  _gdk_macos_display_clear_sorting (this);
}

void
_gdk_macos_display_surface_removed (GdkMacosDisplay *this,
                                    GdkMacosSurface *surface)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));
  g_return_if_fail (GDK_IS_MACOS_SURFACE (surface));

  if (this->keyboard_surface == surface)
    _gdk_macos_display_surface_resigned_key (this, surface);

  if (queue_contains (&this->sorted_surfaces, &surface->sorted))
    g_queue_unlink (&this->sorted_surfaces, &surface->sorted);

  if (queue_contains (&this->main_surfaces, &surface->main))
    _gdk_macos_display_surface_resigned_main (this, surface);

  g_return_if_fail (this->keyboard_surface != surface);
}

void
_gdk_macos_display_surface_became_key (GdkMacosDisplay *this,
                                       GdkMacosSurface *surface)
{
  GdkDevice *keyboard;
  GdkEvent *event;
  GdkSeat *seat;

  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));
  g_return_if_fail (GDK_IS_MACOS_SURFACE (surface));
  g_return_if_fail (this->keyboard_surface == NULL);

  this->keyboard_surface = surface;

  seat = gdk_display_get_default_seat (GDK_DISPLAY (this));
  keyboard = gdk_seat_get_keyboard (seat);
  event = gdk_focus_event_new (GDK_SURFACE (surface), keyboard, TRUE);
  _gdk_event_queue_append (GDK_DISPLAY (this), event);

  /* For each parent surface, we want them to look like they
   * are also still focused, so ensure they have that same
   * state associated with them.
   */
  if (GDK_IS_POPUP (surface))
    {
      for (GdkSurface *parent = GDK_SURFACE (surface)->parent;
           parent != NULL;
           parent = parent->parent)
        {
          if (GDK_IS_TOPLEVEL (parent))
            gdk_synthesize_surface_state (parent, 0, GDK_TOPLEVEL_STATE_FOCUSED);
        }
    }

  /* We just became the active window.  Unlike X11, Mac OS X does
   * not send us motion events while the window does not have focus
   * ("is not key").  We send a dummy motion notify event now, so that
   * everything in the window is set to correct state.
   */
  gdk_surface_request_motion (GDK_SURFACE (surface));
}

static gboolean
select_key_in_idle_cb (gpointer data)
{
  GdkMacosDisplay *this = data;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  this->select_key_in_idle = 0;

  /* Don't steal focus from NSPanel, etc */
  if (this->key_window_is_foreign)
    return G_SOURCE_REMOVE;

  if (this->keyboard_surface == NULL)
    {
      const GList *surfaces = _gdk_macos_display_get_surfaces (this);

      for (const GList *iter = surfaces; iter; iter = iter->next)
        {
          GdkMacosSurface *surface = iter->data;

          if (GDK_SURFACE_IS_MAPPED (GDK_SURFACE (surface)) &&
              ([surface->window styleMask] & NSWindowStyleMaskMiniaturizable) == 0)
            {
              [surface->window showAndMakeKey:YES];
              break;
            }
        }
    }

  return G_SOURCE_REMOVE;
}

void
_gdk_macos_display_surface_resigned_key (GdkMacosDisplay *this,
                                         GdkMacosSurface *surface)
{
  gboolean was_keyboard_surface;

  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));
  g_return_if_fail (GDK_IS_MACOS_SURFACE (surface));

  was_keyboard_surface = this->keyboard_surface == surface;

  this->keyboard_surface = NULL;

  if (was_keyboard_surface)
    {
      GdkDevice *keyboard;
      GdkEvent *event;
      GdkSeat *seat;
      GList *node;

      seat = gdk_display_get_default_seat (GDK_DISPLAY (this));
      keyboard = gdk_seat_get_keyboard (seat);
      event = gdk_focus_event_new (GDK_SURFACE (surface), keyboard, FALSE);
      node = _gdk_event_queue_append (GDK_DISPLAY (this), event);
      _gdk_windowing_got_event (GDK_DISPLAY (this), node, event,
                                _gdk_display_get_next_serial (GDK_DISPLAY (this)));
    }

  /* For each parent surface, we want them to look like they
   * are also still focused, so ensure they have that same
   * state associated with them.
   */
  if (GDK_IS_POPUP (surface))
    {
      for (GdkSurface *parent = GDK_SURFACE (surface)->parent;
           parent != NULL;
           parent = parent->parent)
        {
          if (GDK_IS_TOPLEVEL (parent))
            gdk_synthesize_surface_state (parent, GDK_TOPLEVEL_STATE_FOCUSED, 0);
        }
    }

  _gdk_macos_display_clear_sorting (this);

  if (this->select_key_in_idle == 0)
    this->select_key_in_idle = g_idle_add (select_key_in_idle_cb, this);
}

/* Raises a transient window.
 */
static void
raise_transient (GdkMacosSurface *surface)
{
  GdkMacosSurface *parent_surface = GDK_MACOS_SURFACE (GDK_SURFACE (surface)->transient_for);

  NSWindow *parent = _gdk_macos_surface_get_native (parent_surface);
  NSWindow *window = _gdk_macos_surface_get_native (surface);

  [parent removeChildWindow:window];
  [parent addChildWindow:window ordered:NSWindowAbove];
}

void
_gdk_macos_display_surface_became_main (GdkMacosDisplay *this,
                                        GdkMacosSurface *surface)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));
  g_return_if_fail (GDK_IS_MACOS_SURFACE (surface));

  if (queue_contains (&this->main_surfaces, &surface->main))
    g_queue_unlink (&this->main_surfaces, &surface->main);

  g_queue_push_head_link (&this->main_surfaces, &surface->main);

  if (GDK_SURFACE (surface)->transient_for)
    raise_transient (surface);

  _gdk_macos_display_clear_sorting (this);
}

void
_gdk_macos_display_surface_resigned_main (GdkMacosDisplay *this,
                                          GdkMacosSurface *surface)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));
  g_return_if_fail (GDK_IS_MACOS_SURFACE (surface));

  if (queue_contains (&this->main_surfaces, &surface->main))
    g_queue_unlink (&this->main_surfaces, &surface->main);

  _gdk_macos_display_clear_sorting (this);
}

static GdkKeymap *
gdk_macos_display_get_keymap (GdkDisplay *display)
{
  GdkMacosDisplay *this = (GdkMacosDisplay *)display;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  return GDK_KEYMAP (this->keymap);
}

static void
gdk_macos_display_load_clipboard (GdkMacosDisplay *this)
{
  g_assert (GDK_IS_MACOS_DISPLAY (this));

  GDK_DISPLAY (this)->clipboard = _gdk_macos_clipboard_new (this);
}

static GdkGLContext *
gdk_macos_display_init_gl (GdkDisplay  *display,
                           GError     **error)
{
  if (!gdk_gl_backend_can_be_used (GDK_GL_CGL, error))
    return FALSE;

  return g_object_new (GDK_TYPE_MACOS_GL_CONTEXT,
                       "display", display,
                       NULL);
}

static void
gdk_macos_display_finalize (GObject *object)
{
  GdkMacosDisplay *this = (GdkMacosDisplay *)object;

  _gdk_macos_display_feedback_destroy (this);

  g_clear_handle_id (&this->select_key_in_idle, g_source_remove);
  g_clear_pointer (&this->active_drags, g_hash_table_unref);
  g_clear_pointer (&this->active_drops, g_hash_table_unref);
  g_clear_object (&GDK_DISPLAY (this)->clipboard);
  g_clear_object (&this->monitors);
  g_clear_pointer (&this->name, g_free);

  G_OBJECT_CLASS (gdk_macos_display_parent_class)->finalize (object);
}

static void
gdk_macos_display_class_init (GdkMacosDisplayClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GdkDisplayClass *display_class = GDK_DISPLAY_CLASS (klass);

  object_class->finalize = gdk_macos_display_finalize;

  display_class->toplevel_type = GDK_TYPE_MACOS_TOPLEVEL_SURFACE;
  display_class->popup_type = GDK_TYPE_MACOS_POPUP_SURFACE;
  display_class->cairo_context_type = GDK_TYPE_MACOS_CAIRO_CONTEXT;

#ifdef GDK_RENDERING_VULKAN
  display_class->vk_context_type = GDK_TYPE_MACOS_VULKAN_CONTEXT;
  display_class->vk_extension_name = VK_EXT_METAL_SURFACE_EXTENSION_NAME;
#endif

  display_class->beep = gdk_macos_display_beep;
  display_class->flush = gdk_macos_display_flush;
  display_class->get_keymap = gdk_macos_display_get_keymap;
  display_class->get_monitors = gdk_macos_display_get_monitors;
  display_class->get_monitor_at_surface = gdk_macos_display_get_monitor_at_surface;
  display_class->get_next_serial = gdk_macos_display_get_next_serial;
  display_class->get_name = gdk_macos_display_get_name;
  display_class->get_setting = gdk_macos_display_get_setting;
  display_class->init_gl = gdk_macos_display_init_gl;
  display_class->notify_startup_complete = gdk_macos_display_notify_startup_complete;
  display_class->queue_events = gdk_macos_display_queue_events;
  display_class->sync = gdk_macos_display_sync;
}

static void
gdk_macos_display_init (GdkMacosDisplay *this)
{
  this->monitors = g_list_store_new (GDK_TYPE_MONITOR);
  this->active_drags = g_hash_table_new_full (NULL, NULL, NULL, g_object_unref);
  this->active_drops = g_hash_table_new_full (NULL, NULL, NULL, g_object_unref);

  gdk_display_set_composited (GDK_DISPLAY (this), TRUE);
  gdk_display_set_input_shapes (GDK_DISPLAY (this), FALSE);
  gdk_display_set_rgba (GDK_DISPLAY (this), TRUE);
  gdk_display_set_shadow_width (GDK_DISPLAY (this), FALSE);
}

GdkDisplay *
_gdk_macos_display_open (const char *display_name)
{
  static GdkMacosDisplay *this;
  ProcessSerialNumber psn = { 0, kCurrentProcess };

  /* Until we can have multiple GdkMacosEventSource instances
   * running concurrently, we can't exactly support multiple
   * display connections. So just short-circuit if we already
   * have one active.
   */
  if (this != NULL)
    return NULL;

  display_name = display_name ? display_name : "";
  GDK_DEBUG (MISC, "opening display %s", display_name);

  /* Make the current process a foreground application, i.e. an app
   * with a user interface, in case we're not running from a .app bundle
   */
  TransformProcessType (&psn, kProcessTransformToForegroundApplication);

  [NSApplication sharedApplication];

  this = g_object_new (GDK_TYPE_MACOS_DISPLAY, NULL);
  this->name = g_strdup (display_name);
  this->keymap = _gdk_macos_keymap_new (this);

  gdk_macos_display_load_seat (this);
  gdk_macos_display_load_clipboard (this);
  _gdk_macos_display_reload_monitors (this);

  /* Initialize feedback from display server */
  _gdk_macos_display_feedback_init (this);

  if (event_source == NULL)
    {
      event_source = _gdk_macos_event_source_new (this);
      g_source_attach (event_source, NULL);
    }

  g_object_add_weak_pointer (G_OBJECT (this), (gpointer *)&this);

  gdk_display_emit_opened (GDK_DISPLAY (this));

  [NSApp activateIgnoringOtherApps:YES];

  return GDK_DISPLAY (this);
}

void
_gdk_macos_display_to_display_coords (GdkMacosDisplay *this,
                                      int              x,
                                      int              y,
                                      int             *out_x,
                                      int             *out_y)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));

  if (out_y)
    *out_y = this->height - y + this->min_y;

  if (out_x)
    *out_x = x + this->min_x;
}

void
_gdk_macos_display_from_display_coords (GdkMacosDisplay *this,
                                        int              x,
                                        int              y,
                                        int             *out_x,
                                        int             *out_y)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));

  if (out_y != NULL)
    *out_y = this->height - y + this->min_y;

  if (out_x != NULL)
    *out_x = x - this->min_x;
}

GdkMonitor *
_gdk_macos_display_get_monitor_at_coords (GdkMacosDisplay *this,
                                          int              x,
                                          int              y)
{
  GdkMacosMonitor *best_match = NULL;
  guint n_monitors;

  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);

  n_monitors = g_list_model_get_n_items (G_LIST_MODEL (this->monitors));

  for (guint i = 0; i < n_monitors; i++)
    {
      GdkMacosMonitor *monitor = get_monitor (this, i);
      const GdkRectangle *geom = &GDK_MONITOR (monitor)->geometry;

      if (x >= geom->x &&
          y >= geom->y &&
          x <= (geom->x + geom->width) &&
          y <= (geom->y + geom->height))
        {
          if (x <= geom->x + geom->width && y < geom->y + geom->height)
            return GDK_MONITOR (monitor);

          /* Not an exact match as we're on a boundary, but there is
           * a good chance another monitor doesn't exist there so we
           * would want to still treat this as the best monitor.
           */
          best_match = monitor;
        }
    }

  return GDK_MONITOR (best_match);
}

GdkMonitor *
_gdk_macos_display_get_monitor_at_display_coords (GdkMacosDisplay *this,
                                                  int              x,
                                                  int              y)
{
  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);

  _gdk_macos_display_from_display_coords (this, x, y, &x, &y);

  return _gdk_macos_display_get_monitor_at_coords (this, x, y);
}

NSScreen *
_gdk_macos_display_get_screen_at_display_coords (GdkMacosDisplay *this,
                                                 int              x,
                                                 int              y)
{
  GDK_BEGIN_MACOS_ALLOC_POOL;

  NSArray *screens;
  NSScreen *screen = NULL;

  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);

  screens = [NSScreen screens];

  for (id obj in screens)
    {
      NSRect geom = [obj frame];

      if (x >= geom.origin.x && x <= geom.origin.x + geom.size.width &&
          y >= geom.origin.y && y <= geom.origin.y + geom.size.height)
        {
          screen = obj;
          break;
        }
    }

  GDK_END_MACOS_ALLOC_POOL;

  return screen;
}

void
_gdk_macos_display_break_all_grabs (GdkMacosDisplay *this,
                                    guint32          time)
{
  GdkDevice *devices[2];
  GdkSeat *seat;

  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));

  seat = gdk_display_get_default_seat (GDK_DISPLAY (this));
  devices[0] = gdk_seat_get_keyboard (seat);
  devices[1] = gdk_seat_get_pointer (seat);

  for (guint i = 0; i < G_N_ELEMENTS (devices); i++)
    {
      GdkDevice *device = devices[i];
      GdkDeviceGrabInfo *grab;

      grab = _gdk_display_get_last_device_grab (GDK_DISPLAY (this), device);

      if (grab != NULL)
        {
          GdkEvent *event;
          GList *node;

          event = gdk_grab_broken_event_new (grab->surface,
                                             device,
                                             grab->surface,
                                             TRUE);
          node = _gdk_event_queue_append (GDK_DISPLAY (this), event);
          _gdk_windowing_got_event (GDK_DISPLAY (this), node, event,
                                    _gdk_display_get_next_serial (GDK_DISPLAY (this)));
        }
    }
}

void
_gdk_macos_display_queue_events (GdkMacosDisplay *this)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));

  gdk_macos_display_queue_events (GDK_DISPLAY (this));
}

static GdkMacosSurface *
_gdk_macos_display_get_surface_at_coords (GdkMacosDisplay *this,
                                          int              x,
                                          int              y,
                                          int             *surface_x,
                                          int             *surface_y)
{
  const GList *surfaces;

  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);
  g_return_val_if_fail (surface_x != NULL, NULL);
  g_return_val_if_fail (surface_y != NULL, NULL);

  surfaces = _gdk_macos_display_get_surfaces (this);

  for (const GList *iter = surfaces; iter; iter = iter->next)
    {
      GdkSurface *surface = iter->data;

      g_assert (GDK_IS_MACOS_SURFACE (surface));

      if (!gdk_surface_get_mapped (surface))
        continue;

      if (x >= GDK_MACOS_SURFACE (surface)->root_x &&
          y >= GDK_MACOS_SURFACE (surface)->root_y &&
          x <= (GDK_MACOS_SURFACE (surface)->root_x + surface->width) &&
          y <= (GDK_MACOS_SURFACE (surface)->root_y + surface->height))
        {
          *surface_x = x - GDK_MACOS_SURFACE (surface)->root_x;
          *surface_y = y - GDK_MACOS_SURFACE (surface)->root_y;

          /* One last check to make sure that the x,y is within the input
           * region of the window. Otherwise we might send the event to the
           * wrong window because of window shadow.
           */
          if (surface->input_region != NULL &&
              !cairo_region_contains_point (surface->input_region, *surface_x, *surface_y))
            continue;

          return GDK_MACOS_SURFACE (surface);
        }
    }

  *surface_x = 0;
  *surface_y = 0;

  return NULL;
}

GdkMacosSurface *
_gdk_macos_display_get_surface_at_display_coords (GdkMacosDisplay *this,
                                                  double           x,
                                                  double           y,
                                                  int             *surface_x,
                                                  int             *surface_y)
{
  int x_gdk;
  int y_gdk;

  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);
  g_return_val_if_fail (surface_x != NULL, NULL);
  g_return_val_if_fail (surface_y != NULL, NULL);

  _gdk_macos_display_from_display_coords (this, x, y, &x_gdk, &y_gdk);

  return _gdk_macos_display_get_surface_at_coords (this, x_gdk, y_gdk, surface_x, surface_y);
}

NSWindow *
_gdk_macos_display_find_native_under_pointer (GdkMacosDisplay *this,
                                              int             *x,
                                              int             *y)
{
  GdkMacosSurface *surface;
  NSPoint point;

  g_assert (GDK_IS_MACOS_DISPLAY (this));

  point = [NSEvent mouseLocation];

  surface = _gdk_macos_display_get_surface_at_display_coords (this, point.x, point.y, x, y);
  if (surface != NULL)
    return _gdk_macos_surface_get_native (surface);

  return NULL;
}

void
_gdk_macos_display_clear_sorting (GdkMacosDisplay *this)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));

  while (this->sorted_surfaces.head != NULL)
    g_queue_unlink (&this->sorted_surfaces, this->sorted_surfaces.head);
}

const GList *
_gdk_macos_display_get_surfaces (GdkMacosDisplay *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);

  if (this->sorted_surfaces.length == 0)
    {
      GDK_BEGIN_MACOS_ALLOC_POOL;

      NSArray *array = [NSApp orderedWindows];
      GQueue sorted = G_QUEUE_INIT;

      this->key_window_is_foreign = FALSE;

      for (id obj in array)
        {
          NSWindow *nswindow = (NSWindow *)obj;
          GdkMacosSurface *surface;

          if ([nswindow isKeyWindow])
            this->key_window_is_foreign = !GDK_IS_MACOS_WINDOW (nswindow);

          if (!GDK_IS_MACOS_WINDOW (nswindow))
            continue;

          surface = [(GdkMacosWindow *)nswindow gdkSurface];

          surface->sorted.prev = NULL;
          surface->sorted.next = NULL;

          g_queue_push_tail_link (&sorted, &surface->sorted);
        }

      this->sorted_surfaces = sorted;

      /* We don't get notification of clipboard changes from the system so we
       * instead update it every time the foreground changes (and thusly
       * rebuild the sorted list).  Things could change other ways, such as
       * with scripts, but that is currently out of scope for us.
       */
      _gdk_macos_clipboard_check_externally_modified (
        GDK_MACOS_CLIPBOARD (GDK_DISPLAY (this)->clipboard));

      GDK_END_MACOS_ALLOC_POOL;
    }

  return this->sorted_surfaces.head;
}

void
_gdk_macos_display_warp_pointer (GdkMacosDisplay *this,
                                 int              x,
                                 int              y)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));

  _gdk_macos_display_to_display_coords (this, x, y, &x, &y);

  CGWarpMouseCursorPosition ((CGPoint) { x, y });
}

/* Find the matching `NSEvent` for an `GdkEvent`. This function
 * return the `NSEvent`, also for rewritten `GdkEvent`'s.
 */
NSEvent *
_gdk_macos_display_get_matching_nsevent (GdkEvent *event)
{
  for (GList *iter = event_map.head; iter; iter = iter->next)
    {
      GdkToNSEventMap *map = iter->data;

      if (map->gdk_event->event_type == event->event_type &&
          map->gdk_event->device == event->device &&
          map->gdk_event->time == event->time)
        {
          return map->nsevent;
        }
    }

  return NULL;
}

/* Find the matching `NSEvent` for the original `GdkEvent`.
 * If an event was rewritten, it returns `NULL`.
 */
NSEvent *
_gdk_macos_display_get_exact_nsevent (GdkEvent *event)
{
  for (GList *iter = event_map.head; iter; iter = iter->next)
    {
      GdkToNSEventMap *map = iter->data;

      if (map->gdk_event == event)
        {
          return map->nsevent;
        }
    }

  return NULL;
}

NSEvent *
_gdk_macos_display_get_last_nsevent (void)
{
  const GdkToNSEventMap *map = g_queue_peek_tail (&event_map);
  if (map)
    return map->nsevent;

  return NULL;
}

GdkDrag *
_gdk_macos_display_find_drag (GdkMacosDisplay *this,
                              NSInteger        sequence_number)
{
  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);

  return g_hash_table_lookup (this->active_drags, GSIZE_TO_POINTER (sequence_number));
}

void
_gdk_macos_display_set_drag (GdkMacosDisplay *this,
                             NSInteger        sequence_number,
                             GdkDrag         *drag)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));
  g_return_if_fail (!drag || GDK_IS_MACOS_DRAG (drag));

  if (drag)
    g_hash_table_insert (this->active_drags,
                         GSIZE_TO_POINTER (sequence_number),
                         g_object_ref (drag));
  else
    g_hash_table_remove (this->active_drags,
                         GSIZE_TO_POINTER (sequence_number));
}

GdkDrop *
_gdk_macos_display_find_drop (GdkMacosDisplay *this,
                              NSInteger        sequence_number)
{
  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (this), NULL);

  return g_hash_table_lookup (this->active_drops, GSIZE_TO_POINTER (sequence_number));
}

void
_gdk_macos_display_set_drop (GdkMacosDisplay *this,
                             NSInteger        sequence_number,
                             GdkDrop         *drop)
{
  g_return_if_fail (GDK_IS_MACOS_DISPLAY (this));
  g_return_if_fail (!drop || GDK_IS_MACOS_DROP (drop));

  if (drop)
    g_hash_table_insert (this->active_drops,
                         GSIZE_TO_POINTER (sequence_number),
                         g_object_ref (drop));
  else
    g_hash_table_remove (this->active_drops,
                         GSIZE_TO_POINTER (sequence_number));
}
