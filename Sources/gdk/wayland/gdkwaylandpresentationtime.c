#include "config.h"

#include <gdk/gdkframeclockprivate.h>

#include "gdkwaylandpresentationtime-private.h"

typedef struct _GdkWaylandPresentationFrame
{
  GdkWaylandPresentationTime *this;
  struct wp_presentation_feedback *feedback;
  GdkFrameClock *frame_clock;
  gint64 frame_number;
} GdkWaylandPresentationFrame;

static void
gdk_wayland_presentation_frame_free (GdkWaylandPresentationFrame *frame)
{
  g_clear_pointer (&frame->feedback, wp_presentation_feedback_destroy);
  g_clear_object (&frame->frame_clock);
  frame->this = NULL;
  g_free (frame);
}

struct _GdkWaylandPresentationTime
{
  GdkWaylandDisplay *display;
  GPtrArray *frames;
};

GdkWaylandPresentationTime *
gdk_wayland_presentation_time_new (GdkWaylandDisplay *display)
{
  GdkWaylandPresentationTime *this;

  g_return_val_if_fail (GDK_IS_WAYLAND_DISPLAY (display), NULL);

  this = g_new0 (GdkWaylandPresentationTime, 1);
  this->display = g_object_ref (display);
  this->frames = g_ptr_array_new_with_free_func ((GDestroyNotify)gdk_wayland_presentation_frame_free);

  return this;
}

void
gdk_wayland_presentation_time_free (GdkWaylandPresentationTime *this)
{
  g_clear_pointer (&this->frames, g_ptr_array_unref);
  g_clear_object (&this->display);
  g_free (this);
}

static gint64
time_from_wayland (uint32_t tv_sec_hi,
                   uint32_t tv_sec_lo,
                   uint32_t tv_nsec)
{
  guint64 t = tv_sec_hi;
  t <<= 32;
  t |= tv_sec_lo;
  t *= G_USEC_PER_SEC;
  t += tv_nsec / 1000L;
  return (gint64)t;
}

static void
gdk_wayland_presentation_feedback_sync_output (void                            *data,
                                               struct wp_presentation_feedback *feedback,
                                               struct wl_output                *output)
{
}

static void
gdk_wayland_presentation_feedback_presented (void                            *data,
                                             struct wp_presentation_feedback *feedback,
                                             uint32_t                         tv_sec_hi,
                                             uint32_t                         tv_sec_lo,
                                             uint32_t                         tv_nsec,
                                             uint32_t                         refresh,
                                             uint32_t                         seq_hi,
                                             uint32_t                         seq_lo,
                                             uint32_t                         flags)
{
  GdkWaylandPresentationFrame *frame = data;
  GdkWaylandPresentationTime *this;
  GdkFrameTimings *timings;
  guint pos;

  g_assert (frame != NULL);
  g_assert (frame->this != NULL);

  this = frame->this;

  if ((timings = gdk_frame_clock_get_timings (frame->frame_clock, frame->frame_number)))
    {
      timings->presentation_time = time_from_wayland (tv_sec_hi, tv_sec_lo, tv_nsec);
      timings->complete = TRUE;
    }

  if (g_ptr_array_find (this->frames, frame, &pos))
    g_ptr_array_remove_index_fast (this->frames, pos);
}

static void
gdk_wayland_presentation_feedback_discarded (void                            *data,
                                             struct wp_presentation_feedback *feedback)
{
  GdkWaylandPresentationFrame *frame = data;
  guint pos;

  g_assert (frame != NULL);
  g_assert (frame->this != NULL);

  if (g_ptr_array_find (frame->this->frames, frame, &pos))
    g_ptr_array_remove_index_fast (frame->this->frames, pos);
}

static const struct wp_presentation_feedback_listener gdk_wayland_presentation_feedback_listener = {
  gdk_wayland_presentation_feedback_sync_output,
  gdk_wayland_presentation_feedback_presented,
  gdk_wayland_presentation_feedback_discarded,
};

void
gdk_wayland_presentation_time_track (GdkWaylandPresentationTime *this,
                                     GdkFrameClock              *frame_clock,
                                     struct wl_surface          *surface,
                                     gint64                      frame_number)
{
  struct wp_presentation_feedback *feedback;
  GdkWaylandPresentationFrame *frame;

  g_return_if_fail (this != NULL);
  g_return_if_fail (surface != NULL);

  if (this->display->presentation == NULL)
    return;

  if (!(feedback = wp_presentation_feedback (this->display->presentation, surface)))
    return;

  frame = g_new0 (GdkWaylandPresentationFrame, 1);
  frame->this = this;
  frame->frame_clock = g_object_ref (frame_clock);
  frame->frame_number = frame_number;
  frame->feedback = g_steal_pointer (&feedback);

  g_ptr_array_add (this->frames, frame);

  wp_presentation_feedback_add_listener (frame->feedback,
                                         &gdk_wayland_presentation_feedback_listener,
                                         frame);
}
