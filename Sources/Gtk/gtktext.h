/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * Copyright (C) 2004-2006 Christian Hammond
 * Copyright (C) 2008 Cody Russell
 * Copyright (C) 2008 Red Hat, Inc.
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

#pragma once


#if !defined (__GTK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gtk/gtk.h> can be included directly."
#endif

#include <gtk/gtkeditable.h>
#include <gtk/gtkentrybuffer.h>


G_BEGIN_DECLS

#define GTK_TYPE_TEXT                  (gtk_text_get_type ())
#define GTK_TEXT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_TEXT, GtkText))
#define GTK_IS_TEXT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_TEXT))

typedef struct _GtkText              GtkText;

struct _GtkText
{
  /*< private >*/
  GtkWidget  parent_instance;
};

GDK_AVAILABLE_IN_ALL
GType           gtk_text_get_type                       (void) G_GNUC_CONST;

GDK_AVAILABLE_IN_ALL
GtkWidget *     gtk_text_new                            (void);
GDK_AVAILABLE_IN_ALL
GtkWidget *     gtk_text_new_with_buffer                (GtkEntryBuffer  *buffer);

GDK_AVAILABLE_IN_ALL
GtkEntryBuffer *gtk_text_get_buffer                     (GtkText         *this);
GDK_AVAILABLE_IN_ALL
void            gtk_text_set_buffer                     (GtkText         *this,
                                                         GtkEntryBuffer  *buffer);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_visibility                 (GtkText         *this,
                                                         gboolean         visible);
GDK_AVAILABLE_IN_ALL
gboolean        gtk_text_get_visibility                 (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_invisible_char             (GtkText         *this,
                                                         gunichar         ch);
GDK_AVAILABLE_IN_ALL
gunichar        gtk_text_get_invisible_char             (GtkText         *this);
GDK_AVAILABLE_IN_ALL
void            gtk_text_unset_invisible_char           (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_overwrite_mode             (GtkText         *this,
                                                         gboolean         overwrite);
GDK_AVAILABLE_IN_ALL
gboolean        gtk_text_get_overwrite_mode             (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_max_length                 (GtkText         *this,
                                                         int              length);
GDK_AVAILABLE_IN_ALL
int             gtk_text_get_max_length                 (GtkText         *this);
GDK_AVAILABLE_IN_ALL
guint16         gtk_text_get_text_length                (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_activates_default          (GtkText         *this,
                                                         gboolean         activates);
GDK_AVAILABLE_IN_ALL
gboolean        gtk_text_get_activates_default          (GtkText         *this);

GDK_AVAILABLE_IN_ALL
const char *    gtk_text_get_placeholder_text           (GtkText         *this);
GDK_AVAILABLE_IN_ALL
void            gtk_text_set_placeholder_text           (GtkText         *this,
                                                         const char      *text);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_input_purpose              (GtkText         *this,
                                                         GtkInputPurpose  purpose);
GDK_AVAILABLE_IN_ALL
GtkInputPurpose gtk_text_get_input_purpose              (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_input_hints                (GtkText         *this,
                                                         GtkInputHints    hints);
GDK_AVAILABLE_IN_ALL
GtkInputHints   gtk_text_get_input_hints                (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_attributes                 (GtkText         *this,
                                                         PangoAttrList   *attrs);
GDK_AVAILABLE_IN_ALL
PangoAttrList * gtk_text_get_attributes                 (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_tabs                       (GtkText         *this,
                                                         PangoTabArray   *tabs);

GDK_AVAILABLE_IN_ALL
PangoTabArray * gtk_text_get_tabs                       (GtkText         *this);

GDK_AVAILABLE_IN_ALL
gboolean        gtk_text_grab_focus_without_selecting   (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_extra_menu                 (GtkText         *this,
                                                         GMenuModel      *model);
GDK_AVAILABLE_IN_ALL
GMenuModel *    gtk_text_get_extra_menu                 (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_enable_emoji_completion    (GtkText         *this,
                                                         gboolean         enable_emoji_completion);
GDK_AVAILABLE_IN_ALL
gboolean        gtk_text_get_enable_emoji_completion    (GtkText         *this);


GDK_AVAILABLE_IN_ALL
void            gtk_text_set_propagate_text_width       (GtkText         *this,
                                                         gboolean         propagate_text_width);
GDK_AVAILABLE_IN_ALL
gboolean        gtk_text_get_propagate_text_width       (GtkText         *this);

GDK_AVAILABLE_IN_ALL
void            gtk_text_set_truncate_multiline         (GtkText         *this,
                                                         gboolean         truncate_multiline);
GDK_AVAILABLE_IN_ALL
gboolean        gtk_text_get_truncate_multiline         (GtkText         *this);

GDK_AVAILABLE_IN_4_4
void            gtk_text_compute_cursor_extents         (GtkText         *this,
                                                         gsize            position,
                                                         graphene_rect_t *strong,
                                                         graphene_rect_t *weak);


G_END_DECLS

