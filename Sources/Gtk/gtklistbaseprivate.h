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

#include "gtklistbase.h"

#include "gtklistitemmanagerprivate.h"
#include "gtkprivate.h"

struct _GtkListBase
{
  GtkWidget parent_instance;
};

struct _GtkListBaseClass
{
  GtkWidgetClass parent_class;

  GtkListTile *        (* split)                                (GtkListBase            *this,
                                                                 GtkListTile            *tile,
                                                                 guint                   n_items);
  GtkListItemBase *    (* create_list_widget)                   (GtkListBase            *this);
  void                 (* prepare_section)                      (GtkListBase            *this,
                                                                 GtkListTile            *tile,
                                                                 guint                   position);
  GtkListHeaderBase *  (* create_header_widget)                 (GtkListBase            *this);

  gboolean             (* get_allocation)                       (GtkListBase            *this,
                                                                 guint                   pos,
                                                                 GdkRectangle           *area);
  gboolean             (* get_position_from_allocation)         (GtkListBase            *this,
                                                                 int                     across,
                                                                 int                     along,
                                                                 guint                  *pos,
                                                                 cairo_rectangle_int_t  *area);
  GtkBitset *          (* get_items_in_rect)                    (GtkListBase            *this,
                                                                 const cairo_rectangle_int_t *rect);
  guint                (* move_focus_along)                     (GtkListBase            *this,
                                                                 guint                   pos,
                                                                 int                     steps);
  guint                (* move_focus_across)                    (GtkListBase            *this,
                                                                 guint                   pos,
                                                                 int                     steps);
};

GtkOrientation         gtk_list_base_get_orientation            (GtkListBase            *this);
#define gtk_list_base_get_opposite_orientation(this) OPPOSITE_ORIENTATION(gtk_list_base_get_orientation(this))
void                   gtk_list_base_get_border_spacing         (GtkListBase            *this,
                                                                 int                    *xspacing,
                                                                 int                    *yspacing);
GtkListItemManager *   gtk_list_base_get_manager                (GtkListBase            *this);
GtkScrollablePolicy    gtk_list_base_get_scroll_policy          (GtkListBase            *this,
                                                                 GtkOrientation          orientation);
guint                  gtk_list_base_get_n_items                (GtkListBase            *this);
GtkSelectionModel *    gtk_list_base_get_model                  (GtkListBase            *this);
gboolean               gtk_list_base_set_model                  (GtkListBase            *this,
                                                                 GtkSelectionModel      *model);

guint                  gtk_list_base_get_anchor                 (GtkListBase            *this);
void                   gtk_list_base_set_anchor                 (GtkListBase            *this,
                                                                 guint                   anchor_pos,
                                                                 double                  anchor_align_across,
                                                                 GtkPackType             anchor_side_across,
                                                                 double                  anchor_align_along,
                                                                 GtkPackType             anchor_side_along);
void                   gtk_list_base_set_anchor_max_widgets     (GtkListBase            *this,
                                                                 guint                   n_center,
                                                                 guint                   n_above_below);

void                   gtk_list_base_set_enable_rubberband      (GtkListBase            *this,
                                                                 gboolean                enable);
gboolean               gtk_list_base_get_enable_rubberband      (GtkListBase            *this);
void                   gtk_list_base_set_tab_behavior           (GtkListBase            *this,
                                                                 GtkListTabBehavior      behavior);
GtkListTabBehavior     gtk_list_base_get_tab_behavior           (GtkListBase            *this);


void                   gtk_list_base_allocate                   (GtkListBase            *this);

void                   gtk_list_base_scroll_to                  (GtkListBase            *this,
                                                                 guint                   pos,
                                                                 GtkListScrollFlags      flags,
                                                                 GtkScrollInfo          *scroll);
