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

#include <gdk/gdk.h>
#include <math.h>

#include "gdkdisplaylinksource.h"
#include "gdkmacosdisplay-private.h"
#include "gdkmacosmonitor-private.h"
#include "gdkmacossurface-private.h"
#include "gdkmacosutils-private.h"

struct _GdkMacosMonitor
{
  GdkMonitor            parent_instance;
  CGDirectDisplayID     screen_id;
  GdkDisplayLinkSource *display_link;
  NSRect                workarea;
  GQueue                awaiting_frames;
  guint                 has_opengl : 1;
  guint                 in_frame : 1;
};

struct _GdkMacosMonitorClass
{
  GdkMonitorClass parent_class;
};

G_DEFINE_TYPE (GdkMacosMonitor, gdk_macos_monitor, GDK_TYPE_MONITOR)

/**
 * gdk_macos_monitor_get_workarea:
 * @monitor: a `GdkMonitor`
 * @geometry: (out): a `GdkRectangle` to be filled with the monitor geometry
 *
 * Retrieves the size and position of the “work area” on a monitor
 * within the display coordinate space.
 *
 * The returned geometry is in ”application pixels”, not in ”device pixels”
 * (see [method@Gdk.Monitor.get_scale_factor]).
 */
void
gdk_macos_monitor_get_workarea (GdkMonitor   *monitor,
                                GdkRectangle *geometry)
{
  GdkMacosMonitor *this = (GdkMacosMonitor *)monitor;
  int x,  y;

  g_return_if_fail (GDK_IS_MACOS_MONITOR (this));
  g_return_if_fail (geometry != NULL);

  x = this->workarea.origin.x;
  y = this->workarea.origin.y + this->workarea.size.height;

  _gdk_macos_display_from_display_coords (GDK_MACOS_DISPLAY (monitor->display),
                                          x, y,
                                          &x, &y);

  geometry->x = x;
  geometry->y = y;
  geometry->width = this->workarea.size.width;
  geometry->height = this->workarea.size.height;
}

static void
gdk_macos_monitor_dispose (GObject *object)
{
  GdkMacosMonitor *this = (GdkMacosMonitor *)object;

  if (this->display_link)
    {
      g_source_destroy ((GSource *)this->display_link);
      g_clear_pointer ((GSource **)&this->display_link, g_source_unref);
    }

  G_OBJECT_CLASS (gdk_macos_monitor_parent_class)->dispose (object);
}

static void
gdk_macos_monitor_class_init (GdkMacosMonitorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gdk_macos_monitor_dispose;
}

static void
gdk_macos_monitor_init (GdkMacosMonitor *this)
{
}

static GdkSubpixelLayout
GetSubpixelLayout (CGDirectDisplayID screen_id)
{
#if 0
  GDK_BEGIN_MACOS_ALLOC_POOL;

  GdkSubpixelLayout subpixel_layout = GDK_SUBPIXEL_LAYOUT_UNKNOWN;
  io_service_t iosvc;
  NSDictionary *dict;
  guint layout;
  gboolean rotation;

  rotation = CGDisplayRotation (screen_id);
  iosvc = CGDisplayIOServicePort (screen_id);
  dict = (NSDictionary *)IODisplayCreateInfoDictionary (iosvc, kIODisplayOnlyPreferredName);
  layout = [[dict objectForKey:@kDisplaySubPixelLayout] unsignedIntValue];

  switch (layout)
    {
    case kDisplaySubPixelLayoutRGB:
      if (rotation == 0.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_HORIZONTAL_RGB;
      else if (rotation == 90.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_VERTICAL_RGB;
      else if (rotation == 180.0 || rotation == -180.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_HORIZONTAL_BGR;
      else if (rotation == -90.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_VERTICAL_BGR;
      break;

    case kDisplaySubPixelLayoutBGR:
      if (rotation == 0.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_HORIZONTAL_BGR;
      else if (rotation == 90.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_VERTICAL_BGR;
      else if (rotation == 180.0 || rotation == -180.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_HORIZONTAL_RGB;
      else if (rotation == -90.0 || rotation == 270.0)
        subpixel_layout = GDK_SUBPIXEL_LAYOUT_VERTICAL_RGB;
      break;

    default:
      break;
    }

  GDK_END_MACOS_ALLOC_POOL;

  return subpixel_layout;
#endif

  return GDK_SUBPIXEL_LAYOUT_UNKNOWN;
}

char *
_gdk_macos_monitor_get_localized_name (NSScreen *screen)
{
  GDK_BEGIN_MACOS_ALLOC_POOL;

  NSString *str;
  char *name;

  g_assert (screen);

  str = [screen localizedName];
  name = g_strdup ([str UTF8String]);

  GDK_END_MACOS_ALLOC_POOL;

  return g_steal_pointer (&name);
}

char *
_gdk_macos_monitor_get_connector_name (CGDirectDisplayID screen_id)
{
  guint unit = CGDisplayUnitNumber (screen_id);
  return g_strdup_printf ("unit-%u", unit);
}

static NSScreen *
find_screen (CGDirectDisplayID screen_id)
{
  GDK_BEGIN_MACOS_ALLOC_POOL;

  NSScreen *screen = NULL;

  for (id obj in [NSScreen screens])
    {
      if (screen_id == [[[obj deviceDescription] objectForKey:@"NSScreenNumber"] unsignedIntValue])
        {
          screen = (NSScreen *)obj;
          break;
        }
    }

  GDK_END_MACOS_ALLOC_POOL;

  return screen;
}

static gboolean
gdk_macos_monitor_display_link_cb (GdkMacosMonitor *this)
{
  gint64 presentation_time;
  gint64 refresh_interval;
  GList *iter;

  g_assert (GDK_IS_MACOS_MONITOR (this));
  g_assert (!this->display_link->paused);

  this->in_frame = TRUE;

  presentation_time = this->display_link->presentation_time;
  refresh_interval = this->display_link->refresh_interval;

  iter = this->awaiting_frames.head;

  while (iter != NULL)
    {
      GdkMacosSurface *surface = iter->data;

      g_assert (GDK_IS_MACOS_SURFACE (surface));

      iter = iter->next;

      g_queue_unlink (&this->awaiting_frames, &surface->frame);
      _gdk_macos_surface_frame_presented (surface, presentation_time, refresh_interval);
    }

  if (this->awaiting_frames.length == 0 && !this->display_link->paused)
    gdk_display_link_source_pause (this->display_link);

  this->in_frame = FALSE;

  return G_SOURCE_CONTINUE;
}

static void
_gdk_macos_monitor_reset_display_link (GdkMacosMonitor  *this,
                                       CGDisplayModeRef  mode)
{
  GSource *source;

  g_assert (GDK_IS_MACOS_MONITOR (this));

  if (this->display_link)
    {
      g_source_destroy ((GSource *)this->display_link);
      g_clear_pointer ((GSource **)&this->display_link, g_source_unref);
    }

  source = gdk_display_link_source_new (this->screen_id, mode);
  this->display_link = (GdkDisplayLinkSource *)source;
  g_source_set_callback (source,
                         (GSourceFunc) gdk_macos_monitor_display_link_cb,
                         this,
                         NULL);
  g_source_attach (source, NULL);
}

gboolean
_gdk_macos_monitor_reconfigure (GdkMacosMonitor *this)
{
  GdkSubpixelLayout subpixel_layout;
  CGDisplayModeRef mode;
  GdkMacosDisplay *display;
  NSScreen *screen;
  GdkRectangle geom;
  gboolean has_opengl;
  CGSize size;
  CGRect bounds;
  size_t width;
  size_t pixel_width;
  char *connector;
  char *name;
  int refresh_rate;
  int scale_factor = 1;
  int width_mm;
  int height_mm;

  g_return_val_if_fail (GDK_IS_MACOS_MONITOR (this), FALSE);

  display = GDK_MACOS_DISPLAY (GDK_MONITOR (this)->display);

  if (!(screen = find_screen (this->screen_id)) ||
      !(mode = CGDisplayCopyDisplayMode (this->screen_id)))
    return FALSE;

  size = CGDisplayScreenSize (this->screen_id);
  bounds = [screen frame];
  width = CGDisplayModeGetWidth (mode);
  pixel_width = CGDisplayModeGetPixelWidth (mode);
  has_opengl = CGDisplayUsesOpenGLAcceleration (this->screen_id);
  subpixel_layout = GetSubpixelLayout (this->screen_id);
  name = _gdk_macos_monitor_get_localized_name (screen);
  connector = _gdk_macos_monitor_get_connector_name (this->screen_id);

  if (width != 0 && pixel_width != 0)
    scale_factor = MAX (1, pixel_width / width);

  width_mm = size.width;
  height_mm = size.height;

  geom.x = bounds.origin.x - display->min_x;
  geom.y = display->height - bounds.origin.y - bounds.size.height + display->min_y;
  geom.width = bounds.size.width;
  geom.height = bounds.size.height;

  /* We will often get 0 back from CGDisplayModeGetRefreshRate().  We
   * can fallback by getting it from CoreVideo based on a CVDisplayLink
   * setting (which is also used by the frame clock).
   */
  if (!(refresh_rate = CGDisplayModeGetRefreshRate (mode)))
    refresh_rate = 60 * 1000;

  gdk_monitor_set_connector (GDK_MONITOR (this), connector);
  gdk_monitor_set_model (GDK_MONITOR (this), name);
  gdk_monitor_set_geometry (GDK_MONITOR (this), &geom);
  gdk_monitor_set_physical_size (GDK_MONITOR (this), width_mm, height_mm);
  gdk_monitor_set_scale_factor (GDK_MONITOR (this), scale_factor);
  gdk_monitor_set_refresh_rate (GDK_MONITOR (this), refresh_rate);
  gdk_monitor_set_subpixel_layout (GDK_MONITOR (this), subpixel_layout);

  this->workarea = [screen visibleFrame];

  /* We might be able to use this at some point to change which GSK renderer
   * we use for surfaces on this monitor.  For example, it might be better
   * to use cairo if we cannot use OpenGL (or it would be software) anyway.
   * Presumably that is more common in cases where macOS is running under
   * an emulator such as QEMU.
   */
  this->has_opengl = !!has_opengl;

  /* Create a new display link to receive feedback about when to render */
  _gdk_macos_monitor_reset_display_link (this, mode);

  CGDisplayModeRelease (mode);
  g_free (name);
  g_free (connector);

  return TRUE;
}

GdkMacosMonitor *
_gdk_macos_monitor_new (GdkMacosDisplay   *display,
                        CGDirectDisplayID  screen_id)
{
  GdkMacosMonitor *this;

  g_return_val_if_fail (GDK_IS_MACOS_DISPLAY (display), NULL);

  this = g_object_new (GDK_TYPE_MACOS_MONITOR,
                       "display", display,
                       NULL);

  this->screen_id = screen_id;

  _gdk_macos_monitor_reconfigure (this);

  return g_steal_pointer (&this);
}

CGDirectDisplayID
_gdk_macos_monitor_get_screen_id (GdkMacosMonitor *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_MONITOR (this), 0);

  return this->screen_id;
}

CGColorSpaceRef
_gdk_macos_monitor_copy_colorspace (GdkMacosMonitor *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_MONITOR (this), NULL);

  return CGDisplayCopyColorSpace (this->screen_id);
}

void
_gdk_macos_monitor_add_frame_callback (GdkMacosMonitor *this,
                                       GdkMacosSurface *surface)
{
  g_return_if_fail (GDK_IS_MACOS_MONITOR (this));
  g_return_if_fail (GDK_IS_MACOS_SURFACE (surface));
  g_return_if_fail (surface->frame.data == (gpointer)surface);
  g_return_if_fail (surface->frame.prev == NULL);
  g_return_if_fail (surface->frame.next == NULL);
  g_return_if_fail (this->awaiting_frames.head != &surface->frame);
  g_return_if_fail (this->awaiting_frames.tail != &surface->frame);

  /* Processing frames is always head to tail, so push to the
   * head so that we don't possibly re-enter this right after
   * adding to the queue.
   */
  if (!queue_contains (&this->awaiting_frames, &surface->frame))
    {
      g_queue_push_head_link (&this->awaiting_frames, &surface->frame);

      if (!this->in_frame && this->awaiting_frames.length == 1)
        gdk_display_link_source_unpause (this->display_link);
    }
}

void
_gdk_macos_monitor_remove_frame_callback (GdkMacosMonitor *this,
                                          GdkMacosSurface *surface)
{
  g_return_if_fail (GDK_IS_MACOS_MONITOR (this));
  g_return_if_fail (GDK_IS_MACOS_SURFACE (surface));
  g_return_if_fail (surface->frame.data == (gpointer)surface);

  if (queue_contains (&this->awaiting_frames, &surface->frame))
    {
      g_queue_unlink (&this->awaiting_frames, &surface->frame);

      if (!this->in_frame && this->awaiting_frames.length == 0)
        gdk_display_link_source_pause (this->display_link);
    }
}

void
_gdk_macos_monitor_clamp (GdkMacosMonitor *this,
                          GdkRectangle    *area)
{
  GdkRectangle workarea;
  GdkRectangle geom;

  g_return_if_fail (GDK_IS_MACOS_MONITOR (this));
  g_return_if_fail (area != NULL);

  gdk_macos_monitor_get_workarea (GDK_MONITOR (this), &workarea);
  gdk_monitor_get_geometry (GDK_MONITOR (this), &geom);

  if (area->x + area->width > workarea.x + workarea.width)
    area->x = workarea.x + workarea.width - area->width;

  if (area->x < workarea.x)
    area->x = workarea.x;

  if (area->y + area->height > workarea.y + workarea.height)
    area->y = workarea.y + workarea.height - area->height;

  if (area->y < workarea.y)
    area->y = workarea.y;
}
