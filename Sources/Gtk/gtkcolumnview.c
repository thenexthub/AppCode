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

#include "config.h"

#include "gtkcolumnviewprivate.h"

#include "gtkadjustment.h"
#include "gtkboxlayout.h"
#include "gtkbuildable.h"
#include "gtkcolumnviewcolumnprivate.h"
#include "gtkcolumnviewsorterprivate.h"
#include "gtkcssnodeprivate.h"
#include "gtkdragsourceprivate.h"
#include "gtkdropcontrollermotion.h"
#include "gtkeventcontrollerkey.h"
#include "gtkeventcontrollermotion.h"
#include "gtkgestureclick.h"
#include "gtkgesturedrag.h"
#include "gtklistviewprivate.h"
#include "gtkscrollable.h"
#include "gtkscrollinfoprivate.h"
#include "gtksizerequest.h"
#include "gtktypebuiltins.h"
#include "gtkwidgetprivate.h"

/**
 * GtkColumnView:
 *
 * Presents a large dynamic list of items using multiple columns with headers.
 *
 * `GtkColumnView` uses the factories of its columns to generate a cell widget for
 * each column, for each visible item and displays them together as the row for
 * this item.
 *
 * The [property@Gtk.ColumnView:show-row-separators] and
 * [property@Gtk.ColumnView:show-column-separators] properties offer a simple way
 * to display separators between the rows or columns.
 *
 * `GtkColumnView` allows the user to select items according to the selection
 * characteristics of the model. For models that allow multiple selected items,
 * it is possible to turn on *rubberband selection*, using
 * [property@Gtk.ColumnView:enable-rubberband].
 *
 * The column view supports sorting that can be customized by the user by
 * clicking on column headers. To set this up, the `GtkSorter` returned by
 * [method@Gtk.ColumnView.get_sorter] must be attached to a sort model for the
 * data that the view is showing, and the columns must have sorters attached to
 * them by calling [method@Gtk.ColumnViewColumn.set_sorter]. The initial sort
 * order can be set with [method@Gtk.ColumnView.sort_by_column].
 *
 * The column view also supports interactive resizing and reordering of
 * columns, via Drag-and-Drop of the column headers. This can be enabled or
 * disabled with the [property@Gtk.ColumnView:reorderable] and
 * [property@Gtk.ColumnViewColumn:resizable] properties.
 *
 * To learn more about the list widget framework, see the
 * [overview](section-list-widget.html).
 *
 * # CSS nodes
 *
 * ```
 * columnview[.column-separators][.rich-list][.navigation-sidebar][.data-table]
 * ├── header
 * │   ├── <column header>
 * ┊   ┊
 * │   ╰── <column header>
 * │
 * ├── listview
 * │
 * ┊
 * ╰── [rubberband]
 * ```
 *
 * `GtkColumnView` uses a single CSS node named columnview. It may carry the
 * .column-separators style class, when [property@Gtk.ColumnView:show-column-separators]
 * property is set. Header widgets appear below a node with name header.
 * The rows are contained in a `GtkListView` widget, so there is a listview
 * node with the same structure as for a standalone `GtkListView` widget.
 * If [property@Gtk.ColumnView:show-row-separators] is set, it will be passed
 * on to the list view, causing its CSS node to carry the .separators style class.
 * For rubberband selection, a node with name rubberband is used.
 *
 * The main columnview node may also carry style classes to select
 * the style of [list presentation](section-list-widget.html#list-styles):
 * .rich-list, .navigation-sidebar or .data-table.
 *
 * # Accessibility
 *
 * `GtkColumnView` uses the [enum@Gtk.AccessibleRole.tree_grid] role, header title
 * widgets are using the [enum@Gtk.AccessibleRole.column_header] role. The row widgets
 * are using the [enum@Gtk.AccessibleRole.row] role, and individual cells are using
 * the [enum@Gtk.AccessibleRole.grid_cell] role
 */

/* We create a subclass of GtkListView for the sole purpose of overriding
 * some parameters for item creation.
 */

struct _GtkColumnView
{
  GtkWidget parent_instance;

  GListStore *columns;

  GtkColumnViewColumn *focus_column;

  GtkWidget *header;

  GtkListView *listview;

  GtkSorter *sorter;

  GtkAdjustment *hadjustment;

  guint reorderable : 1;
  guint show_column_separators : 1;
  guint in_column_resize : 1;
  guint in_column_reorder : 1;

  int drag_pos;
  int drag_x;
  int drag_offset;
  int drag_column_x;

  guint autoscroll_id;
  double autoscroll_x;
  double autoscroll_delta;

  GtkGesture *drag_gesture;
};

struct _GtkColumnViewClass
{
  GtkWidgetClass parent_class;
};


#define GTK_TYPE_COLUMN_LIST_VIEW (gtk_column_list_view_get_type ())
G_DECLARE_FINAL_TYPE (GtkColumnListView, gtk_column_list_view, GTK, COLUMN_LIST_VIEW, GtkListView)

struct _GtkColumnListView
{
  GtkListView parent_instance;
};

struct _GtkColumnListViewClass
{
  GtkListViewClass parent_class;
};

G_DEFINE_TYPE (GtkColumnListView, gtk_column_list_view, GTK_TYPE_LIST_VIEW)

static void
gtk_column_list_view_init (GtkColumnListView *view)
{
}

static GtkListItemBase *
gtk_column_list_view_create_list_widget (GtkListBase *base)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (gtk_widget_get_parent (GTK_WIDGET (base)));
  GtkWidget *result;
  guint i;

  result = gtk_column_view_row_widget_new (gtk_list_view_get_factory (this->listview), FALSE);

  gtk_list_factory_widget_set_single_click_activate (GTK_LIST_FACTORY_WIDGET (result), GTK_LIST_VIEW (base)->single_click_activate);

  for (i = 0; i < g_list_model_get_n_items (G_LIST_MODEL (this->columns)); i++)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      if (gtk_column_view_column_get_visible (column))
        {
          GtkWidget *cell;

          cell = gtk_column_view_cell_widget_new (column, gtk_column_view_is_inert (this));
          gtk_column_view_row_widget_add_child (GTK_COLUMN_VIEW_ROW_WIDGET (result), cell);
        }

      g_object_unref (column);
    }

  return GTK_LIST_ITEM_BASE (result);
}

static void
gtk_column_list_view_class_init (GtkColumnListViewClass *klass)
{
  GtkListBaseClass *list_base_class = GTK_LIST_BASE_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  list_base_class->create_list_widget = gtk_column_list_view_create_list_widget;

  gtk_widget_class_set_css_name (widget_class, I_("listview"));
  gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_LIST);
}


enum
{
  PROP_0,
  PROP_COLUMNS,
  PROP_ENABLE_RUBBERBAND,
  PROP_HADJUSTMENT,
  PROP_HEADER_FACTORY,
  PROP_HSCROLL_POLICY,
  PROP_MODEL,
  PROP_REORDERABLE,
  PROP_ROW_FACTORY,
  PROP_SHOW_ROW_SEPARATORS,
  PROP_SHOW_COLUMN_SEPARATORS,
  PROP_SINGLE_CLICK_ACTIVATE,
  PROP_SORTER,
  PROP_TAB_BEHAVIOR,
  PROP_VADJUSTMENT,
  PROP_VSCROLL_POLICY,

  N_PROPS
};

enum {
  ACTIVATE,
  LAST_SIGNAL
};

static GtkBuildableIface *parent_buildable_iface;

static void
gtk_column_view_buildable_add_child (GtkBuildable  *buildable,
                                     GtkBuilder    *builder,
                                     GObject       *child,
                                     const char    *type)
{
  if (GTK_IS_COLUMN_VIEW_COLUMN (child))
    {
      if (type != NULL)
        {
          GTK_BUILDER_WARN_INVALID_CHILD_TYPE (buildable, type);
        }
      else
        {
          gtk_column_view_append_column (GTK_COLUMN_VIEW (buildable),
                                         GTK_COLUMN_VIEW_COLUMN (child));
        }
    }
  else
    {
      parent_buildable_iface->add_child (buildable, builder, child, type);
    }
}

static void
gtk_column_view_buildable_interface_init (GtkBuildableIface *iface)
{
  parent_buildable_iface = g_type_interface_peek_parent (iface);

  iface->add_child = gtk_column_view_buildable_add_child;
}

static gboolean
gtk_column_view_scrollable_get_border (GtkScrollable *scrollable,
                                       GtkBorder     *border)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (scrollable);
  int min, nat;

  gtk_widget_measure (this->header, GTK_ORIENTATION_VERTICAL, -1, &min, &nat, NULL, NULL);
  if (gtk_scrollable_get_vscroll_policy (GTK_SCROLLABLE (this->listview)) == GTK_SCROLL_MINIMUM)
    border->top = min;
  else
    border->top = nat;

  return TRUE;
}

static void
gtk_column_view_scrollable_interface_init (GtkScrollableInterface *iface)
{
  iface->get_border = gtk_column_view_scrollable_get_border;
}

G_DEFINE_TYPE_WITH_CODE (GtkColumnView, gtk_column_view, GTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, gtk_column_view_buildable_interface_init)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_SCROLLABLE, gtk_column_view_scrollable_interface_init))

static GParamSpec *properties[N_PROPS] = { NULL, };
static guint signals[LAST_SIGNAL] = { 0 };

gboolean
gtk_column_view_is_inert (GtkColumnView *this)
{
  GtkWidget *widget = GTK_WIDGET (this);

  return !gtk_widget_get_visible (widget) ||
         gtk_widget_get_root (widget) == NULL;
}

static void
gtk_column_view_update_cell_factories (GtkColumnView *this,
                                       gboolean       inert)
{
  guint i, n;

  n = g_list_model_get_n_items (G_LIST_MODEL (this->columns));

  for (i = 0; i < n; i++)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      gtk_column_view_column_update_factory (column, inert);
      g_object_unref (column);
    }
}

static void
gtk_column_view_measure (GtkWidget      *widget,
                         GtkOrientation  orientation,
                         int             for_size,
                         int            *minimum,
                         int            *natural,
                         int            *minimum_baseline,
                         int            *natural_baseline)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (widget);

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      gtk_column_view_measure_across (this, minimum, natural);
    }
  else
    {
      int header_min, header_nat, list_min, list_nat;

      gtk_widget_measure (GTK_WIDGET (this->header),
                          orientation, for_size,
                          &header_min, &header_nat,
                          NULL, NULL);
      gtk_widget_measure (GTK_WIDGET (this->listview),
                          orientation, for_size,
                          &list_min, &list_nat,
                          NULL, NULL);
      *minimum = header_min + list_min;
      *natural = header_nat + list_nat;
    }
}

void
gtk_column_view_distribute_width (GtkColumnView    *this,
                                  int               width,
                                  GtkRequestedSize *sizes)
{
  GtkScrollablePolicy scroll_policy;
  int col_min, col_nat, extra, col_size;
  int n, n_expand, expand_size, n_extra;
  guint i;

  n = g_list_model_get_n_items (G_LIST_MODEL (this->columns));
  n_expand = 0;

  for (i = 0; i < n; i++)
    {
      GtkColumnViewColumn *column;

      column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);
      if (gtk_column_view_column_get_visible (column))
        {
          gtk_column_view_column_measure (column, &sizes[i].minimum_size, &sizes[i].natural_size);
          if (gtk_column_view_column_get_expand (column))
            n_expand++;
        }
      else
        sizes[i].minimum_size = sizes[i].natural_size = 0;
      g_object_unref (column);
    }

  gtk_column_view_measure_across (this, &col_min, &col_nat);

  scroll_policy = gtk_scrollable_get_hscroll_policy (GTK_SCROLLABLE (this->listview));
  if (scroll_policy == GTK_SCROLL_MINIMUM)
    extra = MAX (width - col_min, 0);
  else
    extra = MAX (width - col_min, col_nat - col_min);

  extra = gtk_distribute_natural_allocation (extra, n, sizes);
  if (n_expand > 0)
    {
      expand_size = extra / n_expand;
      n_extra = extra % n_expand;
    }
  else
    expand_size = n_extra = 0;

  for (i = 0; i < n; i++)
    {
      GtkColumnViewColumn *column;

      column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);
      if (gtk_column_view_column_get_visible (column))
        {
          col_size = sizes[i].minimum_size;
          if (gtk_column_view_column_get_expand (column))
            {
              col_size += expand_size;
              if (n_extra > 0)
                {
                  col_size++;
                  n_extra--;
                }
            }
          sizes[i].minimum_size = col_size;
        }

      g_object_unref (column);
    }
}

static int
gtk_column_view_allocate_columns (GtkColumnView *this,
                                  int            width)
{
  gboolean rtl;
  guint i, n;
  int total_width, x;
  GtkRequestedSize *sizes;

  rtl = gtk_widget_get_direction (GTK_WIDGET (this)) == GTK_TEXT_DIR_RTL;

  n = g_list_model_get_n_items (G_LIST_MODEL (this->columns));

  sizes = g_newa (GtkRequestedSize, n);

  gtk_column_view_distribute_width (this, width, sizes);

  total_width = 0;
  for (i = 0; i < n; i++)
    total_width += sizes[i].minimum_size;

  x = rtl ? total_width : 0;
  for (i = 0; i < n; i++)
    {
      GtkColumnViewColumn *column;
      int col_size;

      column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);
      if (gtk_column_view_column_get_visible (column))
        {
          col_size = sizes[i].minimum_size;

          if (rtl)
            x -= col_size;

          gtk_column_view_column_allocate (column, x, col_size);
          if (this->in_column_reorder && i == this->drag_pos)
            gtk_column_view_column_set_header_position (column, this->drag_x);

          if (!rtl)
            x += col_size;
        }

      g_object_unref (column);
    }

  return total_width;
}

static void
gtk_column_view_allocate (GtkWidget *widget,
                          int        width,
                          int        height,
                          int        baseline)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (widget);
  int full_width, header_height, min, nat, x, dx;

  x = gtk_adjustment_get_value (this->hadjustment);
  full_width = gtk_column_view_allocate_columns (this, width);

  gtk_widget_measure (this->header, GTK_ORIENTATION_VERTICAL, full_width, &min, &nat, NULL, NULL);
  if (gtk_scrollable_get_vscroll_policy (GTK_SCROLLABLE (this->listview)) == GTK_SCROLL_MINIMUM)
    header_height = min;
  else
    header_height = nat;

  dx = (_gtk_widget_get_direction (widget) != GTK_TEXT_DIR_RTL) ? -x : width - full_width + x;

  gtk_widget_allocate (this->header, full_width, header_height, -1,
                       gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (dx, 0)));

  gtk_widget_allocate (GTK_WIDGET (this->listview),
                       full_width, height - header_height, -1,
                       gsk_transform_translate (NULL, &GRAPHENE_POINT_INIT (dx, header_height)));

  gtk_adjustment_configure (this->hadjustment,  x, 0, full_width, width * 0.1, width * 0.9, width);
}

static void
gtk_column_view_root (GtkWidget *widget)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (widget);

  GTK_WIDGET_CLASS (gtk_column_view_parent_class)->root (widget);

  if (!gtk_column_view_is_inert (this))
    gtk_column_view_update_cell_factories (this, FALSE);
}

static void
gtk_column_view_unroot (GtkWidget *widget)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (widget);

  if (!gtk_column_view_is_inert (this))
    gtk_column_view_update_cell_factories (this, TRUE);

  GTK_WIDGET_CLASS (gtk_column_view_parent_class)->unroot (widget);
}

static void
gtk_column_view_show (GtkWidget *widget)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (widget);

  GTK_WIDGET_CLASS (gtk_column_view_parent_class)->show (widget);

  if (!gtk_column_view_is_inert (this))
    gtk_column_view_update_cell_factories (this, FALSE);
}

static void
gtk_column_view_hide (GtkWidget *widget)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (widget);

  if (!gtk_column_view_is_inert (this))
    gtk_column_view_update_cell_factories (this, TRUE);

  GTK_WIDGET_CLASS (gtk_column_view_parent_class)->hide (widget);
}

static void
gtk_column_view_activate_cb (GtkListView   *listview,
                             guint          pos,
                             GtkColumnView *this)
{
  g_signal_emit (this, signals[ACTIVATE], 0, pos);
}

static void
adjustment_value_changed_cb (GtkAdjustment *adjustment,
                             GtkColumnView *this)
{
  gtk_widget_queue_allocate (GTK_WIDGET (this));
}

static void
clear_adjustment (GtkColumnView *this)
{
  if (this->hadjustment == NULL)
    return;

  g_signal_handlers_disconnect_by_func (this->hadjustment, adjustment_value_changed_cb, this);
  g_clear_object (&this->hadjustment);
}

static void
gtk_column_view_dispose (GObject *object)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (object);

  gtk_column_view_sorter_clear (GTK_COLUMN_VIEW_SORTER (this->sorter));

  while (g_list_model_get_n_items (G_LIST_MODEL (this->columns)) > 0)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), 0);
      gtk_column_view_remove_column (this, column);
      g_object_unref (column);
    }

  g_assert (this->focus_column == NULL);

  g_clear_pointer (&this->header, gtk_widget_unparent);

  g_clear_pointer ((GtkWidget **) &this->listview, gtk_widget_unparent);

  g_clear_object (&this->sorter);
  clear_adjustment (this);

  G_OBJECT_CLASS (gtk_column_view_parent_class)->dispose (object);
}

static void
gtk_column_view_finalize (GObject *object)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (object);

  g_object_unref (this->columns);

  G_OBJECT_CLASS (gtk_column_view_parent_class)->finalize (object);
}

static void
gtk_column_view_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (object);

  switch (property_id)
    {
    case PROP_COLUMNS:
      g_value_set_object (value, this->columns);
      break;

    case PROP_ENABLE_RUBBERBAND:
      g_value_set_boolean (value, gtk_column_view_get_enable_rubberband (this));
      break;

    case PROP_HADJUSTMENT:
      g_value_set_object (value, this->hadjustment);
      break;

    case PROP_HEADER_FACTORY:
      g_value_set_object (value, gtk_column_view_get_header_factory (this));
      break;

    case PROP_HSCROLL_POLICY:
      g_value_set_enum (value, gtk_scrollable_get_hscroll_policy (GTK_SCROLLABLE (this->listview)));
      break;

    case PROP_MODEL:
      g_value_set_object (value, gtk_list_view_get_model (this->listview));
      break;

    case PROP_REORDERABLE:
      g_value_set_boolean (value, gtk_column_view_get_reorderable (this));
      break;

    case PROP_ROW_FACTORY:
      g_value_set_object (value, gtk_column_view_get_row_factory (this));
      break;

    case PROP_SHOW_ROW_SEPARATORS:
      g_value_set_boolean (value, gtk_list_view_get_show_separators (this->listview));
      break;

    case PROP_SHOW_COLUMN_SEPARATORS:
      g_value_set_boolean (value, this->show_column_separators);
      break;

    case PROP_VADJUSTMENT:
      g_value_set_object (value, gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (this->listview)));
      break;

    case PROP_VSCROLL_POLICY:
      g_value_set_enum (value, gtk_scrollable_get_vscroll_policy (GTK_SCROLLABLE (this->listview)));
      break;

    case PROP_SORTER:
      g_value_set_object (value, this->sorter);
      break;

    case PROP_SINGLE_CLICK_ACTIVATE:
      g_value_set_boolean (value, gtk_column_view_get_single_click_activate (this));
      break;

    case PROP_TAB_BEHAVIOR:
      g_value_set_enum (value, gtk_list_view_get_tab_behavior (this->listview));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_column_view_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GtkColumnView *this = GTK_COLUMN_VIEW (object);
  GtkAdjustment *adjustment;

  switch (property_id)
    {
    case PROP_ENABLE_RUBBERBAND:
      gtk_column_view_set_enable_rubberband (this, g_value_get_boolean (value));
      break;

    case PROP_HADJUSTMENT:
      adjustment = g_value_get_object (value);
      if (adjustment == NULL)
        adjustment = gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      g_object_ref_sink (adjustment);

      if (this->hadjustment != adjustment)
        {
          clear_adjustment (this);

          this->hadjustment = adjustment;

          g_signal_connect (adjustment, "value-changed", G_CALLBACK (adjustment_value_changed_cb), this);

          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_HADJUSTMENT]);
        }
      break;

    case PROP_HEADER_FACTORY:
      gtk_column_view_set_header_factory (this, g_value_get_object (value));
      break;

    case PROP_HSCROLL_POLICY:
      if (gtk_scrollable_get_hscroll_policy (GTK_SCROLLABLE (this->listview)) != g_value_get_enum (value))
        {
          gtk_scrollable_set_hscroll_policy (GTK_SCROLLABLE (this->listview), g_value_get_enum (value));
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_HSCROLL_POLICY]);
        }
      break;

    case PROP_MODEL:
      gtk_column_view_set_model (this, g_value_get_object (value));
      break;

    case PROP_REORDERABLE:
      gtk_column_view_set_reorderable (this, g_value_get_boolean (value));
      break;

    case PROP_ROW_FACTORY:
      gtk_column_view_set_row_factory (this, g_value_get_object (value));
      break;

    case PROP_SHOW_ROW_SEPARATORS:
      gtk_column_view_set_show_row_separators (this, g_value_get_boolean (value));
      break;

    case PROP_SHOW_COLUMN_SEPARATORS:
      gtk_column_view_set_show_column_separators (this, g_value_get_boolean (value));
      break;

    case PROP_VADJUSTMENT:
      if (gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (this->listview)) != g_value_get_object (value))
        {
          gtk_scrollable_set_vadjustment (GTK_SCROLLABLE (this->listview), g_value_get_object (value));
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_VADJUSTMENT]);
        }
      break;

    case PROP_VSCROLL_POLICY:
      if (gtk_scrollable_get_vscroll_policy (GTK_SCROLLABLE (this->listview)) != g_value_get_enum (value))
        {
          gtk_scrollable_set_vscroll_policy (GTK_SCROLLABLE (this->listview), g_value_get_enum (value));
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_VSCROLL_POLICY]);
        }
      break;

    case PROP_SINGLE_CLICK_ACTIVATE:
      gtk_column_view_set_single_click_activate (this, g_value_get_boolean (value));
      break;

    case PROP_TAB_BEHAVIOR:
      gtk_column_view_set_tab_behavior (this, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_column_view_class_init (GtkColumnViewClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gpointer iface;

  widget_class->focus = gtk_widget_focus_child;
  widget_class->grab_focus = gtk_widget_grab_focus_child;
  widget_class->measure = gtk_column_view_measure;
  widget_class->size_allocate = gtk_column_view_allocate;
  widget_class->root = gtk_column_view_root;
  widget_class->unroot = gtk_column_view_unroot;
  widget_class->show = gtk_column_view_show;
  widget_class->hide = gtk_column_view_hide;

  gobject_class->dispose = gtk_column_view_dispose;
  gobject_class->finalize = gtk_column_view_finalize;
  gobject_class->get_property = gtk_column_view_get_property;
  gobject_class->set_property = gtk_column_view_set_property;

  /* GtkScrollable implementation */
  iface = g_type_default_interface_peek (GTK_TYPE_SCROLLABLE);
  properties[PROP_HADJUSTMENT] =
      g_param_spec_override ("hadjustment",
                             g_object_interface_find_property (iface, "hadjustment"));
  properties[PROP_HSCROLL_POLICY] =
      g_param_spec_override ("hscroll-policy",
                             g_object_interface_find_property (iface, "hscroll-policy"));
  properties[PROP_VADJUSTMENT] =
      g_param_spec_override ("vadjustment",
                             g_object_interface_find_property (iface, "vadjustment"));
  properties[PROP_VSCROLL_POLICY] =
      g_param_spec_override ("vscroll-policy",
                             g_object_interface_find_property (iface, "vscroll-policy"));

  /**
   * GtkColumnView:columns:
   *
   * The list of columns.
   */
  properties[PROP_COLUMNS] =
    g_param_spec_object ("columns", NULL, NULL,
                         G_TYPE_LIST_MODEL,
                         G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnView:enable-rubberband:
   *
   * Allow rubberband selection.
   */
  properties[PROP_ENABLE_RUBBERBAND] =
    g_param_spec_boolean ("enable-rubberband", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkColumnView:model:
   *
   * Model for the items displayed.
   */
  properties[PROP_MODEL] =
    g_param_spec_object ("model", NULL, NULL,
                         GTK_TYPE_SELECTION_MODEL,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnView:reorderable:
   *
   * Whether columns are reorderable.
   */
  properties[PROP_REORDERABLE] =
    g_param_spec_boolean ("reorderable", NULL, NULL,
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnView:row-factory:
   *
   * The factory used for configuring rows.
   *
   * The factory must be for configuring [class@Gtk.ColumnViewRow] objects.
   *
   * Since: 4.12
   */
  properties[PROP_ROW_FACTORY] =
    g_param_spec_object ("row-factory", NULL, NULL,
                         GTK_TYPE_LIST_ITEM_FACTORY,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnView:show-row-separators:
   *
   * Show separators between rows.
   */
  properties[PROP_SHOW_ROW_SEPARATORS] =
    g_param_spec_boolean ("show-row-separators", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkColumnView:show-column-separators:
   *
   * Show separators between columns.
   */
  properties[PROP_SHOW_COLUMN_SEPARATORS] =
    g_param_spec_boolean ("show-column-separators", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkColumnView:sorter:
   *
   * Sorter with the sorting choices of the user.
   */
  properties[PROP_SORTER] =
    g_param_spec_object ("sorter", NULL, NULL,
                         GTK_TYPE_SORTER,
                         G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnView:single-click-activate:
   *
   * Activate rows on single click and select them on hover.
   */
  properties[PROP_SINGLE_CLICK_ACTIVATE] =
    g_param_spec_boolean ("single-click-activate", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkColumnView:tab-behavior:
   *
   * Behavior of the <kbd>Tab</kbd> key
   *
   * Since: 4.12
   */
  properties[PROP_TAB_BEHAVIOR] =
    g_param_spec_enum ("tab-behavior", NULL, NULL,
                       GTK_TYPE_LIST_TAB_BEHAVIOR,
                       GTK_LIST_TAB_ALL,
                       G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkColumnView:header-factory:
   *
   * Factory for creating header widgets.
   *
   * The factory must be for configuring [class@Gtk.ListHeader] objects.
   *
   * Since: 4.12
   */
  properties[PROP_HEADER_FACTORY] =
    g_param_spec_object ("header-factory", NULL, NULL,
                         GTK_TYPE_LIST_ITEM_FACTORY,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);

  /**
   * GtkColumnView::activate:
   * @this: The columnview
   * @position: position of item to activate
   *
   * Emitted when a row has been activated by the user, usually via activating
   * the GtkListBase|list.activate-item action.
   *
   * This allows for a convenient way to handle activation in a columnview.
   * See [method@Gtk.ListItem.set_activatable] for details on how to use this
   * signal.
   */
  signals[ACTIVATE] =
    g_signal_new (I_("activate"),
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__UINT,
                  G_TYPE_NONE, 1,
                  G_TYPE_UINT);
  g_signal_set_va_marshaller (signals[ACTIVATE],
                              G_TYPE_FROM_CLASS (gobject_class),
                              g_cclosure_marshal_VOID__UINTv);

  gtk_widget_class_set_css_name (widget_class, I_("columnview"));
  gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_TREE_GRID);
}

static void update_column_resize  (GtkColumnView *this,
                                   double         x);
static void update_column_reorder (GtkColumnView *this,
                                   double         x);

static gboolean
autoscroll_cb (GtkWidget     *widget,
               GdkFrameClock *frame_clock,
               gpointer       data)
{
  GtkColumnView *this = data;

  gtk_adjustment_set_value (this->hadjustment,
                            gtk_adjustment_get_value (this->hadjustment) + this->autoscroll_delta);

  this->autoscroll_x += this->autoscroll_delta;

  if (this->in_column_resize)
    update_column_resize (this, this->autoscroll_x);
  else if (this->in_column_reorder)
    update_column_reorder (this, this->autoscroll_x);

  return G_SOURCE_CONTINUE;
}

static void
add_autoscroll (GtkColumnView *this,
                double         x,
                double         delta)
{
  this->autoscroll_x = x;
  this->autoscroll_delta = delta;

  if (this->autoscroll_id == 0)
    this->autoscroll_id = gtk_widget_add_tick_callback (GTK_WIDGET (this), autoscroll_cb, this, NULL);
}

static void
remove_autoscroll (GtkColumnView *this)
{
  if (this->autoscroll_id != 0)
    {
      gtk_widget_remove_tick_callback (GTK_WIDGET (this), this->autoscroll_id);
      this->autoscroll_id = 0;
    }
}

#define SCROLL_EDGE_SIZE 30

static void
update_autoscroll (GtkColumnView *this,
                   double         x)
{
  double width;
  double delta;
  graphene_point_t v;

  /* x is in header coordinates */
  if (!gtk_widget_compute_point (this->header, GTK_WIDGET (this),
                                 &GRAPHENE_POINT_INIT (x, 0),
                                 &v))
    graphene_point_init (&v, 0, 0);
  width = gtk_widget_get_width (GTK_WIDGET (this));

  if (v.x < SCROLL_EDGE_SIZE)
    delta = - (SCROLL_EDGE_SIZE - v.x)/3.0;
  else if (width - v.x < SCROLL_EDGE_SIZE)
    delta = (SCROLL_EDGE_SIZE - (width - v.x))/3.0;
  else
    delta = 0;

  if (gtk_widget_get_direction (GTK_WIDGET (this)) == GTK_TEXT_DIR_RTL)
    delta = - delta;

  if (delta != 0)
    add_autoscroll (this, x, delta);
  else
    remove_autoscroll (this);
}


#define DRAG_WIDTH 8

static gboolean
gtk_column_view_in_resize_rect (GtkColumnView       *this,
                                GtkColumnViewColumn *column,
                                double               x,
                                double               y)
{
  GtkWidget *header;
  graphene_rect_t rect;
  int width;

  header = gtk_column_view_column_get_header (column);

  if (!gtk_widget_compute_bounds (header, this->header, &rect))
    return FALSE;

  gtk_column_view_column_get_allocation (column, NULL, &width);
  rect.size.width = width;

  if (_gtk_widget_get_direction (GTK_WIDGET (this)) != GTK_TEXT_DIR_RTL)
    rect.origin.x += rect.size.width;
  rect.origin.x -= DRAG_WIDTH / 2;
  rect.size.width = DRAG_WIDTH;

  return graphene_rect_contains_point (&rect, &(graphene_point_t) { x, y});
}

static gboolean
gtk_column_view_in_header (GtkColumnView       *this,
                           GtkColumnViewColumn *column,
                           double               x,
                           double               y)
{
  GtkWidget *header;
  graphene_rect_t rect;

  header = gtk_column_view_column_get_header (column);

  if (!gtk_widget_compute_bounds (header, this->header, &rect))
    return FALSE;

  return graphene_rect_contains_point (&rect, &(graphene_point_t) { x, y});
}

static void
set_resize_cursor (GtkColumnView *this,
                   gboolean       set)
{
  int i, n;

  n = g_list_model_get_n_items (G_LIST_MODEL (this->columns));
  for (i = 0; i < n; i++)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);
      GtkWidget *header = gtk_column_view_column_get_header (column);
      if (set)
        gtk_widget_set_cursor_from_name (header, "col-resize");
      else
        gtk_widget_set_cursor (header, NULL);
      g_object_unref (column);
    }
}

static void
header_drag_begin (GtkGestureDrag *gesture,
                   double          start_x,
                   double          start_y,
                   GtkColumnView  *this)
{
  int i, n;

  this->drag_pos = -1;

  n = g_list_model_get_n_items (G_LIST_MODEL (this->columns));
  for (i = n - 1; !this->in_column_resize && i >= 0; i--)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      if (!gtk_column_view_column_get_visible (column))
        {
          g_object_unref (column);
          continue;
        }

      if (i + 1 < n &&
          gtk_column_view_column_get_resizable (column) &&
          gtk_column_view_in_resize_rect (this, column, start_x, start_y))
        {
          int size;

          gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);
          if (!gtk_widget_has_focus (GTK_WIDGET (this)))
            gtk_widget_grab_focus (GTK_WIDGET (this));

          gtk_column_view_column_get_allocation (column, NULL, &size);
          gtk_column_view_column_set_fixed_width (column, size);

          this->drag_pos = i;
          if (_gtk_widget_get_direction (GTK_WIDGET (this)) != GTK_TEXT_DIR_RTL)
            this->drag_x = start_x - size;
          else
            {
              int width = gtk_widget_get_width (GTK_WIDGET (this->header));

              this->drag_x = width - (start_x + size);
            }
          this->in_column_resize = TRUE;

          set_resize_cursor (this, TRUE);

          g_object_unref (column);

          break;
        }

      g_object_unref (column);
    }

  for (i = 0; !this->in_column_resize && i < n; i++)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      if (!gtk_column_view_column_get_visible (column))
        {
          g_object_unref (column);
          continue;
        }

      if (gtk_column_view_get_reorderable (this) &&
          gtk_column_view_in_header (this, column, start_x, start_y))
        {
          int pos;

          gtk_column_view_column_get_allocation (column, &pos, NULL);

          this->drag_pos = i;
          this->drag_offset = start_x - pos;

          g_object_unref (column);
          break;
        }

      g_object_unref (column);
    }
}

static void
header_drag_end (GtkGestureDrag *gesture,
                 double          offset_x,
                 double          offset_y,
                 GtkColumnView  *this)
{
  double start_x, x;

  gtk_gesture_drag_get_start_point (gesture, &start_x, NULL);
  x = start_x + offset_x;

  remove_autoscroll (this);

  if (this->in_column_resize)
    {
      set_resize_cursor (this, FALSE);
      this->in_column_resize = FALSE;
    }
  else if (this->in_column_reorder)
    {
      GdkEventSequence *sequence;
      GtkColumnViewColumn *column;
      GtkWidget *header;
      int i;

      this->in_column_reorder = FALSE;

      if (this->drag_pos == -1)
        return;

      column = g_list_model_get_item (G_LIST_MODEL (this->columns), this->drag_pos);
      header = gtk_column_view_column_get_header (column);
      gtk_widget_remove_css_class (header, "dnd");

      sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));
      if (!gtk_gesture_handles_sequence (GTK_GESTURE (gesture), sequence))
        {
          g_object_unref (column);
          return;
        }

      for (i = 0; i < g_list_model_get_n_items (G_LIST_MODEL (this->columns)); i++)
        {
          GtkColumnViewColumn *col = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

          if (gtk_column_view_column_get_visible (col))
            {
              int pos, size;

              gtk_column_view_column_get_allocation (col, &pos, &size);
              if (pos <= x && x <= pos + size)
                {
                  gtk_column_view_insert_column (this, i, column);
                  g_object_unref (col);
                  break;
                }
            }

          g_object_unref (col);
        }

      g_object_unref (column);
    }
}

static void
update_column_resize (GtkColumnView *this,
                      double         x)
{
  GtkColumnViewColumn *column;

  if (_gtk_widget_get_direction (GTK_WIDGET (this)) == GTK_TEXT_DIR_RTL)
    {
      int width = gtk_widget_get_width (GTK_WIDGET (this->header));

      x = width - x;
    }

  column = g_list_model_get_item (G_LIST_MODEL (this->columns), this->drag_pos);
  gtk_column_view_column_set_fixed_width (column, MAX (x - this->drag_x, 0));
  g_object_unref (column);
}

static void
update_column_reorder (GtkColumnView *this,
                       double         x)
{
  GtkColumnViewColumn *column;
  int width;
  int size;

  column = g_list_model_get_item (G_LIST_MODEL (this->columns), this->drag_pos);
  width = gtk_widget_get_width (GTK_WIDGET (this->header));
  gtk_column_view_column_get_allocation (column, NULL, &size);

  this->drag_x = CLAMP (x - this->drag_offset, 0, width - size);

  gtk_widget_queue_allocate (GTK_WIDGET (this));
  gtk_column_view_column_queue_resize (column);
  g_object_unref (column);
}

static void
header_drag_update (GtkGestureDrag *gesture,
                    double          offset_x,
                    double          offset_y,
                    GtkColumnView  *this)
{
  GdkEventSequence *sequence;
  double start_x, x;

  sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));
  if (!gtk_gesture_handles_sequence (GTK_GESTURE (gesture), sequence))
    return;

  if (this->drag_pos == -1)
    return;

  if (!this->in_column_resize && !this->in_column_reorder)
    {
      if (gtk_drag_check_threshold_double (GTK_WIDGET (this), 0, 0, offset_x, 0))
        {
          GtkColumnViewColumn *column;
          GtkWidget *header;

          column = g_list_model_get_item (G_LIST_MODEL (this->columns), this->drag_pos);
          header = gtk_column_view_column_get_header (column);

          gtk_widget_insert_after (header, this->header, gtk_widget_get_last_child (this->header));
          gtk_widget_add_css_class (header, "dnd");

          gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);
          if (!gtk_widget_has_focus (GTK_WIDGET (this)))
            gtk_widget_grab_focus (GTK_WIDGET (this));

          this->in_column_reorder = TRUE;

          g_object_unref (column);
        }
    }

  gtk_gesture_drag_get_start_point (gesture, &start_x, NULL);
  x = start_x + offset_x;

  if (this->in_column_resize)
    update_column_resize (this, x);
  else if (this->in_column_reorder)
    update_column_reorder (this, x);

  if (this->in_column_resize || this->in_column_reorder)
    update_autoscroll (this, x);
}

static void
header_motion (GtkEventControllerMotion *controller,
               double                    x,
               double                    y,
               GtkColumnView            *this)
{
  gboolean cursor_set = FALSE;
  int i, n;

  if (this->in_column_resize)
    return;

  n = g_list_model_get_n_items (G_LIST_MODEL (this->columns));
  for (i = 0; i < n; i++)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      if (!gtk_column_view_column_get_visible (column))
        {
          g_object_unref (column);
          continue;
        }

      if (i + 1 < n &&
          gtk_column_view_column_get_resizable (column) &&
          gtk_column_view_in_resize_rect (this, column, x, y))
        {
          gtk_widget_set_cursor_from_name (this->header, "col-resize");
          cursor_set = TRUE;
        }

      g_object_unref (column);
    }

  if (!cursor_set)
    gtk_widget_set_cursor (this->header, NULL);
}

static gboolean
header_key_pressed (GtkEventControllerKey *controller,
                    guint                  keyval,
                    guint                  keycode,
                    GdkModifierType        modifiers,
                    GtkColumnView         *this)
{
  if (this->in_column_reorder)
    {
      if (keyval == GDK_KEY_Escape)
        gtk_gesture_set_state (this->drag_gesture, GTK_EVENT_SEQUENCE_DENIED);
      return TRUE;
    }

  return FALSE;
}

static void
header_pressed (GtkGestureClick *gesture,
                int              n_press,
                double           x,
                double           y,
                GtkColumnView   *this)
{
  int i, n;

  if (n_press != 2)
    return;

  n = g_list_model_get_n_items (G_LIST_MODEL (this->columns));
  for (i = n - 1; i >= 0; i--)
    {
      GtkColumnViewColumn *column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      g_object_unref (column);

      if (i + 1 < n &&
          gtk_column_view_column_get_resizable (column) &&
          gtk_column_view_in_resize_rect (this, column, x, y))
        {
          gtk_gesture_set_state (this->drag_gesture, GTK_EVENT_SEQUENCE_DENIED);
          gtk_column_view_column_set_fixed_width (column, -1);
          break;
        }
    }
}

static void
gtk_column_view_drag_motion (GtkDropControllerMotion *motion,
                             double                   x,
                             double                   y,
                             gpointer                 unused)
{
  GtkWidget *widget = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (motion));
  GtkColumnView *this = GTK_COLUMN_VIEW (widget);
  graphene_point_t h;

  if (!gtk_widget_compute_point (widget, this->header,
                                 &GRAPHENE_POINT_INIT (x, 0), &h))
    graphene_point_init (&h, 0, 0);
  update_autoscroll (GTK_COLUMN_VIEW (widget), h.x);
}

static void
gtk_column_view_drag_leave (GtkDropControllerMotion *motion,
                            gpointer                 unused)
{
  GtkWidget *widget = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (motion));

  remove_autoscroll (GTK_COLUMN_VIEW (widget));
}

static void
gtk_column_view_init (GtkColumnView *this)
{
  GtkEventController *controller;

  this->columns = g_list_store_new (GTK_TYPE_COLUMN_VIEW_COLUMN);

  this->header = gtk_column_view_row_widget_new (NULL, TRUE);
  gtk_widget_set_can_focus (this->header, FALSE);
  gtk_widget_set_parent (this->header, GTK_WIDGET (this));

  controller = GTK_EVENT_CONTROLLER (gtk_gesture_click_new ());
  g_signal_connect (controller, "pressed", G_CALLBACK (header_pressed), this);
  gtk_event_controller_set_propagation_phase (controller, GTK_PHASE_CAPTURE);
  gtk_widget_add_controller (this->header, controller);

  controller = GTK_EVENT_CONTROLLER (gtk_gesture_drag_new ());
  g_signal_connect (controller, "drag-begin", G_CALLBACK (header_drag_begin), this);
  g_signal_connect (controller, "drag-update", G_CALLBACK (header_drag_update), this);
  g_signal_connect (controller, "drag-end", G_CALLBACK (header_drag_end), this);
  gtk_event_controller_set_propagation_phase (controller, GTK_PHASE_CAPTURE);
  gtk_widget_add_controller (this->header, controller);
  this->drag_gesture = GTK_GESTURE (controller);

  controller = gtk_event_controller_motion_new ();
  g_signal_connect (controller, "motion", G_CALLBACK (header_motion), this);
  gtk_widget_add_controller (this->header, controller);

  controller = gtk_event_controller_key_new ();
  g_signal_connect (controller, "key-pressed", G_CALLBACK (header_key_pressed), this);
  gtk_widget_add_controller (GTK_WIDGET (this), controller);

  controller = gtk_drop_controller_motion_new ();
  g_signal_connect (controller, "motion", G_CALLBACK (gtk_column_view_drag_motion), NULL);
  g_signal_connect (controller, "leave", G_CALLBACK (gtk_column_view_drag_leave), NULL);
  gtk_widget_add_controller (GTK_WIDGET (this), controller);

  this->sorter = GTK_SORTER (gtk_column_view_sorter_new ());
  this->listview = GTK_LIST_VIEW (g_object_new (GTK_TYPE_COLUMN_LIST_VIEW, NULL));
  gtk_widget_set_hexpand (GTK_WIDGET (this->listview), TRUE);
  gtk_widget_set_vexpand (GTK_WIDGET (this->listview), TRUE);
  g_signal_connect (this->listview, "activate", G_CALLBACK (gtk_column_view_activate_cb), this);
  gtk_widget_set_parent (GTK_WIDGET (this->listview), GTK_WIDGET (this));

  gtk_css_node_add_class (gtk_widget_get_css_node (GTK_WIDGET (this)),
                          g_quark_from_static_string (I_("view")));

  gtk_widget_set_overflow (GTK_WIDGET (this), GTK_OVERFLOW_HIDDEN);

  this->reorderable = TRUE;
}

/**
 * gtk_column_view_new:
 * @model: (nullable) (transfer full): the list model to use
 *
 * Creates a new `GtkColumnView`.
 *
 * You most likely want to call [method@Gtk.ColumnView.append_column]
 * to add columns next.
 *
 * Returns: a new `GtkColumnView`
 **/
GtkWidget *
gtk_column_view_new (GtkSelectionModel *model)
{
  GtkWidget *result;

  g_return_val_if_fail (model == NULL || GTK_IS_SELECTION_MODEL (model), NULL);

  result = g_object_new (GTK_TYPE_COLUMN_VIEW,
                         "model", model,
                         NULL);

  /* consume the reference */
  g_clear_object (&model);

  return result;
}

/**
 * gtk_column_view_get_model:
 * @this: a columnview
 *
 * Gets the model that's currently used to read the items displayed.
 *
 * Returns: (nullable) (transfer none): The model in use
 */
GtkSelectionModel *
gtk_column_view_get_model (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), NULL);

  return gtk_list_view_get_model (this->listview);
}

/**
 * gtk_column_view_set_model:
 * @this: a columnview
 * @model: (nullable) (transfer none): the model to use
 *
 * Sets the model to use.
 *
 * This must be a [iface@Gtk.SelectionModel].
 */
void
gtk_column_view_set_model (GtkColumnView     *this,
                           GtkSelectionModel *model)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));
  g_return_if_fail (model == NULL || GTK_IS_SELECTION_MODEL (model));

  if (gtk_list_view_get_model (this->listview) == model)
    return;

  gtk_list_view_set_model (this->listview, model);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODEL]);
}

/**
 * gtk_column_view_get_columns:
 * @this: a columnview
 *
 * Gets the list of columns in this column view.
 *
 * This list is constant over the lifetime of @this and can be used to
 * monitor changes to the columns of @this by connecting to the
 * [signal@Gio.ListModel::items-changed] signal.
 *
 * Returns: (transfer none): The list managing the columns
 */
GListModel *
gtk_column_view_get_columns (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), NULL);

  return G_LIST_MODEL (this->columns);
}

/**
 * gtk_column_view_set_show_row_separators:
 * @this: a columnview
 * @show_row_separators: whether to show row separators
 *
 * Sets whether the list should show separators between rows.
 */
void
gtk_column_view_set_show_row_separators (GtkColumnView *this,
                                         gboolean       show_row_separators)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));

  if (gtk_list_view_get_show_separators (this->listview) == show_row_separators)
    return;

  gtk_list_view_set_show_separators (this->listview, show_row_separators);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SHOW_ROW_SEPARATORS]);
}

/**
 * gtk_column_view_get_show_row_separators:
 * @this: a columnview
 *
 * Returns whether the list should show separators between rows.
 *
 * Returns: true if the list shows separators
 */
gboolean
gtk_column_view_get_show_row_separators (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), FALSE);

  return gtk_list_view_get_show_separators (this->listview);
}

/**
 * gtk_column_view_set_show_column_separators:
 * @this: a columnview
 * @show_column_separators: whether to show column separators
 *
 * Sets whether the list should show separators between columns.
 */
void
gtk_column_view_set_show_column_separators (GtkColumnView *this,
                                            gboolean       show_column_separators)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));

  if (this->show_column_separators == show_column_separators)
    return;

  this->show_column_separators = show_column_separators;

  if (show_column_separators)
    gtk_widget_add_css_class (GTK_WIDGET (this), "column-separators");
  else
    gtk_widget_remove_css_class (GTK_WIDGET (this), "column-separators");

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SHOW_COLUMN_SEPARATORS]);
}

/**
 * gtk_column_view_get_show_column_separators:
 * @this: a columnview
 *
 * Returns whether the list should show separators between columns.
 *
 * Returns: true if the list shows column separators
 */
gboolean
gtk_column_view_get_show_column_separators (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), FALSE);

  return this->show_column_separators;
}

/**
 * gtk_column_view_append_column:
 * @this: a columnview
 * @column: a column that hasn't been added to a `GtkColumnView` yet
 *
 * Appends the @column to the end of the columns in @this.
 */
void
gtk_column_view_append_column (GtkColumnView       *this,
                               GtkColumnViewColumn *column)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (column));
  g_return_if_fail (gtk_column_view_column_get_column_view (column) == NULL);

  gtk_column_view_column_set_column_view (column, this);
  g_list_store_append (this->columns, column);
}

/**
 * gtk_column_view_remove_column:
 * @this: a columnview
 * @column: a column that's part of @this
 *
 * Removes the @column from the list of columns of @this.
 */
void
gtk_column_view_remove_column (GtkColumnView       *this,
                               GtkColumnViewColumn *column)
{
  guint i;

  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (column));
  g_return_if_fail (gtk_column_view_column_get_column_view (column) == this);

  for (i = 0; i < g_list_model_get_n_items (G_LIST_MODEL (this->columns)); i++)
    {
      GtkColumnViewColumn *item = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      g_object_unref (item);
      if (item == column)
        break;
    }

  gtk_column_view_sorter_remove_column (GTK_COLUMN_VIEW_SORTER (this->sorter), column);
  gtk_column_view_column_set_column_view (column, NULL);
  g_list_store_remove (this->columns, i);

  if (this->focus_column == column)
    {
      GtkColumnViewColumn *item;

      if (i < g_list_model_get_n_items (G_LIST_MODEL (this->columns)))
        item = g_list_model_get_item (G_LIST_MODEL (this->columns), i);
      else if (i > 0)
        item = g_list_model_get_item (G_LIST_MODEL (this->columns), i - 1);
      else
        item = NULL;

      gtk_column_view_set_focus_column (this, item, TRUE);
    }
}

/**
 * gtk_column_view_insert_column:
 * @this: a columnview
 * @position: the position to insert @column at
 * @column: the column to insert
 *
 * Inserts a column at the given position in the columns of @this.
 *
 * If @column is already a column of @this, it will be repositioned.
 */
void
gtk_column_view_insert_column (GtkColumnView       *this,
                               guint                position,
                               GtkColumnViewColumn *column)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (column));
  g_return_if_fail (gtk_column_view_column_get_column_view (column) == NULL ||
                    gtk_column_view_column_get_column_view (column) == this);
  g_return_if_fail (position <= g_list_model_get_n_items (G_LIST_MODEL (this->columns)));
  int old_position = -1;

  g_object_ref (column);

  if (gtk_column_view_column_get_column_view (column) == this)
    {
      guint i;

      for (i = 0; i < g_list_model_get_n_items (G_LIST_MODEL (this->columns)); i++)
        {
          GtkColumnViewColumn *item = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

          g_object_unref (item);
          if (item == column)
            {
              old_position = i;
              g_list_store_remove (this->columns, i);
              break;
            }
        }
    }

  g_list_store_insert (this->columns, position, column);

  gtk_column_view_column_set_column_view (column, this);

  if (old_position != -1 && position != old_position)
    gtk_column_view_column_set_position (column, position);

  gtk_column_view_column_queue_resize (column);

  g_object_unref (column);
}

static void
gtk_column_view_scroll_to_column (GtkColumnView       *this,
                                  GtkColumnViewColumn *column,
                                  GtkScrollInfo       *scroll_info)
{
  int col_x, col_width, new_value;

  gtk_column_view_column_get_header_allocation (column, &col_x, &col_width);

  new_value = gtk_scroll_info_compute_for_orientation (scroll_info,
                                                       GTK_ORIENTATION_HORIZONTAL,
                                                       col_x,
                                                       col_width,
                                                       gtk_adjustment_get_value (this->hadjustment),
                                                       gtk_adjustment_get_page_size (this->hadjustment));

  gtk_adjustment_set_value (this->hadjustment, new_value);

  g_clear_pointer (&scroll_info, gtk_scroll_info_unref);
}

void
gtk_column_view_set_focus_column (GtkColumnView       *this,
                                  GtkColumnViewColumn *column,
                                  gboolean             scroll)
{
  g_assert (column == NULL || gtk_column_view_column_get_column_view (column) == this);

  if (this->focus_column == column)
    return;

  this->focus_column = column;

  if (column && scroll)
    gtk_column_view_scroll_to_column (this, column, NULL);
}

GtkColumnViewColumn *
gtk_column_view_get_focus_column (GtkColumnView *this)
{
  return this->focus_column;
}

void
gtk_column_view_measure_across (GtkColumnView *this,
                                int           *minimum,
                                int           *natural)
{
  guint i;
  int min, nat;

  min = 0;
  nat = 0;

  for (i = 0; i < g_list_model_get_n_items (G_LIST_MODEL (this->columns)); i++)
    {
      GtkColumnViewColumn *column;
      int col_min, col_nat;

      column = g_list_model_get_item (G_LIST_MODEL (this->columns), i);

      if (gtk_column_view_column_get_visible (column))
        {
          gtk_column_view_column_measure (column, &col_min, &col_nat);
          min += col_min;
          nat += col_nat;
        }

      g_object_unref (column);
    }

  *minimum = min;
  *natural = nat;
}

GtkColumnViewRowWidget *
gtk_column_view_get_header_widget (GtkColumnView *this)
{
  return GTK_COLUMN_VIEW_ROW_WIDGET (this->header);
}

GtkListView *
gtk_column_view_get_list_view (GtkColumnView *this)
{
  return GTK_LIST_VIEW (this->listview);
}

/**
 * gtk_column_view_get_sorter:
 * @this: a columnview
 *
 * Returns a special sorter that reflects the users sorting
 * choices in the column view.
 *
 * To allow users to customizable sorting by clicking on column
 * headers, this sorter needs to be set on the sort model underneath
 * the model that is displayed by the view.
 *
 * See [method@Gtk.ColumnViewColumn.set_sorter] for setting up
 * per-column sorting.
 *
 * Here is an example:
 * ```c
 * gtk_column_view_column_set_sorter (column, sorter);
 * gtk_column_view_append_column (view, column);
 * sorter = g_object_ref (gtk_column_view_get_sorter (view)));
 * model = gtk_sort_list_model_new (store, sorter);
 * selection = gtk_no_selection_new (model);
 * gtk_column_view_set_model (view, selection);
 * ```
 *
 * Returns: (nullable) (transfer none): the `GtkSorter` of @this
 */
GtkSorter *
gtk_column_view_get_sorter (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), NULL);

  return this->sorter;
}

/**
 * gtk_column_view_sort_by_column:
 * @this: a columnview
 * @column: (nullable): the column to sort by
 * @direction: the direction to sort in
 *
 * Sets the sorting of the view.
 *
 * This function should be used to set up the initial sorting.
 * At runtime, users can change the sorting of a column view
 * by clicking on the list headers.
 *
 * This call only has an effect if the sorter returned by
 * [method@Gtk.ColumnView.get_sorter] is set on a sort model,
 * and [method@Gtk.ColumnViewColumn.set_sorter] has been called
 * on @column to associate a sorter with the column.
 *
 * If @column is unset, the view will be unsorted.
 */
void
gtk_column_view_sort_by_column (GtkColumnView       *this,
                                GtkColumnViewColumn *column,
                                GtkSortType          direction)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));
  g_return_if_fail (column == NULL || GTK_IS_COLUMN_VIEW_COLUMN (column));
  g_return_if_fail (column == NULL || gtk_column_view_column_get_column_view (column) == this);

  if (column == NULL)
    gtk_column_view_sorter_clear (GTK_COLUMN_VIEW_SORTER (this->sorter));
  else
    gtk_column_view_sorter_set_column (GTK_COLUMN_VIEW_SORTER (this->sorter),
                                       column,
                                       direction == GTK_SORT_DESCENDING);
}

/**
 * gtk_column_view_set_single_click_activate:
 * @this: a columnview
 * @single_click_activate: whether to activate items on single click
 *
 * Sets whether rows should be activated on single click and
 * selected on hover.
 */
void
gtk_column_view_set_single_click_activate (GtkColumnView *this,
                                           gboolean       single_click_activate)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));

  if (single_click_activate == gtk_list_view_get_single_click_activate (this->listview))
    return;

  gtk_list_view_set_single_click_activate (this->listview, single_click_activate);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SINGLE_CLICK_ACTIVATE]);
}

/**
 * gtk_column_view_get_single_click_activate:
 * @this: a columnview
 *
 * Returns whether rows will be activated on single click and
 * selected on hover.
 *
 * Returns: true if rows are activated on single click
 */
gboolean
gtk_column_view_get_single_click_activate (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), FALSE);

  return gtk_list_view_get_single_click_activate (this->listview);
}

/**
 * gtk_column_view_set_reorderable:
 * @this: a columnview
 * @reorderable: whether columns should be reorderable
 *
 * Sets whether columns should be reorderable by dragging.
 */
void
gtk_column_view_set_reorderable (GtkColumnView *this,
                                 gboolean       reorderable)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));

  if (this->reorderable == reorderable)
    return;

  this->reorderable = reorderable;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_REORDERABLE]);
}

/**
 * gtk_column_view_get_reorderable:
 * @this: a columnview
 *
 * Returns whether columns are reorderable.
 *
 * Returns: true if columns are reorderable
 */
gboolean
gtk_column_view_get_reorderable (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), TRUE);

  return this->reorderable;
}

/**
 * gtk_column_view_set_enable_rubberband:
 * @this: a columnview
 * @enable_rubberband: whether to enable rubberband selection
 *
 * Sets whether selections can be changed by dragging with the mouse.
 */
void
gtk_column_view_set_enable_rubberband (GtkColumnView *this,
                                       gboolean       enable_rubberband)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));

  if (enable_rubberband == gtk_list_view_get_enable_rubberband (this->listview))
    return;

  gtk_list_view_set_enable_rubberband (this->listview, enable_rubberband);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ENABLE_RUBBERBAND]);
}

/**
 * gtk_column_view_get_enable_rubberband:
 * @this: a columnview
 *
 * Returns whether rows can be selected by dragging with the mouse.
 *
 * Returns: true if rubberband selection is enabled
 */
gboolean
gtk_column_view_get_enable_rubberband (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), FALSE);

  return gtk_list_view_get_enable_rubberband (this->listview);
}

/**
 * gtk_column_view_set_row_factory:
 * @this: a columnview
 * @factory: (nullable): The row factory
 *
 * Sets the factory used for configuring rows.
 *
 * The factory must be for configuring [class@Gtk.ColumnViewRow] objects.
 *
 * If this factory is not set - which is the default - then the defaults
 * will be used.
 *
 * This factory is not used to set the widgets displayed in the individual
 * cells. For that see [method@GtkColumnViewColumn.set_factory] and
 * [class@GtkColumnViewCell].
 *
 * Since: 4.12
 */
void
gtk_column_view_set_row_factory (GtkColumnView      *this,
                                 GtkListItemFactory *factory)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));

  if (factory == gtk_list_view_get_factory (this->listview))
    return;

  gtk_list_view_set_factory (this->listview, factory);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ROW_FACTORY]);
}

/**
 * gtk_column_view_get_row_factory:
 * @this: a columnview
 *
 * Gets the factory set via [method@Gtk.ColumnView.set_row_factory].
 *
 * Returns: (nullable) (transfer none): The factory
 *
 * Since: 4.12
 */
GtkListItemFactory *
gtk_column_view_get_row_factory (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), FALSE);

  return gtk_list_view_get_factory (this->listview);
}

/**
 * gtk_column_view_set_tab_behavior:
 * @this: a columnview
 * @tab_behavior: The desired tab behavior
 *
 * Sets the <kbd>Tab</kbd> key behavior.
 *
 * This influences how the <kbd>Tab</kbd> and
 * <kbd>Shift</kbd>+<kbd>Tab</kbd> keys move the
 * focus in the columnview.
 *
 * Since: 4.12
 */
void
gtk_column_view_set_tab_behavior (GtkColumnView      *this,
                                  GtkListTabBehavior  tab_behavior)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));

  if (tab_behavior == gtk_list_view_get_tab_behavior (this->listview))
    return;

  gtk_list_view_set_tab_behavior (this->listview, tab_behavior);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_TAB_BEHAVIOR]);
}

/**
 * gtk_column_view_get_tab_behavior:
 * @this: a columnview
 *
 * Gets the behavior set for the <kbd>Tab</kbd> key.
 *
 * Returns: The behavior of the <kbd>Tab</kbd> key
 *
 * Since: 4.12
 */
GtkListTabBehavior
gtk_column_view_get_tab_behavior (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), FALSE);

  return gtk_list_view_get_tab_behavior (this->listview);
}

/**
 * gtk_column_view_get_header_factory:
 * @this: a columnview
 *
 * Gets the factory that's currently used to populate section headers.
 *
 * Returns: (nullable) (transfer none): The factory in use
 *
 * Since: 4.12
 */
GtkListItemFactory *
gtk_column_view_get_header_factory (GtkColumnView *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW (this), NULL);

  return gtk_list_view_get_header_factory (this->listview);
}

/**
 * gtk_column_view_set_header_factory:
 * @this: a columnview
 * @factory: (nullable) (transfer none): the factory to use
 *
 * Sets the factory to use for populating the
 * [class@Gtk.ListHeader] objects used in section headers.
 *
 * If this factory is set to `NULL`, the list will not show
 * section headers.
 *
 * Since: 4.12
 */
void
gtk_column_view_set_header_factory (GtkColumnView      *this,
                                    GtkListItemFactory *factory)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));
  g_return_if_fail (factory == NULL || GTK_IS_LIST_ITEM_FACTORY (factory));

  if (factory == gtk_list_view_get_header_factory (this->listview))
    return;

  gtk_list_view_set_header_factory (this->listview, factory);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_HEADER_FACTORY]);
}

/**
 * gtk_column_view_scroll_to:
 * @this: The columnview
 * @pos: position of the item. Must be less than the number of
 *   items in the view.
 * @column: (nullable) (transfer none): The column to scroll to
 *   or `NULL` to not scroll columns
 * @flags: actions to perform
 * @scroll: (nullable) (transfer full): details of how to perform
 *   the scroll operation or %NULL to scroll into view
 *
 * Scroll to the row at the given position - or cell if a column is
 * given - and performs the actions specified in @flags.
 *
 * This function works no matter if the columnview is shown or focused.
 * If it isn't, then the changes will take effect once that happens.
 *
 * Since: 4.12
 */
void
gtk_column_view_scroll_to (GtkColumnView       *this,
                           guint                pos,
                           GtkColumnViewColumn *column,
                           GtkListScrollFlags   flags,
                           GtkScrollInfo       *scroll)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW (this));
  g_return_if_fail (pos < gtk_list_base_get_n_items (GTK_LIST_BASE (this->listview)));
  g_return_if_fail (column == NULL || GTK_IS_COLUMN_VIEW_COLUMN (column));
  if (column)
    {
      g_return_if_fail (gtk_column_view_column_get_column_view (column) == this);
    }

  if (column && (flags & GTK_LIST_SCROLL_FOCUS))
    gtk_column_view_set_focus_column (this, column, FALSE);

  gtk_list_view_scroll_to (this->listview,
                           pos,
                           flags,
                           scroll ? gtk_scroll_info_ref (scroll) : NULL);

  if (column)
    gtk_column_view_scroll_to_column (this, column, scroll);
  else
    g_clear_pointer (&scroll, gtk_scroll_info_unref);
}
