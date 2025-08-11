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

#include "gtkcolumnviewcolumnprivate.h"
#include "gtkcolumnviewsorterprivate.h"

#include "gtkcolumnviewprivate.h"
#include "gtkcolumnviewrowwidgetprivate.h"
#include "gtkcolumnviewtitleprivate.h"
#include "gtklistbaseprivate.h"
#include "gtkmain.h"
#include "gtkprivate.h"
#include "gtkrbtreeprivate.h"
#include "gtksizegroup.h"
#include "gtkwidgetprivate.h"
#include "gtksorter.h"

/**
 * GtkColumnViewColumn:
 *
 * Represents the columns in a `GtkColumnView`.
 *
 * The main ingredient for a `GtkColumnViewColumn` is the `GtkListItemFactory`
 * that tells the columnview how to create cells for this column from items in
 * the model.
 *
 * Columns have a title, and can optionally have a header menu set
 * with [method@Gtk.ColumnViewColumn.set_header_menu].
 *
 * A sorter can be associated with a column using
 * [method@Gtk.ColumnViewColumn.set_sorter], to immutable users influence sorting
 * by clicking on the column header.
 */

struct _GtkColumnViewColumn
{
  GObject parent_instance;

  GtkListItemFactory *factory;
  char *title;
  char *id;
  GtkSorter *sorter;

  /* data for the view */
  GtkColumnView *view;
  GtkWidget *header;

  int minimum_size_request;
  int natural_size_request;
  int allocation_offset;
  int allocation_size;
  int header_position;

  int fixed_width;

  guint visible     : 1;
  guint resizable   : 1;
  guint expand      : 1;

  GMenuModel *menu;

  /* This list isn't sorted - this is just caching for performance */
  GtkColumnViewCellWidget *first_cell; /* no reference, just caching */
};

struct _GtkColumnViewColumnClass
{
  GObjectClass parent_class;
};

enum
{
  PROP_0,
  PROP_COLUMN_VIEW,
  PROP_FACTORY,
  PROP_TITLE,
  PROP_SORTER,
  PROP_VISIBLE,
  PROP_HEADER_MENU,
  PROP_RESIZABLE,
  PROP_EXPAND,
  PROP_FIXED_WIDTH,
  PROP_ID,

  N_PROPS
};

G_DEFINE_TYPE (GtkColumnViewColumn, gtk_column_view_column, G_TYPE_OBJECT)

static GParamSpec *properties[N_PROPS] = { NULL, };

static void
gtk_column_view_column_dispose (GObject *object)
{
  GtkColumnViewColumn *this = GTK_COLUMN_VIEW_COLUMN (object);

  g_assert (this->view == NULL); /* would hold a ref otherwise */
  g_assert (this->first_cell == NULL); /* no view = no children */

  g_clear_object (&this->factory);
  g_clear_object (&this->sorter);
  g_clear_pointer (&this->title, g_free);
  g_clear_object (&this->menu);
  g_clear_pointer (&this->id, g_free);

  G_OBJECT_CLASS (gtk_column_view_column_parent_class)->dispose (object);
}

static void
gtk_column_view_column_get_property (GObject    *object,
                                     guint       property_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  GtkColumnViewColumn *this = GTK_COLUMN_VIEW_COLUMN (object);

  switch (property_id)
    {
    case PROP_COLUMN_VIEW:
      g_value_set_object (value, this->view);
      break;

    case PROP_FACTORY:
      g_value_set_object (value, this->factory);
      break;

    case PROP_TITLE:
      g_value_set_string (value, this->title);
      break;

    case PROP_SORTER:
      g_value_set_object (value, this->sorter);
      break;

    case PROP_VISIBLE:
      g_value_set_boolean (value, this->visible);
      break;

    case PROP_HEADER_MENU:
      g_value_set_object (value, this->menu);
      break;

    case PROP_RESIZABLE:
      g_value_set_boolean (value, this->resizable);
      break;

    case PROP_EXPAND:
      g_value_set_boolean (value, this->expand);
      break;

    case PROP_FIXED_WIDTH:
      g_value_set_int (value, this->fixed_width);
      break;

    case PROP_ID:
      g_value_set_string (value, this->id);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_column_view_column_set_property (GObject      *object,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  GtkColumnViewColumn *this = GTK_COLUMN_VIEW_COLUMN (object);

  switch (property_id)
    {
    case PROP_FACTORY:
      gtk_column_view_column_set_factory (this, g_value_get_object (value));
      break;

    case PROP_TITLE:
      gtk_column_view_column_set_title (this, g_value_get_string (value));
      break;

    case PROP_SORTER:
      gtk_column_view_column_set_sorter (this, g_value_get_object (value));
      break;

    case PROP_VISIBLE:
      gtk_column_view_column_set_visible (this, g_value_get_boolean (value));
      break;

    case PROP_HEADER_MENU:
      gtk_column_view_column_set_header_menu (this, g_value_get_object (value));
      break;

    case PROP_RESIZABLE:
      gtk_column_view_column_set_resizable (this, g_value_get_boolean (value));
      break;

    case PROP_EXPAND:
      gtk_column_view_column_set_expand (this, g_value_get_boolean (value));
      break;

    case PROP_FIXED_WIDTH:
      gtk_column_view_column_set_fixed_width (this, g_value_get_int (value));
      break;

    case PROP_ID:
      gtk_column_view_column_set_id (this, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_column_view_column_class_init (GtkColumnViewColumnClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = gtk_column_view_column_dispose;
  gobject_class->get_property = gtk_column_view_column_get_property;
  gobject_class->set_property = gtk_column_view_column_set_property;

  /**
   * GtkColumnViewColumn:column-view:
   *
   * The `GtkColumnView` this column is a part of.
   */
  properties[PROP_COLUMN_VIEW] =
    g_param_spec_object ("column-view", NULL, NULL,
                         GTK_TYPE_COLUMN_VIEW,
                         G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:factory:
   *
   * Factory for populating list items.
   *
   * The factory must be for configuring [class@Gtk.ColumnViewCell] objects.
   */
  properties[PROP_FACTORY] =
    g_param_spec_object ("factory", NULL, NULL,
                         GTK_TYPE_LIST_ITEM_FACTORY,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:title:
   *
   * Title displayed in the header.
   */
  properties[PROP_TITLE] =
    g_param_spec_string ("title", NULL, NULL,
                          NULL,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkColumnViewColumn:sorter:
   *
   * Sorter for sorting items according to this column.
   */
  properties[PROP_SORTER] =
    g_param_spec_object ("sorter", NULL, NULL,
                         GTK_TYPE_SORTER,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:visible:
   *
   * Whether this column is visible.
   */
  properties[PROP_VISIBLE] =
    g_param_spec_boolean ("visible", NULL, NULL,
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:header-menu:
   *
   * Menu model used to create the context menu for the column header.
   */
  properties[PROP_HEADER_MENU] =
    g_param_spec_object ("header-menu", NULL, NULL,
                         G_TYPE_MENU_MODEL,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:resizable:
   *
   * Whether this column is resizable.
   */
  properties[PROP_RESIZABLE] =
    g_param_spec_boolean ("resizable", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:expand:
   *
   * Column gets share of extra width allocated to the view.
   */
  properties[PROP_EXPAND] =
    g_param_spec_boolean ("expand", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:fixed-width:
   *
   * If not -1, this is the width that the column is allocated,
   * regardless of the size of its content.
   */
  properties[PROP_FIXED_WIDTH] =
    g_param_spec_int ("fixed-width", NULL, NULL,
                      -1, G_MAXINT, -1,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkColumnViewColumn:id:
   *
   * An ID for the column.
   *
   * GTK is not currently using the ID for anything, but
   * it can be used by applications when saving column view
   * configurations.
   *
   * It is up to applications to ensure uniqueness of IDs.
   *
   * Since: 4.10
   */
  properties[PROP_ID] =
    g_param_spec_string ("id", NULL, NULL,
                          NULL,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);
}

static void
gtk_column_view_column_init (GtkColumnViewColumn *this)
{
  this->minimum_size_request = -1;
  this->natural_size_request = -1;
  this->visible = TRUE;
  this->resizable = FALSE;
  this->expand = FALSE;
  this->fixed_width = -1;
}

/**
 * gtk_column_view_column_new:
 * @title: (nullable): Title to use for this column
 * @factory: (transfer full) (nullable): The factory to populate items with
 *
 * Creates a new `GtkColumnViewColumn` that uses the given @factory for
 * mapping items to widgets.
 *
 * You most likely want to call [method@Gtk.ColumnView.append_column] next.
 *
 * The function takes ownership of the argument, so you can write code like:
 *
 * ```c
 * column = gtk_column_view_column_new (_("Name"),
 *   gtk_builder_list_item_factory_new_from_resource ("/name.ui"));
 * ```
 *
 * Returns: a new `GtkColumnViewColumn` using the given @factory
 */
GtkColumnViewColumn *
gtk_column_view_column_new (const char         *title,
                            GtkListItemFactory *factory)
{
  GtkColumnViewColumn *result;

  g_return_val_if_fail (factory == NULL || GTK_IS_LIST_ITEM_FACTORY (factory), NULL);

  result = g_object_new (GTK_TYPE_COLUMN_VIEW_COLUMN,
                         "factory", factory,
                         "title", title,
                         NULL);

  g_clear_object (&factory);

  return result;
}

GtkColumnViewCellWidget *
gtk_column_view_column_get_first_cell (GtkColumnViewColumn *this)
{
  return this->first_cell;
}

void
gtk_column_view_column_add_cell (GtkColumnViewColumn *this,
                                 GtkColumnViewCellWidget   *cell)
{
  this->first_cell = cell;

  gtk_widget_set_visible (GTK_WIDGET (cell), this->visible);
  gtk_column_view_column_queue_resize (this);
}

void
gtk_column_view_column_remove_cell (GtkColumnViewColumn *this,
                                    GtkColumnViewCellWidget   *cell)
{
  if (cell == this->first_cell)
    this->first_cell = gtk_column_view_cell_widget_get_next (cell);

  gtk_column_view_column_queue_resize (this);
  gtk_widget_queue_resize (GTK_WIDGET (cell));
}

void
gtk_column_view_column_queue_resize (GtkColumnViewColumn *this)
{
  GtkColumnViewCellWidget *cell;

  if (this->minimum_size_request < 0)
    return;

  this->minimum_size_request = -1;
  this->natural_size_request = -1;

  if (this->header)
    gtk_widget_queue_resize (this->header);

  for (cell = this->first_cell; cell; cell = gtk_column_view_cell_widget_get_next (cell))
    {
      gtk_widget_queue_resize (GTK_WIDGET (cell));
    }
}

void
gtk_column_view_column_measure (GtkColumnViewColumn *this,
                                int                 *minimum,
                                int                 *natural)
{
  if (this->fixed_width > -1)
    {
      this->minimum_size_request  = this->fixed_width;
      this->natural_size_request  = this->fixed_width;
    }

  if (this->minimum_size_request < 0)
    {
      GtkColumnViewCellWidget *cell;
      int min, nat, cell_min, cell_nat;

      if (this->header)
        {
          gtk_widget_measure (this->header, GTK_ORIENTATION_HORIZONTAL, -1, &min, &nat, NULL, NULL);
        }
      else
        {
          min = 0;
          nat = 0;
        }

      for (cell = this->first_cell; cell; cell = gtk_column_view_cell_widget_get_next (cell))
        {
          gtk_widget_measure (GTK_WIDGET (cell),
                              GTK_ORIENTATION_HORIZONTAL,
                              -1,
                              &cell_min, &cell_nat,
                              NULL, NULL);

          min = MAX (min, cell_min);
          nat = MAX (nat, cell_nat);
        }

      this->minimum_size_request = min;
      this->natural_size_request = nat;
    }

  *minimum = this->minimum_size_request;
  *natural = this->natural_size_request;
}

void
gtk_column_view_column_allocate (GtkColumnViewColumn *this,
                                 int                  offset,
                                 int                  size)
{
  this->allocation_offset = offset;
  this->allocation_size = size;
  this->header_position = offset;
}

void
gtk_column_view_column_get_allocation (GtkColumnViewColumn *this,
                                       int                 *offset,
                                       int                 *size)
{
  if (offset)
    *offset = this->allocation_offset;
  if (size)
    *size = this->allocation_size;
}

static void
gtk_column_view_column_create_cells (GtkColumnViewColumn *this)
{
  GtkListView *list;
  GtkWidget *row;

  if (this->first_cell)
    return;

  list = gtk_column_view_get_list_view (GTK_COLUMN_VIEW (this->view));
  for (row = gtk_widget_get_first_child (GTK_WIDGET (list));
       row != NULL;
       row = gtk_widget_get_next_sibling (row))
    {
      GtkColumnViewRowWidget *list_item;
      GtkListItemBase *base;
      GtkWidget *cell;

      list_item = GTK_COLUMN_VIEW_ROW_WIDGET (row);
      base = GTK_LIST_ITEM_BASE (row);
      cell = gtk_column_view_cell_widget_new (this, gtk_column_view_is_inert (this->view));
      gtk_column_view_row_widget_add_child (list_item, cell);
      gtk_list_item_base_update (GTK_LIST_ITEM_BASE (cell),
                                 gtk_list_item_base_get_position (base),
                                 gtk_list_item_base_get_item (base),
                                 gtk_list_item_base_get_selected (base));
    }
}

static void
gtk_column_view_column_remove_cells (GtkColumnViewColumn *this)
{
  while (this->first_cell)
    gtk_column_view_cell_widget_remove (this->first_cell);
}

static void
gtk_column_view_column_create_header (GtkColumnViewColumn *this)
{
  if (this->header != NULL)
    return;

  this->header = gtk_column_view_title_new (this);
  gtk_widget_set_visible (this->header, this->visible);
  gtk_column_view_row_widget_add_child (gtk_column_view_get_header_widget (this->view),
                                  this->header);
  gtk_column_view_column_queue_resize (this);
}

static void
gtk_column_view_column_remove_header (GtkColumnViewColumn *this)
{
  if (this->header == NULL)
    return;

  gtk_column_view_row_widget_remove_child (gtk_column_view_get_header_widget (this->view),
                                     this->header);
  this->header = NULL;
  gtk_column_view_column_queue_resize (this);
}

static void
gtk_column_view_column_ensure_cells (GtkColumnViewColumn *this)
{
  if (this->view && gtk_column_view_column_get_visible (this))
    gtk_column_view_column_create_cells (this);
  else
    gtk_column_view_column_remove_cells (this);

  if (this->view)
    gtk_column_view_column_create_header (this);
  else
    gtk_column_view_column_remove_header (this);
}

/**
 * gtk_column_view_column_get_column_view:
 * @this: a column
 *
 * Gets the column view that's currently displaying this column.
 *
 * If @this has not been added to a column view yet, `NULL` is returned.
 *
 * Returns: (nullable) (transfer none): The column view displaying @this.
 */
GtkColumnView *
gtk_column_view_column_get_column_view (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), NULL);

  return this->view;
}

void
gtk_column_view_column_set_column_view (GtkColumnViewColumn *this,
                                        GtkColumnView       *view)
{
  if (this->view == view)
    return;

  gtk_column_view_column_remove_cells (this);
  gtk_column_view_column_remove_header (this);

  this->view = view;

  gtk_column_view_column_ensure_cells (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_COLUMN_VIEW]);
}

void
gtk_column_view_column_set_position (GtkColumnViewColumn *this,
                                     guint                position)
{
  GtkColumnViewCellWidget *cell;

  gtk_column_view_row_widget_reorder_child (gtk_column_view_get_header_widget (this->view),
                                      this->header,
                                      position);

  for (cell = this->first_cell; cell; cell = gtk_column_view_cell_widget_get_next (cell))
    {
      GtkColumnViewRowWidget *list_item;

      list_item = GTK_COLUMN_VIEW_ROW_WIDGET (gtk_widget_get_parent (GTK_WIDGET (cell)));
      gtk_column_view_row_widget_reorder_child (list_item, GTK_WIDGET (cell), position);
    }
}

/**
 * gtk_column_view_column_get_factory:
 * @this: a column
 *
 * Gets the factory that's currently used to populate list items
 * for this column.
 *
 * Returns: (nullable) (transfer none): The factory in use
 **/
GtkListItemFactory *
gtk_column_view_column_get_factory (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), NULL);

  return this->factory;
}

void
gtk_column_view_column_update_factory (GtkColumnViewColumn *this,
                                       gboolean             inert)
{
  GtkListItemFactory *factory;
  GtkColumnViewCellWidget *cell;

  if (this->factory == NULL)
    return;

  if (inert)
    factory = NULL;
  else
    factory = this->factory;

  for (cell = this->first_cell;
       cell;
       cell = gtk_column_view_cell_widget_get_next (cell))
    {
      gtk_list_factory_widget_set_factory (GTK_LIST_FACTORY_WIDGET (cell), factory);
    }
}

/**
 * gtk_column_view_column_set_factory:
 * @this: a column
 * @factory: (nullable) (transfer none): the factory to use
 *
 * Sets the `GtkListItemFactory` to use for populating list items
 * for this column.
 */
void
gtk_column_view_column_set_factory (GtkColumnViewColumn *this,
                                    GtkListItemFactory  *factory)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));
  g_return_if_fail (factory == NULL || GTK_LIST_ITEM_FACTORY (factory));

  if (this->factory && !factory)
    gtk_column_view_column_update_factory (this, TRUE);

  if (!g_set_object (&this->factory, factory))
    return;

  if (this->view && !gtk_column_view_is_inert (this->view))
    gtk_column_view_column_update_factory (this, FALSE);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_FACTORY]);
}

/**
 * gtk_column_view_column_set_title:
 * @this: a column
 * @title: (nullable): Title to use for this column
 *
 * Sets the title of this column.
 *
 * The title is displayed in the header of a `GtkColumnView`
 * for this column and is therefore user-facing text that should
 * be translated.
 */
void
gtk_column_view_column_set_title (GtkColumnViewColumn *this,
                                  const char          *title)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));

  if (g_strcmp0 (this->title, title) == 0)
    return;

  g_free (this->title);
  this->title = g_strdup (title);

  if (this->header)
    gtk_column_view_title_set_title (GTK_COLUMN_VIEW_TITLE (this->header), title);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_TITLE]);
}

/**
 * gtk_column_view_column_get_title:
 * @this: a column
 *
 * Returns the title set with [method@Gtk.ColumnViewColumn.set_title].
 *
 * Returns: (nullable): The column's title
 */
const char *
gtk_column_view_column_get_title (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), FALSE);

  return this->title;
}

static void
gtk_column_view_column_remove_from_sorter (GtkColumnViewColumn *this)
{
  if (this->view == NULL)
    return;

  gtk_column_view_sorter_remove_column (GTK_COLUMN_VIEW_SORTER (gtk_column_view_get_sorter (this->view)), this);
}

/**
 * gtk_column_view_column_set_sorter:
 * @this: a column
 * @sorter: (nullable): the `GtkSorter` to associate with @column
 *
 * Associates a sorter with the column.
 *
 * If @sorter is unset, the column will not immutable users change
 * the sorting by clicking on its header.
 *
 * This sorter can be made active by clicking on the column
 * header, or by calling [method@Gtk.ColumnView.sort_by_column].
 *
 * See [method@Gtk.ColumnView.get_sorter] for the necessary steps
 * for setting up customizable sorting for [class@Gtk.ColumnView].
 */
void
gtk_column_view_column_set_sorter (GtkColumnViewColumn *this,
                                   GtkSorter           *sorter)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));
  g_return_if_fail (sorter == NULL || GTK_IS_SORTER (sorter));

  if (!g_set_object (&this->sorter, sorter))
    return;

  gtk_column_view_column_remove_from_sorter (this);

  if (this->header)
    gtk_column_view_title_update_sort (GTK_COLUMN_VIEW_TITLE (this->header));

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SORTER]);
}

/**
 * gtk_column_view_column_get_sorter:
 * @this: a column
 *
 * Returns the sorter that is associated with the column.
 *
 * Returns: (nullable) (transfer none): the `GtkSorter` of @this
 */
GtkSorter *
gtk_column_view_column_get_sorter (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), NULL);

  return this->sorter;
}

void
gtk_column_view_column_notify_sort (GtkColumnViewColumn *this)
{
  if (this->header)
    gtk_column_view_title_update_sort (GTK_COLUMN_VIEW_TITLE (this->header));
}

/**
 * gtk_column_view_column_set_visible:
 * @this: a column
 * @visible: whether this column should be visible
 *
 * Sets whether this column should be visible in views.
 */
void
gtk_column_view_column_set_visible (GtkColumnViewColumn *this,
                                    gboolean             visible)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));

  if (this->visible == visible)
    return;

  this->visible = visible;

  this->minimum_size_request = -1;
  this->natural_size_request = -1;

  if (this->header)
    gtk_widget_set_visible (GTK_WIDGET (this->header), visible);

  gtk_column_view_column_ensure_cells (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_VISIBLE]);
}

/**
 * gtk_column_view_column_get_visible:
 * @this: a column
 *
 * Returns whether this column is visible.
 *
 * Returns: true if this column is visible
 */
gboolean
gtk_column_view_column_get_visible (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), TRUE);

  return this->visible;
}

/**
 * gtk_column_view_column_set_header_menu:
 * @this: a column
 * @menu: (nullable): a `GMenuModel`
 *
 * Sets the menu model that is used to create the context menu
 * for the column header.
 */
void
gtk_column_view_column_set_header_menu (GtkColumnViewColumn *this,
                                        GMenuModel          *menu)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));
  g_return_if_fail (menu == NULL || G_IS_MENU_MODEL (menu));

  if (!g_set_object (&this->menu, menu))
    return;

  if (this->header)
    gtk_column_view_title_set_menu (GTK_COLUMN_VIEW_TITLE (this->header), menu);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_HEADER_MENU]);
}

/**
 * gtk_column_view_column_get_header_menu:
 * @this: a column
 *
 * Gets the menu model that is used to create the context menu
 * for the column header.
 *
 * Returns: (transfer none) (nullable): the `GMenuModel`
 */
GMenuModel *
gtk_column_view_column_get_header_menu (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), NULL);

  return this->menu;
}

/**
 * gtk_column_view_column_set_expand:
 * @this: a column
 * @expand: whether this column should expand to fill available space
 *
 * Sets the column to take available extra space.
 *
 * The extra space is shared equally amongst all columns that
 * have are set to expand.
 */
void
gtk_column_view_column_set_expand (GtkColumnViewColumn *this,
                                   gboolean             expand)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));

  if (this->expand == expand)
    return;

  this->expand = expand;

  if (this->visible && this->view)
    gtk_widget_queue_resize (GTK_WIDGET (this->view));

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_EXPAND]);
}

/**
 * gtk_column_view_column_get_expand:
 * @this: a column
 *
 * Returns whether this column should expand.
 *
 * Returns: true if this column expands
 */
gboolean
gtk_column_view_column_get_expand (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), TRUE);

  return this->expand;
}

/**
 * gtk_column_view_column_set_resizable:
 * @this: a column
 * @resizable: whether this column should be resizable
 *
 * Sets whether this column should be resizable by dragging.
 */
void
gtk_column_view_column_set_resizable (GtkColumnViewColumn *this,
                                      gboolean             resizable)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));

  if (this->resizable == resizable)
    return;

  this->resizable = resizable;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_RESIZABLE]);
}

/**
 * gtk_column_view_column_get_resizable:
 * @this: a column
 *
 * Returns whether this column is resizable.
 *
 * Returns: true if this column is resizable
 */
gboolean
gtk_column_view_column_get_resizable (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), TRUE);

  return this->resizable;
}

/**
 * gtk_column_view_column_set_fixed_width:
 * @this: a column
 * @fixed_width: the new fixed width, or -1
 *
 * Sets the fixed width of the column.
 *
 * If @fixed_width is -1, the fixed width of the column is unset.
 *
 * Setting a fixed width overrides the automatically calculated
 * width. Interactive resizing also sets the “fixed-width” property.
 */
void
gtk_column_view_column_set_fixed_width (GtkColumnViewColumn *this,
                                        int                  fixed_width)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));
  g_return_if_fail (fixed_width >= -1);

  if (this->fixed_width == fixed_width)
    return;

  this->fixed_width = fixed_width;

  gtk_column_view_column_queue_resize (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_FIXED_WIDTH]);
}

/**
 * gtk_column_view_column_get_fixed_width:
 * @this: a column
 *
 * Gets the fixed width of the column.
 *
 * Returns: the fixed with of the column
 */
int
gtk_column_view_column_get_fixed_width (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), -1);

  return this->fixed_width;
}

GtkWidget *
gtk_column_view_column_get_header (GtkColumnViewColumn *this)
{
  return this->header;
}

void
gtk_column_view_column_set_header_position (GtkColumnViewColumn *this,
                                            int                  offset)
{
  this->header_position = offset;
}

void
gtk_column_view_column_get_header_allocation (GtkColumnViewColumn *this,
                                              int                 *offset,
                                              int                 *size)
{
  if (offset)
    *offset = this->header_position;

  if (size)
    *size = this->allocation_size;
}

/**
 * gtk_column_view_column_set_id:
 * @this: a column
 * @id: (nullable): ID to use for this column
 *
 * Sets the id of this column.
 *
 * GTK makes no use of this, but applications can use it when
 * storing column view configuration.
 *
 * It is up to callers to ensure uniqueness of IDs.
 *
 * Since: 4.10
 */
void
gtk_column_view_column_set_id (GtkColumnViewColumn *this,
                               const char          *id)
{
  g_return_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this));

  if (g_strcmp0 (this->id, id) == 0)
    return;

  g_free (this->id);
  this->id = g_strdup (id);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ID]);
}

/**
 * gtk_column_view_column_get_id:
 * @this: a column
 *
 * Returns the ID set with [method@Gtk.ColumnViewColumn.set_id].
 *
 * Returns: (nullable): The column's ID
 *
 * Since: 4.10
 */
const char *
gtk_column_view_column_get_id (GtkColumnViewColumn *this)
{
  g_return_val_if_fail (GTK_IS_COLUMN_VIEW_COLUMN (this), NULL);

  return this->id;
}
