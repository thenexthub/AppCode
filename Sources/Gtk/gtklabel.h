/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
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

#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#define GTK_TYPE_LABEL		  (gtk_label_get_type ())
#define GTK_LABEL(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_LABEL, GtkLabel))
#define GTK_IS_LABEL(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_LABEL))

typedef struct _GtkLabel GtkLabel;

GDK_AVAILABLE_IN_ALL
GType                 gtk_label_get_type          (void) G_GNUC_CONST;
GDK_AVAILABLE_IN_ALL
GtkWidget*            gtk_label_new               (const char    *str);
GDK_AVAILABLE_IN_ALL
GtkWidget*            gtk_label_new_with_mnemonic (const char    *str);
GDK_AVAILABLE_IN_ALL
void                  gtk_label_set_text          (GtkLabel      *this,
						   const char    *str);
GDK_AVAILABLE_IN_ALL
const char *          gtk_label_get_text          (GtkLabel      *this);
GDK_AVAILABLE_IN_ALL
void                  gtk_label_set_attributes    (GtkLabel      *this,
						   PangoAttrList *attrs);
GDK_AVAILABLE_IN_ALL
PangoAttrList        *gtk_label_get_attributes    (GtkLabel      *this);
GDK_AVAILABLE_IN_ALL
void                  gtk_label_set_label         (GtkLabel      *this,
						   const char    *str);
GDK_AVAILABLE_IN_ALL
const char *         gtk_label_get_label         (GtkLabel      *this);
GDK_AVAILABLE_IN_ALL
void                  gtk_label_set_markup        (GtkLabel      *this,
						   const char    *str);
GDK_AVAILABLE_IN_ALL
void                  gtk_label_set_use_markup    (GtkLabel      *this,
						   gboolean       setting);
GDK_AVAILABLE_IN_ALL
gboolean              gtk_label_get_use_markup    (GtkLabel      *this);
GDK_AVAILABLE_IN_ALL
void                  gtk_label_set_use_underline (GtkLabel      *this,
						   gboolean       setting);
GDK_AVAILABLE_IN_ALL
gboolean              gtk_label_get_use_underline (GtkLabel      *this);

GDK_AVAILABLE_IN_ALL
void     gtk_label_set_markup_with_mnemonic       (GtkLabel         *this,
						   const char       *str);
GDK_AVAILABLE_IN_ALL
guint    gtk_label_get_mnemonic_keyval            (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_mnemonic_widget            (GtkLabel         *this,
						   GtkWidget        *widget);
GDK_AVAILABLE_IN_ALL
GtkWidget *gtk_label_get_mnemonic_widget          (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_text_with_mnemonic         (GtkLabel         *this,
						   const char       *str);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_justify                    (GtkLabel         *this,
						   GtkJustification  jtype);
GDK_AVAILABLE_IN_ALL
GtkJustification gtk_label_get_justify            (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_ellipsize                  (GtkLabel         *this,
                                                   PangoEllipsizeMode mode);
GDK_AVAILABLE_IN_ALL
PangoEllipsizeMode gtk_label_get_ellipsize        (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_width_chars                (GtkLabel         *this,
                                                   int               n_chars);
GDK_AVAILABLE_IN_ALL
int      gtk_label_get_width_chars                (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_max_width_chars            (GtkLabel         *this,
                                                   int               n_chars);
GDK_AVAILABLE_IN_ALL
int      gtk_label_get_max_width_chars            (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_lines                      (GtkLabel         *this,
                                                   int               lines);
GDK_AVAILABLE_IN_ALL
int      gtk_label_get_lines                      (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_wrap                       (GtkLabel         *this,
                                                   gboolean          wrap);
GDK_AVAILABLE_IN_ALL
gboolean gtk_label_get_wrap                       (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_wrap_mode                  (GtkLabel         *this,
                                                   PangoWrapMode     wrap_mode);
GDK_AVAILABLE_IN_ALL
PangoWrapMode gtk_label_get_wrap_mode             (GtkLabel         *this);
GDK_AVAILABLE_IN_4_6
void          gtk_label_set_natural_wrap_mode     (GtkLabel         *this,
                                                   GtkNaturalWrapMode wrap_mode);
GDK_AVAILABLE_IN_4_6
GtkNaturalWrapMode gtk_label_get_natural_wrap_mode(GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_set_selectable                 (GtkLabel         *this,
						   gboolean          setting);
GDK_AVAILABLE_IN_ALL
gboolean gtk_label_get_selectable                 (GtkLabel         *this);
GDK_AVAILABLE_IN_ALL
void     gtk_label_select_region                  (GtkLabel         *this,
						   int               start_offset,
						   int               end_offset);
GDK_AVAILABLE_IN_ALL
gboolean gtk_label_get_selection_bounds           (GtkLabel         *this,
                                                   int              *start,
                                                   int              *end);

GDK_AVAILABLE_IN_ALL
PangoLayout *gtk_label_get_layout         (GtkLabel *this);
GDK_AVAILABLE_IN_ALL
void         gtk_label_get_layout_offsets (GtkLabel *this,
                                           int      *x,
                                           int      *y);

GDK_AVAILABLE_IN_ALL
void         gtk_label_set_single_line_mode  (GtkLabel *this,
                                              gboolean single_line_mode);
GDK_AVAILABLE_IN_ALL
gboolean     gtk_label_get_single_line_mode  (GtkLabel *this);

GDK_AVAILABLE_IN_ALL
const char *gtk_label_get_current_uri (GtkLabel *this);

GDK_AVAILABLE_IN_ALL
void         gtk_label_set_xalign (GtkLabel *this,
                                   float     xalign);

GDK_AVAILABLE_IN_ALL
float        gtk_label_get_xalign (GtkLabel *this);

GDK_AVAILABLE_IN_ALL
void         gtk_label_set_yalign (GtkLabel *this,
                                   float     yalign);

GDK_AVAILABLE_IN_ALL
float        gtk_label_get_yalign (GtkLabel *this);

GDK_AVAILABLE_IN_ALL
void         gtk_label_set_extra_menu (GtkLabel   *this,
                                       GMenuModel *model);
GDK_AVAILABLE_IN_ALL
GMenuModel * gtk_label_get_extra_menu (GtkLabel   *this);

GDK_AVAILABLE_IN_4_8
void             gtk_label_set_tabs (GtkLabel      *this,
                                     PangoTabArray *tabs);

GDK_AVAILABLE_IN_4_8
PangoTabArray * gtk_label_get_tabs  (GtkLabel      *this);



G_DEFINE_AUTOPTR_CLEANUP_FUNC(GtkLabel, g_object_unref)

G_END_DECLS

