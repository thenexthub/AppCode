/*
 * Copyright (c) 2024-2025 Florian "sp1rit" <sp1rit@disroot.org>
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

#include "gdktoplevellayout.h"

#include "gdkdisplayprivate.h"
#include "gdkeventsprivate.h"

#include "gdkandroidinit-private.h"
#include "gdkandroiddisplay-private.h"
#include "gdkandroidclipboard-private.h"
#include "gdkandroidutils-private.h"

#include "gdkandroidtoplevel-private.h"

typedef struct _GdkAndroidToplevelActivityRequest {
  jobject parent_activity;
  guint request_code;
  guint signal_id;
  GTask *task;
} GdkAndroidToplevelActivityRequest;
static void
gdk_android_toplevel_activity_request_free (GdkAndroidToplevelActivityRequest *data)
{
  JNIEnv *env = gdk_android_get_env();
  (*env)->CallVoidMethod (env, data->parent_activity,
                          gdk_android_get_java_cache ()->a_activity.finish_activity,
                          data->request_code);
  (*env)->DeleteGlobalRef (env, data->parent_activity);
  if (data->task)
    g_task_return_new_error (data->task, G_IO_ERR, G_IO_ERROR_CANCELLED, "Activity destroyed before it received a response");
  g_free (data);
}

#define gdk_android_check_toplevel(toplevel) {             \
    if (!toplevel)                                         \
      return;                                              \
    g_return_if_fail (GDK_IS_ANDROID_TOPLEVEL (toplevel)); \
  }
#define gdk_android_check_toplevel_throw(toplevel, object) {                                                                                                                            \
    if (!toplevel)                                                                                                                                                                      \
      {                                                                                                                                                                                 \
        jthrowable exception = (*env)->NewObject (env, gdk_android_get_java_cache ()->surface_exception.klass, gdk_android_get_java_cache ()->surface_exception.constructor, (object)); \
        (*env)->Throw (env, exception);                                                                                                                                                 \
        (*env)->DeleteLocalRef (env, exception);                                                                                                                                        \
        return;                                                                                                                                                                         \
      }                                                                                                                                                                                 \
    g_return_if_fail (GDK_IS_ANDROID_TOPLEVEL (toplevel));                                                                                                                              \
  }

static void
gdk_android_toplevel_update_title (GdkAndroidToplevel *this);

static void
gdk_android_toplevel_update_window (GdkAndroidToplevel *this);

void
_gdk_android_toplevel_bind_native (JNIEnv *env, jobject this,
                                   jlong native_identifier)
{
  gdk_android_set_latest_activity (env, this);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidToplevel *this = g_hash_table_lookup (display->surfaces, (gpointer) native_identifier);
  gdk_android_check_toplevel_throw (this, this);

  g_debug ("TRACE: Toplevel.BindNative (%p)", this);

  if (this->activity)
    (*env)->DeleteGlobalRef (env, this->activity);
  this->activity = (*env)->NewGlobalRef (env, this);
  (*env)->SetLongField (env, this, gdk_android_get_java_cache ()->toplevel.native_identifier, native_identifier);

  (*env)->CallVoidMethod (env, this, gdk_android_get_java_cache ()->toplevel.attach_toplevel_surface);

  gdk_android_toplevel_update_title (this);
  gdk_android_toplevel_update_window (this);

  gdk_android_display_update_night_mode (display, this);
}

void
_gdk_android_toplevel_on_configuration_change (JNIEnv *env, jobject this)
{
  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  gdk_android_display_update_night_mode (display, this);
}

void
_gdk_android_toplevel_on_state_change (JNIEnv *env, jobject this, jboolean has_focus, jboolean is_fullscreen)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->toplevel.native_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidToplevel *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_toplevel (this);
  GdkSurface *surface = (GdkSurface *) this;

  g_debug ("TRACE: Toplevel.OnStateChenge (%p)", this);

  GdkToplevelState unset = 0;
  GdkToplevelState set = 0;

  *(has_focus ? &set : &unset) |= GDK_TOPLEVEL_STATE_FOCUSED;
  if (has_focus)
    gdk_android_clipboard_update_remote_formats ((GdkAndroidClipboard *) ((GdkDisplay *) display)->clipboard);

  *(is_fullscreen ? &set : &unset) |= GDK_TOPLEVEL_STATE_FULLSCREEN;
  /*GDK_TOPLEVEL_STATE_MINIMIZED
  GDK_TOPLEVEL_STATE_MAXIMIZED
  GDK_TOPLEVEL_STATE_SUSPENDED*/

  gdk_synthesize_surface_state (surface, unset, set);
}

void
_gdk_android_toplevel_on_back_press (JNIEnv *env, jobject this)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->toplevel.native_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidToplevel *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_toplevel (this);
  GdkSurface *surface = (GdkSurface *) this;

  g_debug ("TRACE: Toplevel.OnBackPress (%p)", this);

  GdkEvent *event = gdk_delete_event_new (surface);
  gdk_surface_handle_event (event);
  gdk_event_unref (event);
}

void
_gdk_android_toplevel_on_destroy (JNIEnv *env, jobject this)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->toplevel.native_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidToplevel *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_toplevel (this);
  GdkAndroidSurface *surface_impl = (GdkAndroidSurface *) this;
  GdkSurface *surface = (GdkSurface *) this;

  g_debug ("TRACE: On Destroy GdkAndroidToplevel %lx, (%p)", identifier, (gpointer) this);

  if (!GDK_SURFACE_DESTROYED (surface))
    {
      if (!surface_impl->visible)
        {
          this->did_spawn_activity = FALSE;
          return;
        }

      g_info ("GdkAndroidToplevel (%p): OS destroyed activity", (gpointer) this);
      // TODO: is there no better way of letting GTK know a surface no longer exits?
      //  the issue with this is, that if a toplevel with modal is open (grabbed),
      //  the delete event on any other widget will not be handled (see gtkmain.c,
      //  the GDK_DELETE branch of the switch in gtk_main_do_event)
      GdkEvent *event = gdk_delete_event_new (surface);
      gdk_surface_handle_event (event);
      gdk_event_unref (event);

      if (!GDK_SURFACE_DESTROYED (surface))
        {
          g_warning ("GdkAndroidToplevel (%p): Force destroying activity", (gpointer) this);
          _gdk_surface_destroy (surface, TRUE);
        }
    }
  else
    {
      // This should be unreachable as gdk_android_check_toplevel should have returned
      g_return_if_reached ();
    }
}

void
_gdk_android_toplevel_on_activity_result (JNIEnv *env, jobject this,
                                          jint request_code,
                                          jint response_code,
                                          jobject result)
{
  glong identifier = (*env)->GetLongField (env, this, gdk_android_get_java_cache ()->toplevel.native_identifier);

  GdkAndroidDisplay *display = gdk_android_display_get_display_instance ();
  GdkAndroidToplevel *this = g_hash_table_lookup (display->surfaces, (gpointer) identifier);
  gdk_android_check_toplevel (this);

  GdkAndroidToplevelActivityRequest *data = g_hash_table_lookup (this->activity_requests,
                                                                 GUINT_TO_POINTER((guint)request_code));
  g_return_if_fail (data != NULL);

  GCancellable *cancellable = g_task_get_cancellable (data->task);
  if (cancellable)
    g_cancellable_disconnect (cancellable, data->signal_id);

  g_task_set_task_data (data->task, (*env)->NewGlobalRef (env, result),
                        (GDestroyNotify)gdk_android_utils_unref_jobject);
  g_task_return_int (data->task, response_code);

  data->task = NULL;
  g_hash_table_remove (this->activity_requests, GUINT_TO_POINTER((guint)request_code));
}

static void
gdk_android_toplevel_present (GdkToplevel *toplevel, GdkToplevelLayout *layout)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (toplevel);
  GdkAndroidSurface *surface_impl = (GdkAndroidSurface *)this;
  GdkSurface *surface = (GdkSurface *) this;

  surface_impl->visible = TRUE;

  if (layout != this->layout)
    {
      g_clear_pointer (&this->layout, gdk_toplevel_layout_unref);
      this->layout = gdk_toplevel_layout_copy (layout);
    }

  JNIEnv *env = gdk_android_get_env ();
  (*env)->PushLocalFrame (env, 4);
  g_debug ("TRACE: Android.Toplevel present called: %p", (void *) this->intent);
  if (!GDK_SURFACE_IS_MAPPED (surface) && !this->did_spawn_activity)
    {
      if (this->monitor)
        {
          gdk_android_monitor_drop_toplevel (this->monitor);
          g_object_unref (this->monitor);
        }
      if (surface->transient_for && GDK_ANDROID_TOPLEVEL (surface->transient_for)->monitor)
        this->monitor = g_object_ref (GDK_ANDROID_TOPLEVEL (surface->transient_for)->monitor);
      else
        this->monitor = (GdkAndroidMonitor *) gdk_android_monitor_new (gdk_surface_get_display (surface));
      gdk_android_monitor_add_toplevel (this->monitor);

      g_debug ("Spawning activity for toplevel: (transient %p)", surface->transient_for);
      jobject parent = surface->transient_for ? GDK_ANDROID_TOPLEVEL (surface->transient_for)->activity : NULL;
      if (!parent)
        {
          parent = gdk_android_get_activity ();
          gboolean is_bound = (*env)->GetLongField (env, parent, gdk_android_get_java_cache ()->toplevel.native_identifier) != 0;
          if (!is_bound)
            {
              (*env)->CallVoidMethod (env, parent, gdk_android_get_java_cache ()->toplevel.bind_native,
                                      (jlong)this);
              goto skip_new_activity;
            }
          else
            (*env)->CallObjectMethod (env, this->intent, gdk_android_get_java_cache ()->a_intent.add_flags,
                                      gdk_android_get_java_cache ()->a_intent.flag_activity_new_task | gdk_android_get_java_cache ()->a_intent.flag_activity_multiple_task);
        }

      (*env)->CallVoidMethod (env, parent, gdk_android_get_java_cache ()->a_activity.start_activity, this->intent);
skip_new_activity:
      this->did_spawn_activity = TRUE;
    }
  else if (surface_impl->surface)
    {
      (*env)->CallVoidMethod (env, surface_impl->surface, gdk_android_get_java_cache ()->surface.set_visibility, TRUE);
      gdk_android_toplevel_update_window (this);
    }

  (*env)->PopLocalFrame (env, NULL);
}

static gboolean
gdk_android_toplevel_minimize (GdkToplevel *toplevel)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (toplevel);
  JNIEnv *env = gdk_android_get_env ();
  return (*env)->CallBooleanMethod (env, this->activity, gdk_android_get_java_cache ()->a_activity.move_task_to_back, TRUE);
}

static void
gdk_android_toplevel_focus (GdkToplevel *toplevel, guint32 timestamp)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (toplevel);
  if (!this->activity)
    return;

  JNIEnv *env = gdk_android_get_env ();
  (*env)->PushLocalFrame (env, 1);
  jobject activity_service = (*env)->CallObjectMethod (env, this->activity, gdk_android_get_java_cache ()->a_context.get_system_service, gdk_android_get_java_cache ()->a_context.activity_service);
  jint task_id = (*env)->CallIntMethod (env, this->activity, gdk_android_get_java_cache ()->a_activity.get_task_id);
  (*env)->CallVoidMethod (env, activity_service, gdk_android_get_java_cache ()->a_activity_manager.move_task_to_font, task_id, 0, NULL);
  (*env)->PopLocalFrame (env, NULL);
}

static void
gdk_android_toplevel_begin_resize (GdkToplevel   *toplevel,
                                   GdkSurfaceEdge edge,
                                   GdkDevice     *device,
                                   int            button,
                                   double x, double y,
                                   guint32        timestamp)
{}

static void
gdk_android_toplevel_begin_move (GdkToplevel *toplevel,
                                 GdkDevice   *device,
                                 int          button,
                                 double x, double y,
                                 guint32      timestamp)
{}

static void
gdk_android_toplevel_iface_init (GdkToplevelInterface *iface)
{
  iface->present = gdk_android_toplevel_present;
  iface->minimize = gdk_android_toplevel_minimize;
  // I have not found a way to implement lower, but maybe the current minimize implementation
  // should actually matches the behaviour of lower quite closely? This needs to be tested on
  // some large screen device that has free floating windows.
  iface->focus = gdk_android_toplevel_focus;
  iface->begin_resize = gdk_android_toplevel_begin_resize;
  iface->begin_move = gdk_android_toplevel_begin_move;
}


/**
 * GdkAndroidToplevel:
 *
 * The Android implementation of [iface@Gdk.Toplevel].
 *
 * Each [class@Gdk.AndroidToplevel] is backed by an individual
 * [Activity](https://developer.android.com/reference/android/app/Activity)
 * once it has been realized. The activity uses a View which renders a
 * "toplevel surface" over its whole size (used by the
 * [class@Gdk.Surface] parent class of the toplevel) but provides the
 * ability to add further fixed-size surfaces at specific locations that
 * will become [iface@Gdk.Popup].
 *
 * Since: 4.18
 */
G_DEFINE_TYPE_WITH_CODE (GdkAndroidToplevel, gdk_android_toplevel, GDK_TYPE_ANDROID_SURFACE,
                         G_IMPLEMENT_INTERFACE (GDK_TYPE_TOPLEVEL, gdk_android_toplevel_iface_init))

enum
{
  N_PROPERTIES = 1,
};

static void
gdk_android_toplevel_finalize (GObject *object)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (object);

  g_hash_table_unref (this->activity_requests);

  if (this->layout)
    gdk_toplevel_layout_unref (this->layout);
  g_free (this->title);

  if (this->monitor)
    {
      gdk_android_monitor_drop_toplevel (this->monitor);
      g_object_unref (this->monitor);
    }

  JNIEnv *env = gdk_android_get_env ();
  if (this->activity)
    (*env)->DeleteGlobalRef (env, this->activity);
  if (this->intent)
    (*env)->DeleteGlobalRef (env, this->intent);
  G_OBJECT_CLASS (gdk_android_toplevel_parent_class)->finalize (object);
}

static void
gdk_android_display_night_mode_changed (GdkAndroidDisplay  *display,
                                        GParamSpec         *pspec,
                                        GdkAndroidToplevel *this)
{
  gdk_android_toplevel_update_window (this);
}

static void
gdk_android_toplevel_constructed (GObject *object)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (object);

  JNIEnv *env = gdk_android_get_env ();
  (*env)->PushLocalFrame (env, 2);

  jobject intent = (*env)->NewObject (env, gdk_android_get_java_cache ()->a_intent.klass, gdk_android_get_java_cache ()->a_intent.constructor,
                                      gdk_android_get_activity (),
                                      gdk_android_get_java_cache ()->toplevel.klass);
  (*env)->CallObjectMethod (env, intent,
                            gdk_android_get_java_cache ()->a_intent.put_extra_long,
                            gdk_android_get_java_cache ()->toplevel.toplevel_identifier_key,
                            (glong) this);
  this->intent = (*env)->NewGlobalRef (env, intent);
  (*env)->PopLocalFrame (env, NULL);

  GdkDisplay *display = gdk_surface_get_display ((GdkSurface *) this);
  g_signal_connect_object (display, "notify::night-mode", G_CALLBACK (gdk_android_display_night_mode_changed), this, G_CONNECT_DEFAULT);

  G_OBJECT_CLASS (gdk_android_toplevel_parent_class)->constructed (object);

  // on android, the window is always "maximized" as to avoid rounded corners
  gdk_synthesize_surface_state ((GdkSurface *) this, 0, GDK_TOPLEVEL_STATE_MAXIMIZED);
}

static void
gdk_android_toplevel_update_title (GdkAndroidToplevel *this)
{
  if (!this->activity)
    return;

  JNIEnv *env = gdk_android_get_env ();
  (*env)->PushLocalFrame (env, 1);

  jstring jtitle = gdk_android_utf8_to_java (this->title);
  (*env)->CallVoidMethod (env, this->activity, gdk_android_get_java_cache ()->toplevel.post_title, jtitle);

  (*env)->PopLocalFrame (env, NULL);
}

static void
gdk_android_toplevel_set_title (GdkAndroidToplevel *this, const gchar *title)
{
  g_clear_pointer (&this->title, g_free);
  this->title = g_strdup (title);
  gdk_android_toplevel_update_title (this);

  g_object_notify ((GObject *) this, "title");
}

static void
gdk_android_toplevel_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (object);
  GdkSurface *surface = (GdkSurface *) this;

  switch (prop_id)
    {
    case N_PROPERTIES + GDK_TOPLEVEL_PROP_STATE:
      g_value_set_flags (value, surface->state);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_TITLE:
      g_value_set_string (value, this->title);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_STARTUP_ID:
      g_value_set_string (value, "");
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_TRANSIENT_FOR:
      g_value_set_object (value, surface->transient_for);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_MODAL:
      g_value_set_boolean (value, surface->modal_hint);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_ICON_LIST:
      g_value_set_pointer (value, NULL);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_DECORATED:
      g_value_set_boolean (value, FALSE);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_DELETABLE:
      g_value_set_boolean (value, TRUE);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_FULLSCREEN_MODE:
      g_value_set_enum (value, surface->fullscreen_mode);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_SHORTCUTS_INHIBITED:
      g_value_set_boolean (value, FALSE);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_CAPABILITIES:
      g_value_set_flags (value, GDK_TOPLEVEL_CAPABILITIES_MAXIMIZE |
                                GDK_TOPLEVEL_CAPABILITIES_FULLSCREEN |
                                GDK_TOPLEVEL_CAPABILITIES_MINIMIZE);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_GRAVITY:
      g_value_set_enum (value, GDK_GRAVITY_NORTH_EAST);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}
static void
gdk_android_toplevel_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (object);
  GdkSurface *surface = (GdkSurface *) this;

  switch (prop_id)
    {
    case N_PROPERTIES + GDK_TOPLEVEL_PROP_TITLE:
      gdk_android_toplevel_set_title (this, g_value_get_string (value));
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_STARTUP_ID:
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_TRANSIENT_FOR:
      g_clear_object (&surface->transient_for);
      surface->transient_for = g_value_dup_object (value);
      g_object_notify_by_pspec (G_OBJECT (surface), pspec);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_MODAL:
      surface->modal_hint = g_value_get_boolean (value);
      g_object_notify_by_pspec (G_OBJECT (surface), pspec);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_ICON_LIST:
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_DECORATED:
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_DELETABLE:
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_FULLSCREEN_MODE:
      surface->fullscreen_mode = g_value_get_enum (value);
      g_object_notify_by_pspec (G_OBJECT (surface), pspec);
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_SHORTCUTS_INHIBITED:
      break;

    case N_PROPERTIES + GDK_TOPLEVEL_PROP_GRAVITY:
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/*static void
gdk_android_surface_request_layout (GdkSurface *surface)
{
        GdkAndroidSurface *this = GDK_ANDROID_SURFACE(surface);
}*/

static void
gdk_android_toplevel_hide (GdkSurface *surface)
{
  GdkAndroidToplevel *this = (GdkAndroidToplevel *) surface;
  GDK_SURFACE_CLASS (gdk_android_toplevel_parent_class)->hide (surface);

  JNIEnv *env = gdk_android_get_env ();
  (*env)->CallVoidMethod (env, this->activity, gdk_android_get_java_cache ()->a_activity.finish);
}

static gboolean
gdk_android_toplevel_compute_size (GdkSurface *surface)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (surface);
  GdkAndroidSurface *surface_impl = (GdkAndroidSurface *)this;

  GdkToplevelSize size;
  gdk_toplevel_size_init (&size,
                          ceilf (surface_impl->cfg.width / surface_impl->cfg.scale),
                          ceilf (surface_impl->cfg.height / surface_impl->cfg.scale));
  gdk_toplevel_notify_compute_size ((GdkToplevel *) this, &size);

  g_warn_if_fail (size.width > 0);
  g_warn_if_fail (size.height > 0);

  // We currently don't do anything with size right now.

  return GDK_SURFACE_CLASS (gdk_android_toplevel_parent_class)->compute_size (surface);
}

static void
gdk_android_toplevel_destroy (GdkSurface *surface, gboolean foreign_destroy)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (surface);

  JNIEnv *env = gdk_android_get_env ();
  if (!foreign_destroy && this->activity)
    (*env)->CallVoidMethod (env, this->activity, gdk_android_get_java_cache ()->a_activity.finish);

  g_clear_object (&surface->transient_for);
  GDK_SURFACE_CLASS (gdk_android_toplevel_parent_class)->destroy (surface, foreign_destroy);
}

static void
gdk_android_toplevel_on_layout (GdkAndroidSurface *surface)
{
  GdkAndroidToplevel *this = GDK_ANDROID_TOPLEVEL (surface);
  GdkRectangle bounds = {
    .x = surface->cfg.x, .y = surface->cfg.y,
    .width = surface->cfg.width, .height = surface->cfg.height
  };
  gdk_android_monitor_update (this->monitor, &bounds, surface->cfg.scale);
}

static void
gdk_android_toplevel_class_init (GdkAndroidToplevelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GdkSurfaceClass *surface_class = GDK_SURFACE_CLASS (klass);
  GdkAndroidSurfaceClass *surface_impl_class = GDK_ANDROID_SURFACE_CLASS (klass);

  object_class->finalize = gdk_android_toplevel_finalize;
  object_class->constructed = gdk_android_toplevel_constructed;
  object_class->get_property = gdk_android_toplevel_get_property;
  object_class->set_property = gdk_android_toplevel_set_property;

  surface_class->hide = gdk_android_toplevel_hide;
  surface_class->compute_size = gdk_android_toplevel_compute_size;
  surface_class->destroy = gdk_android_toplevel_destroy;

  surface_impl_class->on_layout = gdk_android_toplevel_on_layout;

  gdk_toplevel_install_properties (object_class, N_PROPERTIES);
}

static void
gdk_android_toplevel_init (GdkAndroidToplevel *this)
{
  this->activity_request_counter = 2048; // reserve some lower activity request codes
  this->activity_requests = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)gdk_android_toplevel_activity_request_free);
}

static const GdkRGBA light_bg = { .red = 0xf6 / 255.f, .green = 0xf5 / 255.f, .blue = 0xf4 / 255.f, .alpha = 1.f };
static const GdkRGBA dark_bg = { .red = 0x35 / 255.f, .green = 0x35 / 255.f, .blue = 0x35 / 255.f, .alpha = 1.f };
const GdkRGBA *
gdk_android_toplevel_get_bars_color (GdkAndroidToplevel *this)
{
  GdkDisplay *display = gdk_surface_get_display ((GdkSurface *) this);
  if (gdk_android_display_get_night_mode (GDK_ANDROID_DISPLAY (display)) == GDK_ANDROID_DISPLAY_NIGHT_YES)
    return &dark_bg;
  return &light_bg;
}

static void
gdk_android_toplevel_update_window (GdkAndroidToplevel *this)
{
  if (!this->activity)
    return;
  gboolean is_fullscreen;
  if (!this->layout || !gdk_toplevel_layout_get_fullscreen (this->layout, &is_fullscreen))
    is_fullscreen = FALSE;
  JNIEnv *env = gdk_android_get_env ();
  (*env)->CallVoidMethod (env, this->activity, gdk_android_get_java_cache ()->toplevel.post_window_configuration,
                          gdk_android_utils_color_to_android (gdk_android_toplevel_get_bars_color (this)),
                          is_fullscreen);
}

/**
 * gdk_android_toplevel_get_activity: (skip)
 * @this: (transfer none): the toplevel
 *
 * Get the
 * [Android Activity object](https://developer.android.com/reference/android/app/Activity)
 * backing this toplevel.
 *
 * Returns: (nullable): local reference to Activity object or %NULL if not yet realized
 *
 * Since: 4.18
 */
jobject
gdk_android_toplevel_get_activity (GdkAndroidToplevel *this)
{
  g_return_val_if_fail (GDK_IS_ANDROID_TOPLEVEL (this), FALSE);
  JNIEnv *env = gdk_android_get_env();
  return (*env)->NewLocalRef (env, this->activity);
}

/**
 * gdk_android_toplevel_launch_activity: (skip)
 * @this: (transfer none): the toplevel
 * @intent: [Intent](https://developer.android.com/reference/android/content/Intent) to launch
 * @error: return location for a [struct@GLib.Error]
 *
 * Launch a new activity defined by @intent with @this as parent.
 *
 * Returns: true if successful, false otherwise
 *
 * Since: 4.18
 */
gboolean
gdk_android_toplevel_launch_activity (GdkAndroidToplevel *this,
                                      jobject             intent,
                                      GError            **error)
{
  g_return_val_if_fail (GDK_IS_ANDROID_TOPLEVEL (this), FALSE);
  JNIEnv *env = gdk_android_get_env();
  g_return_val_if_fail ((*env)->IsInstanceOf (env, intent,
                                          gdk_android_get_java_cache ()->a_intent.klass), FALSE);

  (*env)->CallVoidMethod (env, this->activity,
                          gdk_android_get_java_cache ()->a_activity.start_activity,
                          intent);

  return !gdk_android_check_exception (error);
}

static void
gdk_android_toplevel_cancel_launched_activity (GCancellable *cancellable,
                                               GdkAndroidToplevelActivityRequest *data)
{
  JNIEnv *env = gdk_android_get_env();
  (*env)->CallVoidMethod (env, data->parent_activity,
                          gdk_android_get_java_cache ()->a_activity.finish_activity,
                          data->request_code);
}
/**
 * gdk_android_toplevel_launch_activity_for_result_async: (skip)
 * @this: (transfer none): the toplevel
 * @intent: [Intent](https://developer.android.com/reference/android/content/Intent) to launch
 * @cancellable: (nullable): [class@Gio.Cancellable] that stops the launched activity when cancelled
 * @callback: (scope async) (closure user_data): a [fn@Gio.AsyncReadyCallback] to call when the launched activity exits
 * @user_data: the data to pass to @callback
 *
 * Launch a new activity defined by @intent with @this as parent for
 * which you would like a result when it finished.
 */
void
gdk_android_toplevel_launch_activity_for_result_async (GdkAndroidToplevel *this,
                                                       jobject             intent,
                                                       GCancellable       *cancellable,
                                                       GAsyncReadyCallback callback,
                                                       gpointer            user_data)
{
  g_return_if_fail (GDK_IS_ANDROID_TOPLEVEL (this));
  JNIEnv *env = gdk_android_get_env();
  g_return_if_fail ((*env)->IsInstanceOf (env, intent,
                                          gdk_android_get_java_cache ()->a_intent.klass));

  GTask *task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_source_tag (task, gdk_android_toplevel_launch_activity_for_result_async);

  GError *err = NULL;
  if (g_cancellable_set_error_if_cancelled (cancellable, &err))
    {
      g_task_return_error (task, err);
      return;
    }

  guint request_code = this->activity_request_counter++;
  (*env)->CallVoidMethod (env, this->activity,
                          gdk_android_get_java_cache ()->a_activity.start_activity_for_result,
                          intent, request_code);

  if (gdk_android_check_exception (&err))
    {
      g_task_return_error (task, err);
      return;
    }

  GdkAndroidToplevelActivityRequest *data = g_new(GdkAndroidToplevelActivityRequest, 1);
  data->parent_activity = (*env)->NewGlobalRef (env, this->activity);
  data->request_code = request_code;
  data->task = task;
  if (cancellable)
    data->signal_id = g_cancellable_connect (cancellable,
                                             G_CALLBACK (gdk_android_toplevel_cancel_launched_activity),
                                             data, NULL);

  g_hash_table_insert (this->activity_requests, GUINT_TO_POINTER (request_code), data);
}

/**
 * gdk_android_toplevel_launch_activity_for_result_finish: (skip)
 * @this: (transfer none): the toplevel
 * @result: The [iface@Gio.AsyncResult] passed to the callback
 * @response: (out): The result code of the exited activity
 * @data: (out) (optional) (nullable): Local reference to the [Intent](https://developer.android.com/reference/android/content/Intent) object returned by the exited activity
 *
 * Get the result code (and data) returned from an activity launched via
 * [method@Gdk.AndroidToplevel.launch_activity_for_result_async].
 *
 * Returns: true if successful, false otherwise
 */
gboolean
gdk_android_toplevel_launch_activity_for_result_finish (GdkAndroidToplevel *this,
                                                        GAsyncResult       *result,
                                                        jint               *response,
                                                        jobject            *data,
                                                        GError            **error)
{
  GTask *task = (GTask *) result;
  JNIEnv *env = gdk_android_get_env();
  if (data != NULL)
    {
      jobject task_data = g_task_get_task_data (task);
      *data = task_data ? (*env)->NewLocalRef (env, task_data) : NULL;
    }
  gboolean successful = !g_task_had_error (task);
  *response = g_task_propagate_int (task, error);
  return successful;
}
