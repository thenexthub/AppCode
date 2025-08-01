/* -*- Mode: C; tab-width: 8; indent-tabs-mode: Nothing; c-basic-offset: 2 -*- */
/* GTK - The GIMP Toolkit
 * Copyright (C) David Zeuthen <davidz@redhat.com>
 * Copyright (C) 2001 Havoc Pennington
 * Copyright (C) 2005-2007 Vincent Untz
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

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <gio/gio.h>
#include "x11/gdkx.h"
#include <X11/Xatom.h>
#include <glib/gi18n-lib.h>

/* for the kill(2) system call and errno - POSIX.1-2001 and later */
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#if defined(__OpenBSD__)
#include <sys/sysctl.h>
#endif

#include "gtkmountoperationprivate.h"

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

/* ---------------------------------------------------------------------------------------------------- */
/* these functions are based on code from libwnck (LGPLv2) */

static gboolean get_window_list   (GdkDisplay *display,
                                   Display   *xdisplay,
                                   Window     xwindow,
                                   Atom       atom,
                                   Window   **windows,
                                   int       *len);

static char*    get_utf8_property (GdkDisplay *display,
                                   Display   *xdisplay,
                                   Window     xwindow,
                                   Atom       atom);

static gboolean get_cardinal      (GdkDisplay *display,
                                   Display   *xdisplay,
                                   Window     xwindow,
                                   Atom       atom,
                                   int       *val);

static gboolean read_rgb_icon     (GdkDisplay *display,
                                   Display   *xdisplay,
                                   Window     xwindow,
                                   int        ideal_width,
                                   int        ideal_height,
                                   int       *width,
                                   int       *height,
                                   guchar   **pixdata);


static gboolean
get_cardinal (GdkDisplay *display,
              Display *xdisplay,
              Window   xwindow,
              Atom     atom,
              int     *val)
{
  Atom type;
  int format;
  gulong nitems;
  gulong bytes_after;
  gulong *num;
  int err, result;

  *val = 0;

  gdk_x11_display_error_trap_push (display);
  type = None;
  result = XGetWindowProperty (xdisplay,
                               xwindow,
                               atom,
                               0, G_MAXLONG,
                               False, XA_CARDINAL, &type, &format, &nitems,
                               &bytes_after, (void*)&num);
  XSync (xdisplay, False);
  err = gdk_x11_display_error_trap_pop (display);

  if (err != Success ||
      result != Success)
    return FALSE;

  if (type != XA_CARDINAL)
    {
      XFree (num);
      return FALSE;
    }

  *val = *num;

  XFree (num);

  return TRUE;
}

static char*
get_utf8_property (GdkDisplay *display,
                   Display *xdisplay,
                   Window   xwindow,
                   Atom     atom)
{
  Atom type;
  int format;
  gulong nitems;
  gulong bytes_after;
  char *val;
  int err, result;
  char *retval;
  Atom utf8_string;

  utf8_string = gdk_x11_get_xatom_by_name_for_display (display, "UTF8_STRING");

  gdk_x11_display_error_trap_push (display);
  type = None;
  val = NULL;
  result = XGetWindowProperty (xdisplay,
                               xwindow,
                               atom,
                               0, G_MAXLONG,
                               False, utf8_string,
                               &type, &format, &nitems,
                               &bytes_after, (guchar **)&val);
  XSync (xdisplay, False);
  err = gdk_x11_display_error_trap_pop (display);

  if (err != Success ||
      result != Success)
    return NULL;

  if (type != utf8_string ||
      format != 8 ||
      nitems == 0)
    {
      if (val)
        XFree (val);
      return NULL;
    }

  if (!g_utf8_validate (val, nitems, NULL))
    {
      g_warning ("Property %s contained invalid UTF-8",
                 gdk_x11_get_xatom_name_for_display (display, atom));
      XFree (val);
      return NULL;
    }

  retval = g_strndup (val, nitems);

  XFree (val);

  return retval;
}

static gboolean
find_largest_sizes (gulong *data,
                    gulong  nitems,
                    int    *width,
                    int    *height)
{
  *width = 0;
  *height = 0;

  while (nitems > 0)
    {
      int w, h;

      if (nitems < 3)
        return FALSE; /* no space for w, h */

      w = data[0];
      h = data[1];

      if (nitems < ((w * h) + 2))
        return FALSE; /* not enough data */

      *width = MAX (w, *width);
      *height = MAX (h, *height);

      data += (w * h) + 2;
      nitems -= (w * h) + 2;
    }

  return TRUE;
}

static gboolean
find_best_size (gulong  *data,
                gulong   nitems,
                int      ideal_width,
                int      ideal_height,
                int     *width,
                int     *height,
                gulong **start)
{
  int best_w;
  int best_h;
  gulong *best_start;
  int max_width, max_height;

  *width = 0;
  *height = 0;
  *start = NULL;

  if (!find_largest_sizes (data, nitems, &max_width, &max_height))
    return FALSE;

  if (ideal_width < 0)
    ideal_width = max_width;
  if (ideal_height < 0)
    ideal_height = max_height;

  best_w = 0;
  best_h = 0;
  best_start = NULL;

  while (nitems > 0)
    {
      int w, h;
      gboolean replace;

      replace = FALSE;

      if (nitems < 3)
        return FALSE; /* no space for w, h */

      w = data[0];
      h = data[1];

      if (nitems < ((w * h) + 2))
        break; /* not enough data */

      if (best_start == NULL)
        {
          replace = TRUE;
        }
      else
        {
          /* work with averages */
          const int ideal_size = (ideal_width + ideal_height) / 2;
          int best_size = (best_w + best_h) / 2;
          int this_size = (w + h) / 2;

          /* larger than desired is always better than smaller */
          if (best_size < ideal_size &&
              this_size >= ideal_size)
            replace = TRUE;
          /* if we have too small, pick anything bigger */
          else if (best_size < ideal_size &&
                   this_size > best_size)
            replace = TRUE;
          /* if we have too large, pick anything smaller
           * but still >= the ideal
           */
          else if (best_size > ideal_size &&
                   this_size >= ideal_size &&
                   this_size < best_size)
            replace = TRUE;
        }

      if (replace)
        {
          best_start = data + 2;
          best_w = w;
          best_h = h;
        }

      data += (w * h) + 2;
      nitems -= (w * h) + 2;
    }

  if (best_start)
    {
      *start = best_start;
      *width = best_w;
      *height = best_h;
      return TRUE;
    }
  else
    return FALSE;
}

static void
argbdata_to_pixdata (gulong  *argb_data,
                     int      len,
                     guchar **pixdata)
{
  guchar *p;
  int i;

  *pixdata = g_new (guchar, len * 4);
  p = *pixdata;

  /* One could speed this up a lot. */
  i = 0;
  while (i < len)
    {
      guint argb;
      guint rgba;

      argb = argb_data[i];
      rgba = (argb << 8) | (argb >> 24);

      *p = rgba >> 24;
      ++p;
      *p = (rgba >> 16) & 0xff;
      ++p;
      *p = (rgba >> 8) & 0xff;
      ++p;
      *p = rgba & 0xff;
      ++p;

      ++i;
    }
}

static gboolean
read_rgb_icon (GdkDisplay *display,
               Display   *xdisplay,
               Window     xwindow,
               int        ideal_width,
               int        ideal_height,
               int       *width,
               int       *height,
               guchar   **pixdata)
{
  Atom type;
  int format;
  gulong nitems;
  gulong bytes_after;
  int result, err;
  gulong *data;
  gulong *best;
  int w, h;

  gdk_x11_display_error_trap_push (display);
  type = None;
  data = NULL;
  result = XGetWindowProperty (xdisplay,
                               xwindow,
                               gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_ICON"),
                               0, G_MAXLONG,
                               False, XA_CARDINAL, &type, &format, &nitems,
                               &bytes_after, (void*)&data);

  XSync (xdisplay, False);
  err = gdk_x11_display_error_trap_pop (display);

  if (err != Success ||
      result != Success)
    return FALSE;

  if (type != XA_CARDINAL)
    {
      XFree (data);
      return FALSE;
    }

  if (!find_best_size (data, nitems,
                       ideal_width, ideal_height,
                       &w, &h, &best))
    {
      XFree (data);
      return FALSE;
    }

  *width = w;
  *height = h;

  argbdata_to_pixdata (best, w * h, pixdata);

  XFree (data);

  return TRUE;
}

static void
free_pixels (guchar *pixels, gpointer data)
{
  g_free (pixels);
}

static GdkTexture *
scaled_from_pixdata (guchar *pixdata,
                     int     w,
                     int     h,
                     int     new_w,
                     int     new_h)
{
  GdkPixbuf *src;
  GdkPixbuf *dest;
  GdkTexture *ret;

  src = gdk_pixbuf_new_from_data (pixdata,
                                  GDK_COLORSPACE_RGB,
                                  TRUE,
                                  8,
                                  w, h, w * 4,
                                  free_pixels,
                                  NULL);

  if (src == NULL)
    return NULL;

  if (w != h)
    {
      GdkPixbuf *tmp;
      int size;

      size = MAX (w, h);

      tmp = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, size, size);

      if (tmp != NULL)
        {
          gdk_pixbuf_fill (tmp, 0);
          gdk_pixbuf_copy_area (src, 0, 0, w, h,
                                tmp,
                                (size - w) / 2, (size - h) / 2);

          g_object_unref (src);
          src = tmp;
        }
    }

  if (w != new_w || h != new_h)
    {
      dest = gdk_pixbuf_scale_simple (src, new_w, new_h, GDK_INTERP_BILINEAR);

      g_object_unref (G_OBJECT (src));
    }
  else
    {
      dest = src;
    }

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  ret = gdk_texture_new_for_pixbuf (dest);
G_GNUC_END_IGNORE_DEPRECATIONS

  g_object_unref (dest);

  return ret;
}

static gboolean
get_window_list (GdkDisplay *display,
                 Display  *xdisplay,
                 Window    xwindow,
                 Atom      atom,
                 Window  **windows,
                 int      *len)
{
  Atom type;
  int format;
  gulong nitems;
  gulong bytes_after;
  Window *data;
  int err, result;

  *windows = NULL;
  *len = 0;

  gdk_x11_display_error_trap_push (display);
  type = None;
  result = XGetWindowProperty (xdisplay,
                               xwindow,
                               atom,
                               0, G_MAXLONG,
                               False, XA_WINDOW, &type, &format, &nitems,
                               &bytes_after, (void*)&data);
  XSync (xdisplay, False);
  err = gdk_x11_display_error_trap_pop (display);

  if (err != Success ||
      result != Success)
    return FALSE;

  if (type != XA_WINDOW)
    {
      XFree (data);
      return FALSE;
    }

  *windows = g_new (Window, nitems);
  memcpy (*windows, data, sizeof (Window) * nitems);
  *len = nitems;

  XFree (data);

  return TRUE;
}


/* ---------------------------------------------------------------------------------------------------- */

struct _GtkMountOperationLookupContext
{
  /* Hash from pid (int) -> XID (int)
   *
   * Note that XIDs are at most 27 bits - however, also note that sizeof(XID) == 8 on
   * x86_64 - that's just xlib brokenness. So it's safe to stuff the XID into a pointer.
   */
  GHashTable *pid_to_window;
  GdkDisplay *display;
};

GtkMountOperationLookupContext *
_gtk_mount_operation_lookup_context_get (GdkDisplay *display)
{
  GtkMountOperationLookupContext *context;
  Window *mapping;
  int mapping_length;
  int n;

  context = g_new0 (GtkMountOperationLookupContext, 1);

  context->pid_to_window = g_hash_table_new (g_direct_hash, g_direct_equal);
  context->display = display;

  if (GDK_IS_X11_DISPLAY (display))
    {
      mapping = NULL;
      mapping_length = 0;
      get_window_list (context->display,
                       gdk_x11_display_get_xdisplay (context->display),
                       gdk_x11_display_get_xrootwindow (context->display),
                       gdk_x11_get_xatom_by_name_for_display (context->display,
                                                              "_NET_CLIENT_LIST"),
                       &mapping,
                       &mapping_length);
      for (n = 0; n < mapping_length; n++)
        {
          int pid;

          if (!get_cardinal (context->display,
                             GDK_DISPLAY_XDISPLAY (context->display),
                             mapping[n],
                             gdk_x11_get_xatom_by_name_for_display (context->display,
                                                                    "_NET_WM_PID"),
                             &pid))
            continue;

          g_hash_table_insert (context->pid_to_window,
                               GINT_TO_POINTER (pid),
                               GINT_TO_POINTER ((int) mapping[n]));
        }
      g_free (mapping);
    }

  return context;
}

void
_gtk_mount_operation_lookup_context_free (GtkMountOperationLookupContext *context)
{
  g_hash_table_unref (context->pid_to_window);
  g_free (context);
}

/* ---------------------------------------------------------------------------------------------------- */

#ifdef __linux__

static GPid
pid_get_parent (GPid pid)
{
  GPid ppid;
  char **tokens;
  char *stat_filename;
  char *stat_contents;
  gsize stat_len;

  ppid = 0;
  tokens = NULL;
  stat_contents = NULL;
  stat_filename = NULL;

  /* fail if trying to get the parent of the init process (no such thing) */
  if (pid == 1)
      goto out;

  stat_filename = g_strdup_printf ("/proc/%d/status", pid);
  if (g_file_get_contents (stat_filename,
                           &stat_contents,
                           &stat_len,
                           NULL))
    {
      guint n;

      tokens = g_strsplit (stat_contents, "\n", 0);

      for (n = 0; tokens[n] != NULL; n++)
        {
          if (g_str_has_prefix (tokens[n], "PPid:"))
            {
              char *endp;

              endp = NULL;
              ppid = strtoll (tokens[n] + sizeof "PPid:" - 1, &endp, 10);
              if (endp == NULL || *endp != '\0')
                {
                  g_warning ("Error parsing contents of `%s'. Parent pid is malformed.",
                             stat_filename);
                  ppid = 0;
                  goto out;
                }

              break;
            }
        }
    }

 out:
  g_strfreev (tokens);
  g_free (stat_contents);
  g_free (stat_filename);

  return ppid;
}

static char *
pid_get_env (GPid         pid,
             const char *key)
{
  char *ret;
  char *env_filename;
  char *env;
  gsize env_len;
  gsize key_len;
  char *end;

  ret = NULL;

  key_len = strlen (key);

  env_filename = g_strdup_printf ("/proc/%d/environ", pid);
  if (g_file_get_contents (env_filename,
                           &env,
                           &env_len,
                           NULL))
    {
      guint n;

      /* /proc/<pid>/environ in Linux is split at '\0' points, g_strsplit() can't handle that... */
      n = 0;
      while (TRUE)
        {
          if (n >= env_len || env[n] == '\0')
            break;

          if (g_str_has_prefix (env + n, key) && (*(env + n + key_len) == '='))
            {
              ret = g_strdup (env + n + key_len + 1);

              /* skip invalid UTF-8 */
              if (!g_utf8_validate (ret, -1, (const char **) &end))
                *end = '\0';
              break;
            }

          for (; n < env_len && env[n] != '\0'; n++)
            ;
          n++;
        }
      g_free (env);
    }
  g_free (env_filename);

  return ret;
}

static char *
pid_get_command_line (GPid pid)
{
  char *cmdline_filename;
  char *cmdline_contents;
  gsize cmdline_len;
  guint n;
  char *end;

  cmdline_contents = NULL;

  cmdline_filename = g_strdup_printf ("/proc/%d/cmdline", pid);
  if (!g_file_get_contents (cmdline_filename,
                            &cmdline_contents,
                            &cmdline_len,
                            NULL))
    goto out;

  /* /proc/<pid>/cmdline separates args by NUL-bytes - replace with spaces */
  for (n = 0; n < cmdline_len - 1; n++)
    {
      if (cmdline_contents[n] == '\0')
        cmdline_contents[n] = ' ';
    }

  /* skip invalid UTF-8 */
  if (!g_utf8_validate (cmdline_contents, -1, (const char **) &end))
      *end = '\0';

 out:
  g_free (cmdline_filename);

  return cmdline_contents;
}

/* ---------------------------------------------------------------------------------------------------- */

#elif defined(__OpenBSD__)

/* ---------------------------------------------------------------------------------------------------- */

static GPid
pid_get_parent (GPid pid)
{
  struct kinfo_proc *kp = NULL;
  size_t len;
  GPid ppid = 0;

  /* fail if trying to get the parent of the init process (no such thing) */
  if (pid == 1)
      goto out;

  int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid,
                sizeof(struct kinfo_proc), 0 };

  if (sysctl (mib, G_N_ELEMENTS (mib), NULL, &len, NULL, 0) == -1)
      goto out;

  mib[5] = (len / sizeof(struct kinfo_proc));

  kp = g_malloc0 (len);

  if (sysctl (mib, G_N_ELEMENTS (mib), kp, &len, NULL, 0) < 0)
      goto out;

  ppid = kp->p_ppid;

out:
  if (kp)
      g_free (kp);
  return ppid;
}

static char *
pid_get_env (GPid pid, const char *key)
{
  size_t len;
  char **strs;
  char *ret = NULL;
  char *end;
  int key_len;
  int i;

  int mib[] = { CTL_KERN, KERN_PROC_ARGS, pid, KERN_PROC_ENV };

  if (sysctl (mib, G_N_ELEMENTS (mib), NULL, &len, NULL, 0) == -1)
    return ret;

  strs = g_malloc0 (len);

  key_len = strlen (key);

  if (sysctl (mib, G_N_ELEMENTS (mib), strs, &len, NULL, 0) != -1)
    {
      for (i = 0; strs[i] != NULL; i++)
	{
	  if (g_str_has_prefix (strs[i], key) && (*(strs[i] + key_len) == '='))
	    {
	      ret = g_strdup (strs[i] + key_len + 1);

	      /* skip invalid UTF-8 */
	      if (!g_utf8_validate (ret, -1, (const char **) &end))
		*end = '\0';
	      break;
	    }
	}
    }

  g_free (strs);
  return ret;
}

static char *
pid_get_command_line (GPid pid)
{
  size_t len;
  char **strs;
  char *ret, *end;

  int mib[] = { CTL_KERN, KERN_PROC_ARGS, pid, KERN_PROC_ARGV };

  if (sysctl (mib, G_N_ELEMENTS (mib), NULL, &len, NULL, 0) == -1)
    return NULL;

  strs = g_malloc0 (len);

  if (sysctl (mib, G_N_ELEMENTS (mib), strs, &len, NULL, 0) == -1) {
    g_free (strs);
    return NULL;
  }

  ret = g_strjoinv (" ", strs);
  /* skip invalid UTF-8 */
  if (!g_utf8_validate (ret, -1, (const char **) &end))
    *end = '\0';

  g_free (strs);
  return ret;
}

#else

/* TODO: please implement for your OS - must return valid UTF-8 */

static GPid
pid_get_parent (GPid pid)
{
  return 0;
}

static char *
pid_get_env (GPid         pid,
             const char *key)
{
  return NULL;
}

static char *
pid_get_command_line (GPid pid)
{
  return NULL;
}

#endif

/* ---------------------------------------------------------------------------------------------------- */

static char *
get_name_for_window_with_pid (GtkMountOperationLookupContext *context,
                              GPid                            pid)
{
  Window window;
  Window windowid_window;
  char *ret;

  ret = NULL;

  window = GPOINTER_TO_INT (g_hash_table_lookup (context->pid_to_window, GINT_TO_POINTER (pid)));
  if (window == None)
    {
      char *windowid_value;

      /* check for $WINDOWID (set by terminals) and see if we can get the title that way */
      windowid_value = pid_get_env (pid, "WINDOWID");
      if (windowid_value != NULL)
        {
          char *endp;

          endp = NULL;
          windowid_window = (Window) g_ascii_strtoll (windowid_value, &endp, 10);
          if (endp != NULL && *endp == '\0')
            {
              window = windowid_window;
            }
          g_free (windowid_value);
        }

      /* otherwise, check for parents */
      if (window == None)
        {
          do
            {
              pid = pid_get_parent (pid);
              if (pid == 0)
                break;

              window = GPOINTER_TO_INT (g_hash_table_lookup (context->pid_to_window, GINT_TO_POINTER (pid)));
              if (window != None)
                break;
            }
          while (TRUE);
        }
    }

  if (window != None)
    {
      ret = get_utf8_property (context->display,
                               GDK_DISPLAY_XDISPLAY (context->display),
                               window,
                               gdk_x11_get_xatom_by_name_for_display (context->display,
                                                                      "_NET_WM_NAME"));
      if (ret == NULL)
        ret = get_utf8_property (context->display,
                                 GDK_DISPLAY_XDISPLAY (context->display),
                                 window, gdk_x11_get_xatom_by_name_for_display (context->display,
                                                                                "_NET_WM_ICON_NAME"));
    }

  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

static GdkTexture *
get_texture_for_window_with_pid (GtkMountOperationLookupContext *context,
                                 GPid                            pid,
                                 int                             size_pixels)
{
  Window window;
  GdkTexture *ret;

  ret = NULL;

  window = GPOINTER_TO_INT (g_hash_table_lookup (context->pid_to_window, GINT_TO_POINTER (pid)));
  if (window == None)
    {
      /* otherwise, check for parents */
      do
        {
          pid = pid_get_parent (pid);
          if (pid == 0)
            break;

          window = GPOINTER_TO_INT (g_hash_table_lookup (context->pid_to_window, GINT_TO_POINTER (pid)));
          if (window != None)
            break;
        }
      while (TRUE);
    }

  if (window != None)
    {
      int     width;
      int     height;
      guchar *pixdata;

      if (read_rgb_icon (context->display,
                         GDK_DISPLAY_XDISPLAY (context->display),
                         window,
                         size_pixels, size_pixels,
                         &width, &height,
                         &pixdata))
        {
          /* steals pixdata */

          ret = scaled_from_pixdata (pixdata,
                                     width, height,
                                     size_pixels, size_pixels);
        }
    }

  return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

static const char *well_known_commands[] =
{
  /* translators: this string is a name for the 'less' command */
  "less", N_("Terminal Pager"),
  "top", N_("Top Command"),
  "bash", N_("Bourne Again Shell"),
  "sh", N_("Bourne Shell"),
  "zsh", N_("Z Shell"),
  NULL,
};

gboolean
_gtk_mount_operation_lookup_info (GtkMountOperationLookupContext *context,
                                  GPid                            pid,
                                  int                             size_pixels,
                                  char                          **out_name,
                                  char                          **out_command_line,
                                  GdkTexture                    **out_texture)
{
  g_return_val_if_fail (out_name != NULL && *out_name == NULL, FALSE);
  g_return_val_if_fail (out_command_line != NULL && *out_command_line == NULL, FALSE);
  g_return_val_if_fail (out_texture != NULL && *out_texture == NULL, FALSE);

  if (!GDK_IS_X11_DISPLAY (context->display))
    return FALSE;

  /* We perform two different lookups for name and icon size.. this is
   * because we want the name from the window with WINDOWID and this
   * normally does not give you an icon
   *
   * (the canonical example is a tab in gnome-terminal - the shell/command running
   *  in the shell will have WINDOWID set - but this window won't have an icon - so
   *  we want to continue up until the gnome-terminal window so we can get that icon)
   */

  *out_command_line = pid_get_command_line (pid);

  *out_name = get_name_for_window_with_pid (context, pid);

  *out_texture = get_texture_for_window_with_pid (context, pid, size_pixels);

  /* if we didn't manage to find the name via X, fall back to the basename
   * of the first element of the command line and, for maximum geek-comfort,
   * map a few well-known commands to proper translated names
   */
  if (*out_name == NULL && *out_command_line != NULL &&
      strlen (*out_command_line) > 0 && (*out_command_line)[0] != ' ')
    {
      guint n;
      char *s;
      char *p;

      /* find the first character after the first argument */
      s = strchr (*out_command_line, ' ');
      if (s == NULL)
        s = *out_command_line + strlen (*out_command_line);

      for (p = s; p > *out_command_line; p--)
        {
          if (*p == '/')
            {
              p++;
              break;
            }
        }

      *out_name = g_strndup (p, s - p);

      for (n = 0; well_known_commands[n] != NULL; n += 2)
        {
          /* sometimes the command is prefixed with a -, e.g. '-bash' instead
           * of 'bash' - handle that as well
           */
          if ((strcmp (well_known_commands[n], *out_name) == 0) ||
              ((*out_name)[0] == '-' && (strcmp (well_known_commands[n], (*out_name) + 1) == 0)))
            {
              g_free (*out_name);
              *out_name = g_strdup (_(well_known_commands[n+1]));
              break;
            }
        }
    }

  return TRUE;
}

gboolean
_gtk_mount_operation_kill_process (GPid      pid,
                                   GError  **error)
{
  gboolean ret;

  ret = TRUE;

  if (kill ((pid_t) pid, SIGTERM) != 0)
    {
      int errsv = errno;

      /* TODO: On EPERM, we could use a setuid helper using polkit (very easy to implement
       *       via pkexec(1)) to allow the user to e.g. authenticate to gain the authorization
       *       to kill the process. But that's not how things currently work.
       */

      ret = FALSE;
      g_set_error (error,
                   G_IO_ERROR,
                   g_io_error_from_errno (errsv),
                   _("Cannot end process with PID %d: %s"),
                   pid,
                   g_strerror (errsv));
    }

  return ret;
}
