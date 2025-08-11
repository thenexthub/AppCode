/*
 * Copyright © 2020 Benjamin Otte
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
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include "config.h"

#include "gtkdropprivate.h"

typedef struct _GtkDrop GtkDrop;

struct _GtkDrop
{
  /* TRUE if we are waiting for a gdk_drop_status() call */
  gboolean waiting;
  /* TRUE if begin_event() has been called but end_event() hasn't yet - purely for debugging */
  gboolean active;
};

static void
gtk_drop_free (gpointer data)
{
  GtkDrop *this = data;

  g_free (this);
}

static GtkDrop *
gtk_drop_lookup (GdkDrop *drop)
{
  static GQuark drop_quark = 0;
  GtkDrop *result;

  if (G_UNLIKELY (drop_quark == 0))
    drop_quark = g_quark_from_string ("-gtk-drop-data");

  result = g_object_get_qdata (G_OBJECT (drop), drop_quark);
  if (result == NULL)
    {
      result = g_new0 (GtkDrop, 1);
      g_object_set_qdata_full (G_OBJECT (drop), drop_quark, result, gtk_drop_free);
    }

  return result;
}

void
gtk_drop_begin_event (GdkDrop      *drop,
                      GdkEventType  event_type)
{
  GtkDrop *this;

  this = gtk_drop_lookup (drop);

  g_assert (this->waiting == FALSE);
  g_assert (this->active == FALSE);

  this->active = TRUE;
  if (event_type == GDK_DRAG_ENTER ||
      event_type == GDK_DRAG_MOTION)
    this->waiting = TRUE;
}

void
gtk_drop_end_event (GdkDrop *drop)
{
  GtkDrop *this;

  this = gtk_drop_lookup (drop);

  g_assert (this->active == TRUE);

  if (this->waiting)
    {
      gdk_drop_status (drop, 0, 0);
      this->waiting = FALSE;
    }
  this->active = FALSE;
}

gboolean
gtk_drop_status (GdkDrop       *drop,
                 GdkDragAction  actions,
                 GdkDragAction  preferred_action)
{
  GtkDrop *this;

  this = gtk_drop_lookup (drop);

  g_assert (this->active == TRUE);

  if (!this->waiting)
    return FALSE;

  gdk_drop_status (drop, actions, preferred_action);
  this->waiting = FALSE;
  return TRUE;
}
                     
