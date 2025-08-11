/*
 * Copyright © 2020 Red Hat, Inc.
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
 * Authors: Matthias Clasen
 */

#include "config.h"

#include "gtkselectionfiltermodel.h"
#include "gtkbitset.h"

#include "gtkprivate.h"

/**
 * GtkSelectionFilterModel:
 *
 * A list model that presents the selection from a `GtkSelectionModel`.
 */

enum {
  PROP_0,
  PROP_ITEM_TYPE,
  PROP_MODEL,
  PROP_N_ITEMS,

  NUM_PROPERTIES
};

struct _GtkSelectionFilterModel
{
  GObject parent_instance;

  GtkSelectionModel *model;
  GtkBitset *selection;
};

struct _GtkSelectionFilterModelClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static GType
gtk_selection_filter_model_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_selection_filter_model_get_n_items (GListModel *list)
{
  GtkSelectionFilterModel *this = GTK_SELECTION_FILTER_MODEL (list);

  if (!this->selection)
    return 0;

  return gtk_bitset_get_size (this->selection);
}

static gpointer
gtk_selection_filter_model_get_item (GListModel *list,
                                     guint       position)
{
  GtkSelectionFilterModel *this = GTK_SELECTION_FILTER_MODEL (list);

  if (!this->selection)
    return NULL;

  if (position >= gtk_bitset_get_size (this->selection))
    return NULL;

  position = gtk_bitset_get_nth (this->selection, position);

  return g_list_model_get_item (G_LIST_MODEL (this->model), position);
}

static void
gtk_selection_filter_model_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_selection_filter_model_get_item_type;
  iface->get_n_items = gtk_selection_filter_model_get_n_items;
  iface->get_item = gtk_selection_filter_model_get_item;
}

G_DEFINE_TYPE_WITH_CODE (GtkSelectionFilterModel, gtk_selection_filter_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_selection_filter_model_list_model_init))

static void
selection_filter_model_items_changed (GtkSelectionFilterModel *this,
                                      guint                    position,
                                      guint                    removed,
                                      guint                    added)
{
  GtkBitset *selection;
  guint sel_position = 0;
  guint sel_removed = 0;
  guint sel_added = 0;

  selection = gtk_selection_model_get_selection (this->model);

  if (position > 0)
    sel_position = gtk_bitset_get_size_in_range (this->selection, 0, position - 1);

  if (removed > 0)
    sel_removed = gtk_bitset_get_size_in_range (this->selection, position, position + removed - 1);

  if (added > 0)
    sel_added = gtk_bitset_get_size_in_range (selection, position, position + added - 1);

  gtk_bitset_unref (this->selection);
  this->selection = gtk_bitset_copy (selection);

  gtk_bitset_unref (selection);

  if (sel_removed > 0 || sel_added > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), sel_position, sel_removed, sel_added);
  if (sel_removed != sel_added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

static void
gtk_selection_filter_model_items_changed_cb (GListModel              *model,
                                             guint                    position,
                                             guint                    removed,
                                             guint                    added,
                                             GtkSelectionFilterModel *this)
{
  selection_filter_model_items_changed (this, position, removed, added);
}

static void
gtk_selection_filter_model_selection_changed_cb (GListModel              *model,
                                                 guint                    position,
                                                 guint                    n_items,
                                                 GtkSelectionFilterModel *this)
{
  selection_filter_model_items_changed (this, position, n_items, n_items);
}

static void
gtk_selection_filter_model_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  GtkSelectionFilterModel *this = GTK_SELECTION_FILTER_MODEL (object);

  switch (prop_id)
    {
    case PROP_MODEL:
      gtk_selection_filter_model_set_model (this, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_selection_filter_model_get_property (GObject     *object,
                                         guint        prop_id,
                                         GValue      *value,
                                         GParamSpec  *pspec)
{
  GtkSelectionFilterModel *this = GTK_SELECTION_FILTER_MODEL (object);

  switch (prop_id)
    {
    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, gtk_selection_filter_model_get_item_type (G_LIST_MODEL (this)));
      break;

    case PROP_MODEL:
      g_value_set_object (value, this->model);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, gtk_selection_filter_model_get_n_items (G_LIST_MODEL (this)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_selection_filter_model_clear_model (GtkSelectionFilterModel *this)
{
  if (this->model == NULL)
    return;

  g_signal_handlers_disconnect_by_func (this->model, gtk_selection_filter_model_items_changed_cb, this);
  g_signal_handlers_disconnect_by_func (this->model, gtk_selection_filter_model_selection_changed_cb, this);

  g_clear_object (&this->model);
  g_clear_pointer (&this->selection, gtk_bitset_unref);
}

static void
gtk_selection_filter_model_dispose (GObject *object)
{
  GtkSelectionFilterModel *this = GTK_SELECTION_FILTER_MODEL (object);

  gtk_selection_filter_model_clear_model (this);

  G_OBJECT_CLASS (gtk_selection_filter_model_parent_class)->dispose (object);
}

static void
gtk_selection_filter_model_class_init (GtkSelectionFilterModelClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->set_property = gtk_selection_filter_model_set_property;
  gobject_class->get_property = gtk_selection_filter_model_get_property;
  gobject_class->dispose = gtk_selection_filter_model_dispose;

  /**
   * GtkSelectionFilterModel:item-type:
   *
   * The type of items. See [method@Gio.ListModel.get_item_type].
   *
   * Since: 4.8
   **/
  properties[PROP_ITEM_TYPE] =
    g_param_spec_gtype ("item-type", NULL, NULL,
                        G_TYPE_OBJECT,
                        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkSelectionFilterModel:model:
   *
   * The model being filtered.
   */
  properties[PROP_MODEL] =
      g_param_spec_object ("model", NULL, NULL,
                           GTK_TYPE_SELECTION_MODEL,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSelectionFilterModel:n-items:
   *
   * The number of items. See [method@Gio.ListModel.get_n_items].
   *
   * Since: 4.8
   **/
  properties[PROP_N_ITEMS] =
    g_param_spec_uint ("n-items", NULL, NULL,
                       0, G_MAXUINT, 0,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, NUM_PROPERTIES, properties);
}

static void
gtk_selection_filter_model_init (GtkSelectionFilterModel *this)
{
}

/**
 * gtk_selection_filter_model_new:
 * @model: (nullable) (transfer none): the selection model to filter
 *
 * Creates a new `GtkSelectionFilterModel` that will include the
 * selected items from the underlying selection model.
 *
 * Returns: a new `GtkSelectionFilterModel`
 */
GtkSelectionFilterModel *
gtk_selection_filter_model_new (GtkSelectionModel *model)
{
  return g_object_new (GTK_TYPE_SELECTION_FILTER_MODEL,
                       "model", model,
                       NULL);
}

/**
 * gtk_selection_filter_model_set_model:
 * @this: a `GtkSelectionFilterModel`
 * @model: (nullable): The model to be filtered
 *
 * Sets the model to be filtered.
 *
 * Note that GTK makes no effort to ensure that @model conforms to
 * the item type of @this. It assumes that the caller knows what they
 * are doing and have set up an appropriate filter to ensure that item
 * types match.
 **/
void
gtk_selection_filter_model_set_model (GtkSelectionFilterModel *this,
                                      GtkSelectionModel       *model)
{
  guint removed, added;

  g_return_if_fail (GTK_IS_SELECTION_FILTER_MODEL (this));
  g_return_if_fail (model == NULL || GTK_IS_SELECTION_MODEL (model));

  if (this->model == model)
    return;

  removed = g_list_model_get_n_items (G_LIST_MODEL (this));
  gtk_selection_filter_model_clear_model (this);

  if (model)
    {
      GtkBitset *selection;

      this->model = g_object_ref (model);

      selection = gtk_selection_model_get_selection (this->model);
      this->selection = gtk_bitset_copy (selection);
      gtk_bitset_unref (selection);

      g_signal_connect (model, "items-changed", G_CALLBACK (gtk_selection_filter_model_items_changed_cb), this);
      g_signal_connect (model, "selection-changed", G_CALLBACK (gtk_selection_filter_model_selection_changed_cb), this);
    }

  added = g_list_model_get_n_items (G_LIST_MODEL (this));

  if (removed > 0 || added > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), 0, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODEL]);
}

/**
 * gtk_selection_filter_model_get_model:
 * @this: a `GtkSelectionFilterModel`
 *
 * Gets the model currently filtered or %NULL if none.
 *
 * Returns: (nullable) (transfer none): The model that gets filtered
 */
GtkSelectionModel *
gtk_selection_filter_model_get_model (GtkSelectionFilterModel *this)
{
  g_return_val_if_fail (GTK_IS_SELECTION_FILTER_MODEL (this), NULL);

  return this->model;
}
