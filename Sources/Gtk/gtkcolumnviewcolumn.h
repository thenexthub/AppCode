/*
 * Copyright © 2019 Benjamin Otte
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

#pragma once

#if !defined (__GTK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gtk/gtk.h> can be included directly."
#endif

#include <gtk/gtkcolumnview.h>
#include <gtk/gtksorter.h>

G_BEGIN_DECLS

#define GTK_TYPE_COLUMN_VIEW_COLUMN         (gtk_column_view_column_get_type ())
#define GTK_COLUMN_VIEW_COLUMN(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GTK_TYPE_COLUMN_VIEW_COLUMN, GtkColumnViewColumn))
#define GTK_COLUMN_VIEW_COLUMN_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), GTK_TYPE_COLUMN_VIEW_COLUMN, GtkColumnViewColumnClass))
#define GTK_IS_COLUMN_VIEW_COLUMN(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTK_TYPE_COLUMN_VIEW_COLUMN))
#define GTK_IS_COLUMN_VIEW_COLUMN_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GTK_TYPE_COLUMN_VIEW_COLUMN))
#define GTK_COLUMN_VIEW_COLUMN_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GTK_TYPE_COLUMN_VIEW_COLUMN, GtkColumnViewColumnClass))
G_DEFINE_AUTOPTR_CLEANUP_FUNC(GtkColumnViewColumn, g_object_unref)

typedef struct _GtkColumnViewColumnClass GtkColumnViewColumnClass;

GDK_AVAILABLE_IN_ALL
GType                   gtk_column_view_column_get_type                 (void) G_GNUC_CONST;

GDK_AVAILABLE_IN_ALL
GtkColumnViewColumn *   gtk_column_view_column_new                      (const char             *title,
                                                                         GtkListItemFactory     *factory);

GDK_AVAILABLE_IN_ALL
GtkColumnView *         gtk_column_view_column_get_column_view          (GtkColumnViewColumn    *this);
GDK_AVAILABLE_IN_ALL
void                    gtk_column_view_column_set_factory              (GtkColumnViewColumn    *this,
                                                                         GtkListItemFactory     *factory);
GDK_AVAILABLE_IN_ALL
GtkListItemFactory *    gtk_column_view_column_get_factory              (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_ALL
void                    gtk_column_view_column_set_title                (GtkColumnViewColumn    *this,
                                                                         const char             *title);
GDK_AVAILABLE_IN_ALL
const char *            gtk_column_view_column_get_title                (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_ALL
void                    gtk_column_view_column_set_sorter               (GtkColumnViewColumn    *this,
                                                                         GtkSorter              *sorter);
GDK_AVAILABLE_IN_ALL
GtkSorter *             gtk_column_view_column_get_sorter               (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_ALL
void                    gtk_column_view_column_set_visible              (GtkColumnViewColumn    *this,
                                                                         gboolean                visible);
GDK_AVAILABLE_IN_ALL
gboolean                gtk_column_view_column_get_visible              (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_ALL

void                    gtk_column_view_column_set_header_menu          (GtkColumnViewColumn    *this,
                                                                         GMenuModel             *menu);
GDK_AVAILABLE_IN_ALL
GMenuModel *            gtk_column_view_column_get_header_menu          (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_ALL
void                    gtk_column_view_column_set_fixed_width          (GtkColumnViewColumn    *this,
                                                                         int                     fixed_width);
GDK_AVAILABLE_IN_ALL
int                     gtk_column_view_column_get_fixed_width          (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_ALL
void                    gtk_column_view_column_set_resizable            (GtkColumnViewColumn    *this,
                                                                         gboolean                resizable);
GDK_AVAILABLE_IN_ALL
gboolean                gtk_column_view_column_get_resizable            (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_ALL
void                    gtk_column_view_column_set_expand               (GtkColumnViewColumn    *this,
                                                                         gboolean                expand);
GDK_AVAILABLE_IN_ALL
gboolean                gtk_column_view_column_get_expand               (GtkColumnViewColumn    *this);

GDK_AVAILABLE_IN_4_10
void                    gtk_column_view_column_set_id                   (GtkColumnViewColumn    *this,
                                                                         const char             *id);
GDK_AVAILABLE_IN_4_10
const char *            gtk_column_view_column_get_id                   (GtkColumnViewColumn    *this);

G_END_DECLS

