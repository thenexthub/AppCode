/* gtktreeviewcolumn.h
 * Copyright (C) 2000  Red Hat, Inc.,  Jonathan Blandford <jrb@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#if !defined (__GTK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gtk/gtk.h> can be included directly."
#endif

#include <gtk/deprecated/gtkcellrenderer.h>
#include <gtk/deprecated/gtktreemodel.h>
#include <gtk/deprecated/gtktreesortable.h>
#include <gtk/deprecated/gtkcellarea.h>


G_BEGIN_DECLS


#define GTK_TYPE_TREE_VIEW_COLUMN	     (gtk_tree_view_column_get_type ())
#define GTK_TREE_VIEW_COLUMN(obj)	     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_TREE_VIEW_COLUMN, GtkTreeViewColumn))
#define GTK_IS_TREE_VIEW_COLUMN(obj)	     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_TREE_VIEW_COLUMN))

typedef struct _GtkTreeViewColumn        GtkTreeViewColumn;

/**
 * GtkTreeViewColumnSizing:
 * @GTK_TREE_VIEW_COLUMN_GROW_ONLY: Columns only get bigger in reaction to changes in the model
 * @GTK_TREE_VIEW_COLUMN_AUTOSIZE: Columns resize to be the optimal size every time the model changes.
 * @GTK_TREE_VIEW_COLUMN_FIXED: Columns are a fixed numbers of pixels wide.
 *
 * The sizing method the column uses to determine its width.  Please note
 * that %GTK_TREE_VIEW_COLUMN_AUTOSIZE are inefficient for large views, and
 * can make columns appear choppy.
 *
 * Deprecated: 4.20: There is no replacement.
 */
typedef enum
{
  GTK_TREE_VIEW_COLUMN_GROW_ONLY,
  GTK_TREE_VIEW_COLUMN_AUTOSIZE,
  GTK_TREE_VIEW_COLUMN_FIXED
} GtkTreeViewColumnSizing;

/**
 * GtkTreeCellDataFunc:
 * @tree_column: A `GtkTreeViewColumn`
 * @cell: The `GtkCellRenderer` that is being rendered by @tree_column
 * @tree_model: The `GtkTreeModel` being rendered
 * @iter: A `GtkTreeIter` of the current row rendered
 * @data: (closure): user data
 *
 * A function to set the properties of a cell instead of just using the
 * straight mapping between the cell and the model.
 *
 * This function is useful for customizing the cell renderer. For example,
 * a function might get an* integer from the @tree_model, and render it to
 * the “text” attribute of “cell” by converting it to its written equivalent.
 *
 * See also: gtk_tree_view_column_set_cell_data_func()
 *
 * Deprecated: 4.20: There is no replacement
 */
typedef void (* GtkTreeCellDataFunc) (GtkTreeViewColumn *tree_column,
				      GtkCellRenderer   *cell,
				      GtkTreeModel      *tree_model,
				      GtkTreeIter       *iter,
				      gpointer           data);


GDK_AVAILABLE_IN_ALL
GType                   gtk_tree_view_column_get_type            (void) G_GNUC_CONST;
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkTreeViewColumn      *gtk_tree_view_column_new                 (void);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkTreeViewColumn      *gtk_tree_view_column_new_with_area       (GtkCellArea             *area);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkTreeViewColumn      *gtk_tree_view_column_new_with_attributes (const char              *title,
								  GtkCellRenderer         *cell,
								  ...) G_GNUC_NULL_TERMINATED;
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_pack_start          (GtkTreeViewColumn       *tree_column,
								  GtkCellRenderer         *cell,
								  gboolean                 expand);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_pack_end            (GtkTreeViewColumn       *tree_column,
								  GtkCellRenderer         *cell,
								  gboolean                 expand);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_clear               (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_add_attribute       (GtkTreeViewColumn       *tree_column,
								  GtkCellRenderer         *cell_renderer,
								  const char              *attribute,
								  int                      column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_attributes      (GtkTreeViewColumn       *tree_column,
								  GtkCellRenderer         *cell_renderer,
								  ...) G_GNUC_NULL_TERMINATED;
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_cell_data_func  (GtkTreeViewColumn       *tree_column,
								  GtkCellRenderer         *cell_renderer,
								  GtkTreeCellDataFunc      func,
								  gpointer                 func_data,
								  GDestroyNotify           destroy);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_clear_attributes    (GtkTreeViewColumn       *tree_column,
								  GtkCellRenderer         *cell_renderer);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_spacing         (GtkTreeViewColumn       *tree_column,
								  int                      spacing);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
int                     gtk_tree_view_column_get_spacing         (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_visible         (GtkTreeViewColumn       *tree_column,
								  gboolean                 visible);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_get_visible         (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_resizable       (GtkTreeViewColumn       *tree_column,
								  gboolean                 resizable);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_get_resizable       (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_sizing          (GtkTreeViewColumn       *tree_column,
								  GtkTreeViewColumnSizing  type);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkTreeViewColumnSizing gtk_tree_view_column_get_sizing          (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
int                     gtk_tree_view_column_get_x_offset        (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
int                     gtk_tree_view_column_get_width           (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
int                     gtk_tree_view_column_get_fixed_width     (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_fixed_width     (GtkTreeViewColumn       *tree_column,
								  int                      fixed_width);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_min_width       (GtkTreeViewColumn       *tree_column,
								  int                      min_width);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
int                     gtk_tree_view_column_get_min_width       (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_max_width       (GtkTreeViewColumn       *tree_column,
								  int                      max_width);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
int                     gtk_tree_view_column_get_max_width       (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_clicked             (GtkTreeViewColumn       *tree_column);



/* Options for manipulating the column headers
 */
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_title           (GtkTreeViewColumn       *tree_column,
								  const char              *title);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
const char *           gtk_tree_view_column_get_title           (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_expand          (GtkTreeViewColumn       *tree_column,
								  gboolean                 expand);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_get_expand          (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_clickable       (GtkTreeViewColumn       *tree_column,
								  gboolean                 clickable);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_get_clickable       (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_widget          (GtkTreeViewColumn       *tree_column,
								  GtkWidget               *widget);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkWidget              *gtk_tree_view_column_get_widget          (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_alignment       (GtkTreeViewColumn       *tree_column,
								  float                    xalign);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
float                   gtk_tree_view_column_get_alignment       (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_reorderable     (GtkTreeViewColumn       *tree_column,
								  gboolean                 reorderable);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_get_reorderable     (GtkTreeViewColumn       *tree_column);



/* You probably only want to use gtk_tree_view_column_set_sort_column_id.  The
 * other sorting functions exist primarily to immutable others do their own custom sorting.
 */
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_sort_column_id  (GtkTreeViewColumn       *tree_column,
								  int                      sort_column_id);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
int                     gtk_tree_view_column_get_sort_column_id  (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_sort_indicator  (GtkTreeViewColumn       *tree_column,
								  gboolean                 setting);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_get_sort_indicator  (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_set_sort_order      (GtkTreeViewColumn       *tree_column,
								  GtkSortType              order);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkSortType             gtk_tree_view_column_get_sort_order      (GtkTreeViewColumn       *tree_column);


/* These functions are meant primarily for interaction between the GtkTreeView and the column.
 */
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_cell_set_cell_data  (GtkTreeViewColumn       *tree_column,
								  GtkTreeModel            *tree_model,
								  GtkTreeIter             *iter,
								  gboolean                 is_expander,
								  gboolean                 is_expanded);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_cell_get_size       (GtkTreeViewColumn       *tree_column,
                                                                  int                     *x_offset,
                                                                  int                     *y_offset,
                                                                  int                     *width,
                                                                  int                     *height);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_cell_is_visible     (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_focus_cell          (GtkTreeViewColumn       *tree_column,
								  GtkCellRenderer         *cell);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
gboolean                gtk_tree_view_column_cell_get_position   (GtkTreeViewColumn       *tree_column,
					                          GtkCellRenderer         *cell_renderer,
					                          int                     *x_offset,
					                          int                     *width);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
void                    gtk_tree_view_column_queue_resize        (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkWidget              *gtk_tree_view_column_get_tree_view       (GtkTreeViewColumn       *tree_column);
GDK_DEPRECATED_IN_4_10_FOR(GtkColumnView and GtkColumnViewColumn)
GtkWidget              *gtk_tree_view_column_get_button          (GtkTreeViewColumn       *tree_column);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GtkTreeViewColumn, g_object_unref)

G_END_DECLS
