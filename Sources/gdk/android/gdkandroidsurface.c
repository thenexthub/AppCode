/*
 * Copyright (c) 2024 Florian "sp1rit" <sp1rit@disroot.org>
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

#include <math.h>

#include "gdkframeclockidleprivate.h"
#include "gdkglcontextprivate.h"

#include <android/native_window_jni.h>

#include "gdkandroidinit-private.h"
#include "gdkandroiddisplay-private.h"
#include "gdkandroidtoplevel-private.h"
#include "gdkandroidevents-private.h"
#include "gdkandroiddnd-private.h"
#include "gdkandroidutils-private.h"

#include "gdkandroidsurface-private.h"

#define gdk_android_check_surface(surface) {             \
    if (!surface)                                        \
      return;                                            \
    g_return_if_fail (GDK_IS_ANDROID_SURFACE (surface)); \
  }
#define gdk_android_check_surface_val(surface, val) {               \
    if (!surface)                                                   \
      return val;                                                   \
    g_return_val_if_fail (GDK_IS_ANDROID_SURFACE (surface), (val)); \
  }
#define gdk_android_check_surface_throw(surface, object) {                                                                                                                              \
    if (!surface)                                                                                                                                                                       \
      {                                                                                                                                                                                 \
        jthrowable exception = (*env)->NewObject (env, gdk_android_get_java_cache ()->surface_exception.klass, gdk_android_get_java_cache ()->surface_exception.constructor, (object)); \
        (*env)->Throw (env, exception);                                                                                                                                                 \
        (*env)->DeleteLocalRef (env, exception);                                                                                                                                        \
        return;                                                                                                                                                                         \
      }                                                                                                                                                                                 \
    g_return_if_fail (GDK_IS_ANDROID_SURFACE (surface));                                                                                                                                \
  }

static void
gdk_android_surface_reposition_children (GdkAndroidSurface *this)
{
  for (const GList *iter = GDK_SURFACE (this)->children;
       iter != NULL;
       iter = iter->next)
    {
      GdkAndroidSurface *child = iter->data;
      g_assert (GDK_IS_ANDROID_SURFACE (child));

      GDK_ANDROID_SURFACE_GET_CLASS (child)->reposition (child);
    }
}

static void
gdk_android_surface_drop_child_refs (GdkAndroidSurface *this)
{
  GdkSurface *surface = (GdkSurface *)this;
  JNIEnv *env = gdk_android_get_env();
  for (const GList *i = surface->children; i != NULL; i = i->next)
    {
      GdkAndroidSurface *child = (GdkAndroidSurface *)i->data;
      if (child->surface)
        {
          (*env)->CallVoidMethod (env, child->surface, gdk_android_get_java_cache ()->surface.drop);
          (*env)->DeleteGlobalRef (env, child->surface);
          child->surface = NULL;
        }
      gdk_android_surface_drop_child_refs(child);
    }
}

static void
gdk_android_surface_set_input_region (GdkSurface     *surface,
                                      cairo_region_t *shape_region);

void
_gdk_android_surface_bind_native (JNIEnv *env, jobject this, jlong identifier)
{
  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface_throw (this, this);

  g_debug ("TRACE: Surface.BindNative (%p [%s])", this, G_OBJECT_TYPE_NAME (this));

  if (this->surface)
    {
      (*env)->CallVoidMethod (env, this->surface, gdk_android_get_java_cache ()->surface.drop);
      (*env)->DeleteGlobalRef (env, this->surface);
    }
  gdk_android_surface_drop_child_refs(this);
  this->surface = (*env)->NewGlobalRef (env, this);

  (*env)->CallVoidMethod(env, this, gdk_android_get_java_cache ()->surface.set_visibility, this->visible);

  gdk_android_surface_set_input_region ((GdkSurface *) this, ((GdkSurface *) this)->input_region);
}

void
_gdk_android_surface_on_attach (JNIEnv *env, jobject this)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);
  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface (this);

  g_debug ("TRACE: Surface.OnAttach (%p [%s])", this, G_OBJECT_TYPE_NAME (this));
}

static void
gdk_android_surface_handle_map (GdkAndroidSurface *this);
void
_gdk_android_surface_on_layout_surface (JNIEnv *env, jobject this,
                                        jint width, jint height,
                                        jfloat scale)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);
  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface (this);

  g_debug ("TRACE: Surface.OnLayoutSurface (%p [%s]): %dx%d @ %f", this, G_OBJECT_TYPE_NAME (this), width, height, scale);

  this->cfg.width = width;
  this->cfg.height = height;
  this->cfg.scale = scale;

  GdkAndroidSurfaceClass *klass = GDK_ANDROID_SURFACE_GET_CLASS (this);
  if (klass->on_layout)
    klass->on_layout (this);

  GdkSurface *surface = (GdkSurface *)this;
  surface->width = ceilf (this->cfg.width / this->cfg.scale);
  surface->height = ceilf (this->cfg.height / this->cfg.scale);
  surface->x = this->cfg.x / this->cfg.scale;
  surface->y = this->cfg.y / this->cfg.scale;

  g_debug ("New surface bounds: %dx%d at (%d|%d)", surface->width, surface->height, surface->x, surface->y);
  _gdk_surface_update_size (surface);
  gdk_android_surface_reposition_children (this);
  gdk_surface_invalidate_rect (surface, NULL);

  gdk_surface_request_layout ((GdkSurface *) this);

  if (this->delayed_map)
    {
      gdk_android_surface_handle_map (this);
      this->delayed_map = FALSE;
    }
}

void
_gdk_android_surface_on_layout_position (JNIEnv *env, jobject this,
                                         jint x, jint y)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);
  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface (this);

  g_debug ("TRACE: Surface.OnLayoutPosition (%p [%s]): (%d|%d)", this, G_OBJECT_TYPE_NAME (this), x, y);

  this->cfg.x = x;
  this->cfg.y = y;

  GdkAndroidSurfaceClass *klass = GDK_ANDROID_SURFACE_GET_CLASS (this);
  if (klass->on_layout)
    klass->on_layout (this);

  GdkSurface *surface = (GdkSurface *)this;
  surface->x = this->cfg.x / this->cfg.scale;
  surface->y = this->cfg.y / this->cfg.scale;

  gdk_android_surface_reposition_children (this);
}

void
_gdk_android_surface_on_detach (JNIEnv *env, jobject this)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface (this);

  g_debug ("TRACE: Surface.OnDetach (%p [%s])", this, G_OBJECT_TYPE_NAME (this));
}

void
_gdk_android_surface_on_dnd_start_failed (JNIEnv *env, jobject this,
                                          jobject native_identifier)
{
  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  gdk_android_dnd_handle_drag_start_fail (display, native_identifier);
}

void
_gdk_android_surface_on_motion_event (JNIEnv *env, jobject this,
                                      jint event_identifier, jobject event)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface (this);

  gdk_android_events_handle_motion_event (this, event, event_identifier);
}

void
_gdk_android_surface_on_key_event (JNIEnv *env, jobject this, jobject event)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface (this);

  gdk_android_events_handle_key_event (this, event);
}

jboolean
_gdk_android_surface_on_drag_event (JNIEnv *env, jobject this, jobject event)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_surface_val (this, FALSE);

  return gdk_android_dnd_surface_handle_drop_event (this, event);
}

typedef struct
{
  pthread_barrier_t event_loop_idle;
  pthread_barrier_t surface_update_complete;
  GdkAndroidSurface *this;
  gboolean visible;
} GdkAndroidSurfaceOnVisibilityData;

static void
gdk_android_surface_do_map_cb (GdkSeat    *seat,
                               GdkSurface *surface,
                               gpointer    user_data)
{
  gdk_surface_set_is_mapped (surface, TRUE);
}

static void
gdk_android_surface_handle_map (GdkAndroidSurface *this)
{
  GdkSurface *surface = (GdkSurface *) this;
  GdkAndroidDisplay *display = (GdkAndroidDisplay *) gdk_surface_get_display (surface);

  // Resynchronize visibility state in case of mismatch.
  //   This can happen if there are two events queued where the first presents
  //   the popup surface while the second hides it again. As the second event
  //   triggers before the android eventloop OnVisibility synchronization happens
  //   the surface should actually be hidden.
  //   Early return, as seat_grab can also lead to popup_present calls.
  JNIEnv *env = gdk_android_get_env();
  if (!this->visible)
    {
      (*env)->CallVoidMethod (env, this->surface, gdk_android_get_java_cache ()->surface.set_visibility, FALSE);
      return;
    }

  if (surface->autohide)
    {
      g_debug ("Grabbing surface %p [%s]", this, G_OBJECT_TYPE_NAME (this));
      gdk_seat_grab ((GdkSeat *) display->seat,
                     surface,
                     GDK_SEAT_CAPABILITY_ALL,
                     TRUE,
                     NULL, NULL,
                     gdk_android_surface_do_map_cb, NULL);
    }
  else
    {
      gdk_surface_set_is_mapped (surface, TRUE);
    }
}

static gboolean
gdk_android_surface_eventloop_idle (GdkAndroidSurfaceOnVisibilityData *data)
{
  // wait for the even loop calling this function
  pthread_barrier_wait (&data->event_loop_idle);

  // data becomes invalid after the next call, so copy what is needed
  GdkAndroidSurface *this = g_object_ref (data->this);
  gboolean visible = data->visible;

  // wait for the ui thread to have updated the native window
  pthread_barrier_wait (&data->surface_update_complete);

  GdkSurface *surface = (GdkSurface *) this;

  g_debug ("TRACE: Surface.OnVisibility (%p [%s])", this, G_OBJECT_TYPE_NAME (this));
  // GdkFrameClock *clock = gdk_surface_get_frame_clock(surface);
  if (visible)
    {
      // GDK_FRAME_CLOCK_GET_CLASS(clock)->thaw(clock); // is this legal?
      // gdk_android_surface_queue_configuration_update(this);
      if (this->cfg.x == 0 || this->cfg.y == 0)
        {
          this->delayed_map = TRUE;
        }
      else
        {
          this->delayed_map = FALSE;
          gdk_android_surface_handle_map (this);
        }
    }
  else
    {
      // GDK_FRAME_CLOCK_GET_CLASS(clock)->freeze(clock);

      // This is a really ugly hack to prevent the visibility getting set to
      // FALSE, after Gdk.Surface.set_is_mapped (false) triggers
      // Gdk.Surface.hide getting called. Just resetting to the previous
      // visibility state seems to mostly work and the reposition calls on
      // popups to properly push or reposition themselves on the ToplevelView.
      gboolean old_visibility = this->visible;

      this->delayed_map = FALSE;
      // There is still an issue where GdkSurface.set_is_mapped triggers
      // Gtk.Widget.hide, which inturn cause a GtkWidget.unparent -> unrealize,
      // which then calls Gdk.Surface.destroy and fully removes the popup, even
      // tho it is only shortly gone during activity recreation. As the grab
      // doesn't get removed, no further inputs gets processed due to GDK
      // complaining about inconsistent input state.
      // I don't really know how to fix this one...
      gdk_surface_set_is_mapped (surface, FALSE);

      // cont. ugly hack
      this->visible = old_visibility;
    }

  g_object_unref (this);
  return G_SOURCE_REMOVE;
}

void
_gdk_android_surface_on_visibility_ui_thread (JNIEnv *env, jobject this,
                                              jboolean visible)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->surface.surface_identifier);
  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidSurface *this = gdk_android_display_get_surface_from_identifier (display, identifier);
  if (!this)
    return;
  if (!GDK_IS_ANDROID_SURFACE (this))
    {
      g_object_unref (this);
      return;
    }

  g_debug ("TRACE: [UiThread] Surface.OnVisibility (%p [%s]): %d", this, G_OBJECT_TYPE_NAME (this), visible);

  GdkAndroidSurfaceOnVisibilityData data;
  pthread_barrier_init (&data.event_loop_idle, NULL, 2);
  pthread_barrier_init (&data.surface_update_complete, NULL, 2);
  data.this = this;
  data.visible = visible;

  g_idle_add_full (G_PRIORITY_DEFAULT - 10, G_SOURCE_FUNC (gdk_android_surface_eventloop_idle), &data, NULL);
  pthread_barrier_wait (&data.event_loop_idle); // wait for eventloop idle

  g_mutex_lock (&this->native_lock);

  g_clear_pointer (&this->native, ANativeWindow_release);
  if (visible)
    {
      (*env)->PushLocalFrame (env, 2);
      jobject holder = (*env)->CallObjectMethod (env, this, gdk_android_get_java_cache ()->surface.get_holder);
      jobject android_surface = (*env)->CallObjectMethod (env, holder, gdk_android_get_java_cache ()->a_surfaceholder.get_surface);
      this->native = ANativeWindow_fromSurface (env, android_surface);
      (*env)->PopLocalFrame (env, NULL);
    }

  GdkDrawContext *attached = gdk_surface_get_attached_context ((GdkSurface *)this);
  if (GDK_IS_GL_CONTEXT (attached))
    gdk_gl_context_set_egl_native_window ((GdkGLContext *)attached, this->native);

  g_mutex_unlock (&this->native_lock);

  pthread_barrier_wait (&data.surface_update_complete); // immutable the eventloop continue running
  // The bionic pthread barrier implementation behaves a bit wierd when doing calling wait multiple
  // times per instance. Because of this, we just have two barriers for the two stages instead of
  // reusing a single one.
  pthread_barrier_destroy (&data.event_loop_idle);
  pthread_barrier_destroy (&data.surface_update_complete);

  g_object_unref (this);
}

/**
 * GdkAndroidSurface:
 *
 * The Android implementation of [class@Gdk.Surface].
 *
 * On Android, each surface is backed by an Android
 * [SurfaceView](https://developer.android.com/reference/android/view/SurfaceView).
 *
 * Since: 4.18
 */
G_DEFINE_TYPE (GdkAndroidSurface, gdk_android_surface, GDK_TYPE_SURFACE)

static void
gdk_android_surface_finalize (GObject *object)
{
  GdkAndroidSurface *this = GDK_ANDROID_SURFACE (object);
  g_debug ("Finalizing surface %p [%s]", (gpointer) this, G_OBJECT_TYPE_NAME (this));
  if (this->active_drop)
    {
      g_info ("Finalizing surface %p with active drop operation", this);
      g_object_unref (this->active_drop);
    }
  JNIEnv *env = gdk_android_get_env ();
  if (this->native)
    ANativeWindow_release (this->native);
  if (this->surface)
    (*env)->DeleteGlobalRef (env, this->surface);
  g_mutex_clear (&this->native_lock);
  G_OBJECT_CLASS (gdk_android_surface_parent_class)->finalize (object);
}

static void
gdk_android_surface_frame_clock_after_paint (GdkFrameClock *clock,
                                             GdkSurface    *surface)
{
  GdkAndroidSurface *this = (GdkAndroidSurface *)surface;
  if (!this->surface)
    return;

  GdkFrameTimings *timings = gdk_frame_clock_get_timings (clock, gdk_frame_clock_get_frame_counter (clock));
  if (!timings)
    return;

  JNIEnv *env = gdk_android_get_env();
  (*env)->PushLocalFrame (env, 1);
  jobject view = (*env)->CallObjectMethod (env, this->surface,
                                           gdk_android_get_java_cache ()->a_view.get_display);
  if (!view)
    goto exit;
  gfloat refresh = (*env)->CallFloatMethod (env, view,
                                            gdk_android_get_java_cache ()->a_display.get_refresh_rate);
  timings->refresh_interval = 1000000.f / refresh; // \frac{1}{refresh} * 10^6
  timings->presentation_time = 0;

  timings->complete = TRUE;
exit:
  (*env)->PopLocalFrame (env, NULL);
}

static void
gdk_android_surface_constructed (GObject *object)
{
  GdkAndroidSurface *this = GDK_ANDROID_SURFACE (object);
  GdkSurface *surface = (GdkSurface *) this;
  GdkAndroidDisplay *display = GDK_ANDROID_DISPLAY (gdk_surface_get_display (surface));

  GdkFrameClock *frame_clock = _gdk_frame_clock_idle_new ();
  gdk_surface_set_frame_clock (surface, frame_clock);
  // GDK_FRAME_CLOCK_GET_CLASS(frame_clock)->freeze(frame_clock);
  g_signal_connect (frame_clock,
                    "after-paint",
                    G_CALLBACK (gdk_android_surface_frame_clock_after_paint),
                    this);
  g_object_unref (frame_clock);

  gdk_android_display_add_surface (display, this);

  G_OBJECT_CLASS (gdk_android_surface_parent_class)->constructed (object);
}

static void
gdk_android_surface_hide (GdkSurface *surface)
{
  GdkAndroidSurface *this = (GdkAndroidSurface *) surface;

  // Being able to call hide implies there being the possibility to unhide it again.
  // Where should that be implemented?
  g_debug ("AndroidSurface: hide called %p [%s] (%d %d)", this,
           G_OBJECT_TYPE_NAME (this), surface->is_mapped, surface->pending_is_mapped);

  this->visible = FALSE;

  if (!this->surface || surface->destroyed)
    return;

  JNIEnv *env = gdk_android_get_env ();

  if (surface->is_mapped)
    (*env)->CallVoidMethod (env, this->surface, gdk_android_get_java_cache ()->surface.set_visibility, FALSE);

  _gdk_surface_clear_update_area (surface);
}

static void
gdk_android_surface_get_geometry (GdkSurface *surface,
                                  int *x, int *y,
                                  int *width, int *height)
{
  *x = surface->x;
  *y = surface->y;
  *width = surface->width;
  *height = surface->height;
}

static void
gdk_android_surface_get_root_coords (GdkSurface *surface,
                                     int x, int y,
                                     int *root_x, int *root_y)
{
  *root_x = x - surface->x;
  *root_y = y - surface->y;
}

static gboolean
gdk_android_surface_get_device_state (GdkSurface      *surface,
                                      GdkDevice       *device,
                                      double *x, double *y,
                                      GdkModifierType *mask)
{
  return gdk_device_get_surface_at_position (device, x, y) == surface;
}

static void
gdk_android_surface_set_input_region (GdkSurface     *surface,
                                      cairo_region_t *shape_region)
{
  GdkAndroidSurface *this = (GdkAndroidSurface *) surface;
  if (!this->surface)
    return;

  JNIEnv *env = gdk_android_get_env ();
  if (!shape_region)
    {
      (*env)->CallVoidMethod (env,
                              this->surface,
                              gdk_android_get_java_cache ()->surface.set_input_region,
                              NULL);
      return;
    }

  (*env)->PushLocalFrame (env, 2);

  gint n_rects = cairo_region_num_rectangles (shape_region);
  jobjectArray jrects = (*env)->NewObjectArray (env, n_rects, gdk_android_get_java_cache ()->a_rectf.klass, NULL);
  for (gint i = 0; i < n_rects; i++)
    {
      cairo_rectangle_int_t rect;
      cairo_region_get_rectangle (shape_region, i, &rect);

      jobject jrect = (*env)->NewObject (env, gdk_android_get_java_cache ()->a_rectf.klass,
                                         gdk_android_get_java_cache ()->a_rectf.constructor,
                                         (jfloat) (rect.x * this->cfg.scale),
                                         (jfloat) (rect.y * this->cfg.scale),
                                         (jfloat) ((rect.x + rect.width) * this->cfg.scale),
                                         (jfloat) ((rect.y + rect.height) * this->cfg.scale));
      (*env)->SetObjectArrayElement (env, jrects, i, jrect);
      (*env)->DeleteLocalRef (env, jrect);
    }

  (*env)->CallVoidMethod (env, this->surface, gdk_android_get_java_cache ()->surface.set_input_region, jrects);

  (*env)->PopLocalFrame (env, NULL);
}

static void
gdk_android_surface_destroy (GdkSurface *surface, gboolean foreign_destroy)
{
  GdkAndroidSurface *this = GDK_ANDROID_SURFACE (surface);
  GdkAndroidDisplay *display = GDK_ANDROID_DISPLAY (gdk_surface_get_display (surface));
  g_debug ("Destroying Surface %p (foreign: %d)", (gpointer) this, foreign_destroy);

  g_signal_handlers_disconnect_by_func (gdk_surface_get_frame_clock (surface),
                                        gdk_android_surface_frame_clock_after_paint,
                                        surface);

  if (!foreign_destroy && this->surface)
    {
      JNIEnv *env = gdk_android_get_env ();
      (*env)->CallVoidMethod (env, this->surface, gdk_android_get_java_cache ()->surface.drop);
    }
  g_hash_table_remove (display->surfaces, this);
}

static gdouble
gdk_android_surface_get_scale (GdkSurface *surface)
{
  GdkAndroidSurface *this = GDK_ANDROID_SURFACE (surface);
  return this->cfg.scale;
}

static gboolean
gdk_android_surface_compute_size (GdkSurface *surface)
{
  return FALSE;
}

static void
gdk_android_surface_reposition (GdkAndroidSurface *surface)
{
}

static void
gdk_android_surface_class_init (GdkAndroidSurfaceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GdkSurfaceClass *surface_class = GDK_SURFACE_CLASS (klass);

  object_class->finalize = gdk_android_surface_finalize;
  object_class->constructed = gdk_android_surface_constructed;

  surface_class->hide = gdk_android_surface_hide;
  surface_class->get_geometry = gdk_android_surface_get_geometry;
  surface_class->get_root_coords = gdk_android_surface_get_root_coords;
  surface_class->get_device_state = gdk_android_surface_get_device_state;
  surface_class->set_input_region = gdk_android_surface_set_input_region;
  surface_class->destroy = gdk_android_surface_destroy;
  surface_class->drag_begin = gdk_android_dnd_surface_drag_begin;
  surface_class->get_scale = gdk_android_surface_get_scale;
  surface_class->compute_size = gdk_android_surface_compute_size;

  klass->reposition = gdk_android_surface_reposition;
}

static void
gdk_android_surface_init (GdkAndroidSurface *this)
{
  g_mutex_init (&this->native_lock);

  this->delayed_map = FALSE;
  this->cfg = (GdkAndroidSurfaceConfiguration){
    .x = 0, .y = 0, .width = 0, .height = 0, .scale = 1.f
  };
}

GdkAndroidToplevel *
gdk_android_surface_get_toplevel (GdkAndroidSurface *this)
{
  GdkSurface *surface = (GdkSurface *) this;
  if (surface->parent)
    return gdk_android_surface_get_toplevel (GDK_ANDROID_SURFACE (surface->parent));
  return GDK_ANDROID_TOPLEVEL (this);
}
