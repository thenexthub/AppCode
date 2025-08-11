/*
 * Copyright © 2019 Red Hat, Inc.
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
 * Authors: Matthias Clasen <mclasen@redhat.com>
 */

#include "config.h"

#include "gtkmultiselection.h"

#include "gtkbitset.h"
#include "gtksectionmodelprivate.h"
#include "gtkselectionmodel.h"

/**
 * GtkMultiSelection:
 *
 * A selection model that allows selecting multiple elements.
 */

struct _GtkMultiSelection
{
  GObject parent_instance;

  GListModel *model;

  GtkBitset *selected;
  GHashTable *items; /* item => position */
};

struct _GtkMultiSelectionClass
{
  GObjectClass parent_class;
};

enum {
  PROP_0,
  PROP_ITEM_TYPE,
  PROP_MODEL,
  PROP_N_ITEMS,

  N_PROPS,
};

static GParamSpec *properties[N_PROPS] = { NULL, };

static GType
gtk_multi_selection_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_multi_selection_get_n_items (GListModel *list)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (list);

  if (this->model == NULL)
    return 0;

  return g_list_model_get_n_items (this->model);
}

static gpointer
gtk_multi_selection_get_item (GListModel *list,
                              guint       position)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (list);

  if (this->model == NULL)
    return NULL;

  return g_list_model_get_item (this->model, position);
}

static void
gtk_multi_selection_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_multi_selection_get_item_type;
  iface->get_n_items = gtk_multi_selection_get_n_items;
  iface->get_item = gtk_multi_selection_get_item;
}

static void
gtk_multi_selection_get_section (GtkSectionModel *model,
                                 guint            position,
                                 guint           *out_start,
                                 guint           *out_end)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (model);

  gtk_list_model_get_section (this->model, position, out_start, out_end);
}

static void
gtk_multi_selection_section_model_init (GtkSectionModelInterface *iface)
{
  iface->get_section = gtk_multi_selection_get_section;
}

static gboolean
gtk_multi_selection_is_selected (GtkSelectionModel *model,
                                 guint              position)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (model);

  return gtk_bitset_contains (this->selected, position);
}

static GtkBitset *
gtk_multi_selection_get_selection_in_range (GtkSelectionModel *model,
                                            guint              pos,
                                            guint              n_items)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (model);

  return gtk_bitset_ref (this->selected);
}

static void
gtk_multi_selection_toggle_selection (GtkMultiSelection *this,
                                      GtkBitset         *changes)
{
  GListModel *model = G_LIST_MODEL (this);
  GtkBitsetIter change_iter, selected_iter;
  GtkBitset *selected;
  guint change_pos, selected_pos;
  gboolean more;

  gtk_bitset_difference (this->selected, changes);

  selected = gtk_bitset_copy (changes);
  gtk_bitset_intersect (selected, this->selected);

  if (!gtk_bitset_iter_init_first (&selected_iter, selected, &selected_pos))
    selected_pos = G_MAXUINT;

  for (more = gtk_bitset_iter_init_first (&change_iter, changes, &change_pos);
       more;
       more = gtk_bitset_iter_next (&change_iter, &change_pos))
    {
      gpointer item = g_list_model_get_item (model, change_pos);

      if (change_pos < selected_pos)
        {
          g_hash_table_remove (this->items, item);
          g_object_unref (item);
        }
      else
        {
          g_assert (change_pos == selected_pos);

          g_hash_table_insert (this->items, item, GUINT_TO_POINTER (change_pos));

          if (!gtk_bitset_iter_next (&selected_iter, &selected_pos))
            selected_pos = G_MAXUINT;
        }
    }

  gtk_bitset_unref (selected);
}

static gboolean
gtk_multi_selection_set_selection (GtkSelectionModel *model,
                                   GtkBitset         *selected,
                                   GtkBitset         *mask)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (model);
  GtkBitset *changes;
  guint min, max, n_items;

  /* changes = (this->selected XOR selected) AND mask
   * But doing it this way avoids looking at all values outside the mask
   */
  changes = gtk_bitset_copy (selected);
  gtk_bitset_difference (changes, this->selected);
  gtk_bitset_intersect (changes, mask);

  min = gtk_bitset_get_minimum (changes);
  max = gtk_bitset_get_maximum (changes);

  /* sanity check */
  n_items = this->model ? g_list_model_get_n_items (this->model) : 0;
  if (max >= n_items)
    {
      gtk_bitset_remove_range_closed (changes, n_items, max);
      max = gtk_bitset_get_maximum (changes);
    }

  /* actually do the change */
  gtk_multi_selection_toggle_selection (this, changes);

  gtk_bitset_unref (changes);

  if (min <= max)
    gtk_selection_model_selection_changed (model, min, max - min + 1);

  return TRUE;
}

static void
gtk_multi_selection_selection_model_init (GtkSelectionModelInterface *iface)
{
  iface->is_selected = gtk_multi_selection_is_selected;
  iface->get_selection_in_range = gtk_multi_selection_get_selection_in_range;
  iface->set_selection = gtk_multi_selection_set_selection;
}

G_DEFINE_TYPE_EXTENDED (GtkMultiSelection, gtk_multi_selection, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL,
                                               gtk_multi_selection_list_model_init)
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_SECTION_MODEL,
                                               gtk_multi_selection_section_model_init)
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_SELECTION_MODEL,
                                               gtk_multi_selection_selection_model_init))

static void
gtk_multi_selection_items_changed_cb (GListModel        *model,
                                      guint              position,
                                      guint              removed,
                                      guint              added,
                                      GtkMultiSelection *this)
{
  GHashTableIter iter;
  gpointer item, pos_pointer;
  GHashTable *pending = NULL;
  guint i;

  gtk_bitset_splice (this->selected, position, removed, added);

  g_hash_table_iter_init (&iter, this->items);
  while (g_hash_table_iter_next (&iter, &item, &pos_pointer))
    {
      guint pos = GPOINTER_TO_UINT (pos_pointer);

      if (pos < position)
        continue;
      else if (pos >= position + removed)
        g_hash_table_iter_replace (&iter, GUINT_TO_POINTER (pos - removed + added));
      else /* if pos is in the removed range */
        {
          if (added == 0)
            {
              g_hash_table_iter_remove (&iter);
            }
          else
            {
              g_hash_table_iter_steal (&iter);
              if (pending == NULL)
                pending = g_hash_table_new_full (NULL, NULL, g_object_unref, NULL);
              g_hash_table_add (pending, item);
            }
        }
    }

  for (i = position; pending != NULL && i < position + added; i++)
    {
      item = g_list_model_get_item (model, i);
      if (g_hash_table_contains (pending, item))
        {
          gtk_bitset_add (this->selected, i);
          g_hash_table_insert (this->items, item, GUINT_TO_POINTER (i));
          g_hash_table_remove (pending, item);
          if (g_hash_table_size (pending) == 0)
            g_clear_pointer (&pending, g_hash_table_unref);
        }
      else
        {
          g_object_unref (item);
        }
    }

  g_clear_pointer (&pending, g_hash_table_unref);

  g_list_model_items_changed (G_LIST_MODEL (this), position, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

static void
gtk_multi_selection_sections_changed_cb (GtkSectionModel *model,
                                         unsigned int     position,
                                         unsigned int     n_items,
                                         gpointer         user_data)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (user_data);

  gtk_section_model_sections_changed (GTK_SECTION_MODEL (this), position, n_items);
}

static void
gtk_multi_selection_clear_model (GtkMultiSelection *this)
{
  if (this->model == NULL)
    return;

  g_signal_handlers_disconnect_by_func (this->model,
                                        gtk_multi_selection_items_changed_cb,
                                        this);
  g_signal_handlers_disconnect_by_func (this->model,
                                        gtk_multi_selection_sections_changed_cb,
                                        this);
  g_clear_object (&this->model);
}

static void
gtk_multi_selection_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)

{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (object);

  switch (prop_id)
    {
    case PROP_MODEL:
      gtk_multi_selection_set_model (this, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_multi_selection_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (object);

  switch (prop_id)
    {
    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, gtk_multi_selection_get_item_type (G_LIST_MODEL (this)));
      break;

    case PROP_MODEL:
      g_value_set_object (value, this->model);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, gtk_multi_selection_get_n_items (G_LIST_MODEL (this)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_multi_selection_dispose (GObject *object)
{
  GtkMultiSelection *this = GTK_MULTI_SELECTION (object);

  gtk_multi_selection_clear_model (this);

  g_clear_pointer (&this->selected, gtk_bitset_unref);
  g_clear_pointer (&this->items, g_hash_table_unref);

  G_OBJECT_CLASS (gtk_multi_selection_parent_class)->dispose (object);
}

static void
gtk_multi_selection_class_init (GtkMultiSelectionClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = gtk_multi_selection_get_property;
  gobject_class->set_property = gtk_multi_selection_set_property;
  gobject_class->dispose = gtk_multi_selection_dispose;

  /**
   * GtkMultiSelection:item-type:
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
   * GtkMultiSelection:model:
   *
   * The list managed by this selection.
   */
  properties[PROP_MODEL] =
    g_param_spec_object ("model", NULL, NULL,
                         G_TYPE_LIST_MODEL,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkMultiSelection:n-items:
   *
   * The number of items. See [method@Gio.ListModel.get_n_items].
   *
   * Since: 4.8
   **/
  properties[PROP_N_ITEMS] =
    g_param_spec_uint ("n-items", NULL, NULL,
                       0, G_MAXUINT, 0,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);
}

static void
gtk_multi_selection_init (GtkMultiSelection *this)
{
  this->selected = gtk_bitset_new_empty ();
  this->items = g_hash_table_new_full (NULL, NULL, g_object_unref, NULL);
}

/**
 * gtk_multi_selection_new:
 * @model: (nullable) (transfer full): the `GListModel` to manage
 *
 * Creates a new selection to handle @model.
 *
 * Returns: (transfer full): a new `GtkMultiSelection`
 */
GtkMultiSelection *
gtk_multi_selection_new (GListModel *model)
{
  GtkMultiSelection *this;

  g_return_val_if_fail (model == NULL || G_IS_LIST_MODEL (model), NULL);

  this = g_object_new (GTK_TYPE_MULTI_SELECTION,
                       "model", model,
                       NULL);

  /* consume the reference */
  g_clear_object (&model);

  return this;
}

/**
 * gtk_multi_selection_get_model:
 * @this: a `GtkMultiSelection`
 *
 * Returns the underlying model of @this.
 *
 * Returns: (transfer none) (nullable): the underlying model
 */
GListModel *
gtk_multi_selection_get_model (GtkMultiSelection *this)
{
  g_return_val_if_fail (GTK_IS_MULTI_SELECTION (this), NULL);

  return this->model;
}

/**
 * gtk_multi_selection_set_model:
 * @this: a `GtkMultiSelection`
 * @model: (nullable): A `GListModel` to wrap
 *
 * Sets the model that @this should wrap.
 *
 * If @model is %NULL, @this will be empty.
 */
void
gtk_multi_selection_set_model (GtkMultiSelection *this,
                               GListModel        *model)
{
  guint n_items_before;

  g_return_if_fail (GTK_IS_MULTI_SELECTION (this));
  g_return_if_fail (model == NULL || G_IS_LIST_MODEL (model));

  if (this->model == model)
    return;

  n_items_before = this->model ? g_list_model_get_n_items (this->model) : 0;
  gtk_multi_selection_clear_model (this);

  if (model)
    {
      this->model = g_object_ref (model);
      g_signal_connect (this->model,
                        "items-changed",
                        G_CALLBACK (gtk_multi_selection_items_changed_cb),
                        this);
      if (GTK_IS_SECTION_MODEL (this->model))
        g_signal_connect (this->model, "sections-changed",
                          G_CALLBACK (gtk_multi_selection_sections_changed_cb), this);
      gtk_multi_selection_items_changed_cb (this->model,
                                            0,
                                            n_items_before,
                                            g_list_model_get_n_items (model),
                                            this);
    }
  else
    {
      gtk_bitset_remove_all (this->selected);
      g_hash_table_remove_all (this->items);
      g_list_model_items_changed (G_LIST_MODEL (this), 0, n_items_before, 0);
      if (n_items_before)
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODEL]);
}
