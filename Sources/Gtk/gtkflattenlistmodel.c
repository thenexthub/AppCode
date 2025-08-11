/*
 * Copyright © 2018 Benjamin Otte
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

#include "gtkflattenlistmodel.h"

#include "gtksectionmodel.h"
#include "gtkrbtreeprivate.h"

/**
 * GtkFlattenListModel:
 *
 * A list model that concatenates other list models.
 *
 * `GtkFlattenListModel` takes a list model containing list models, and flattens
 * it into a single model. Each list model becomes a section in the single model.
 */

enum {
  PROP_0,
  PROP_ITEM_TYPE,
  PROP_MODEL,
  PROP_N_ITEMS,

  NUM_PROPERTIES
};

typedef struct _FlattenNode FlattenNode;
typedef struct _FlattenAugment FlattenAugment;

struct _FlattenNode
{
  GListModel *model;
  GtkFlattenListModel *list;
};

struct _FlattenAugment
{
  guint n_items;
  guint n_models;
};

struct _GtkFlattenListModel
{
  GObject parent_instance;

  GListModel *model;
  GtkRbTree *items; /* NULL if model == NULL */
};

struct _GtkFlattenListModelClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static FlattenNode *
gtk_flatten_list_model_get_nth (GtkRbTree *tree,
                                guint      position,
                                guint     *model_position)
{
  FlattenNode *node, *tmp;
  guint model_n_items;

  node = gtk_rb_tree_get_root (tree);

  while (node)
    {
      tmp = gtk_rb_tree_node_get_left (node);
      if (tmp)
        {
          FlattenAugment *aug = gtk_rb_tree_get_augment (tree, tmp);
          if (position < aug->n_items)
            {
              node = tmp;
              continue;
            }
          position -= aug->n_items;
        }

      model_n_items = g_list_model_get_n_items (node->model);
      if (position < model_n_items)
        break;
      position -= model_n_items;

      node = gtk_rb_tree_node_get_right (node);
    }

  if (model_position)
    *model_position = node ? position : 0;

  return node;
}

static FlattenNode *
gtk_flatten_list_model_get_nth_model (GtkRbTree *tree,
                                      guint      position,
                                      guint     *items_before)
{
  FlattenNode *node, *tmp;
  guint before;

  node = gtk_rb_tree_get_root (tree);
  before = 0;

  while (node)
    {
      tmp = gtk_rb_tree_node_get_left (node);
      if (tmp)
        {
          FlattenAugment *aug = gtk_rb_tree_get_augment (tree, tmp);
          if (position < aug->n_models)
            {
              node = tmp;
              continue;
            }
          position -= aug->n_models;
          before += aug->n_items;
        }

      if (position == 0)
        break;
      position--;
      before += g_list_model_get_n_items (node->model);

      node = gtk_rb_tree_node_get_right (node);
    }

  if (items_before)
    *items_before = before;

  return node;
}

static GType
gtk_flatten_list_model_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_flatten_list_model_get_n_items (GListModel *list)
{
  GtkFlattenListModel *this = GTK_FLATTEN_LIST_MODEL (list);
  FlattenAugment *aug;
  FlattenNode *node;

  if (!this->items)
    return 0;

  node = gtk_rb_tree_get_root (this->items);
  if (node == NULL)
    return 0;

  aug = gtk_rb_tree_get_augment (this->items, node);
  return aug->n_items;
}

static gpointer
gtk_flatten_list_model_get_item (GListModel *list,
                                 guint       position)
{
  GtkFlattenListModel *this = GTK_FLATTEN_LIST_MODEL (list);
  FlattenNode *node;
  guint model_pos;

  if (!this->items)
    return NULL;

  node = gtk_flatten_list_model_get_nth (this->items, position, &model_pos);
  if (node == NULL)
    return NULL;

  return g_list_model_get_item (node->model, model_pos);
}

static void
gtk_flatten_list_model_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_flatten_list_model_get_item_type;
  iface->get_n_items = gtk_flatten_list_model_get_n_items;
  iface->get_item = gtk_flatten_list_model_get_item;
}

static void
gtk_flatten_list_model_get_section (GtkSectionModel *model,
                                    guint            position,
                                    guint           *out_start,
                                    guint           *out_end)
{
  GtkFlattenListModel *this = GTK_FLATTEN_LIST_MODEL (model);
  FlattenNode *node;
  guint model_pos;

  node = gtk_flatten_list_model_get_nth (this->items, position, &model_pos);
  if (node == NULL)
    {
      *out_start = gtk_flatten_list_model_get_n_items (G_LIST_MODEL (this));
      *out_end = G_MAXUINT;
      return;
    }

  *out_start = position - model_pos;
  *out_end = position - model_pos + g_list_model_get_n_items (node->model);
}

static void
gtk_flatten_list_model_section_model_init (GtkSectionModelInterface *iface)
{
  iface->get_section = gtk_flatten_list_model_get_section;
}

G_DEFINE_TYPE_WITH_CODE (GtkFlattenListModel, gtk_flatten_list_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL,
                                                gtk_flatten_list_model_model_init)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_SECTION_MODEL,
                                                gtk_flatten_list_model_section_model_init))

static void
gtk_flatten_list_model_items_changed_cb (GListModel *model,
                                         guint       position,
                                         guint       removed,
                                         guint       added,
                                         gpointer    _node)
{
  FlattenNode *node = _node, *parent, *left;
  GtkFlattenListModel *this = node->list;
  guint real_position;

  gtk_rb_tree_node_mark_dirty (node);
  real_position = position;

  left = gtk_rb_tree_node_get_left (node);
  if (left)
    {
      FlattenAugment *aug = gtk_rb_tree_get_augment (this->items, left);
      real_position += aug->n_items;
    }

  for (;
       (parent = gtk_rb_tree_node_get_parent (node)) != NULL;
       node = parent)
    {
      left = gtk_rb_tree_node_get_left (parent);
      if (left != node)
        {
          if (left)
            {
              FlattenAugment *aug = gtk_rb_tree_get_augment (this->items, left);
              real_position += aug->n_items;
            }
          real_position += g_list_model_get_n_items (parent->model);
        }
    }

  g_list_model_items_changed (G_LIST_MODEL (this), real_position, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

static void
gtk_flatten_list_model_clear_node (gpointer _node)
{
  FlattenNode *node= _node;

  g_signal_handlers_disconnect_by_func (node->model, gtk_flatten_list_model_items_changed_cb, node);
  g_object_unref (node->model);
}

static void
gtk_flatten_list_model_augment (GtkRbTree *flatten,
                                gpointer   _aug,
                                gpointer   _node,
                                gpointer   left,
                                gpointer   right)
{
  FlattenNode *node = _node;
  FlattenAugment *aug = _aug;

  aug->n_items = g_list_model_get_n_items (node->model);
  aug->n_models = 1;

  if (left)
    {
      FlattenAugment *left_aug = gtk_rb_tree_get_augment (flatten, left);
      aug->n_items += left_aug->n_items;
      aug->n_models += left_aug->n_models;
    }
  if (right)
    {
      FlattenAugment *right_aug = gtk_rb_tree_get_augment (flatten, right);
      aug->n_items += right_aug->n_items;
      aug->n_models += right_aug->n_models;
    }
}

static guint
gtk_flatten_list_model_add_items (GtkFlattenListModel *this,
                                  FlattenNode         *after,
                                  guint                position,
                                  guint                n)
{
  FlattenNode *node;
  guint added, i;

  added = 0;
  for (i = 0; i < n; i++)
    {
      node = gtk_rb_tree_insert_before (this->items, after);
      node->model = g_list_model_get_item (this->model, position + i);
      g_signal_connect (node->model,
                        "items-changed",
                        G_CALLBACK (gtk_flatten_list_model_items_changed_cb),
                        node);
      node->list = this;
      added += g_list_model_get_n_items (node->model);
    }

  return added;
}

static void
gtk_flatten_list_model_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  GtkFlattenListModel *this = GTK_FLATTEN_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_MODEL:
      gtk_flatten_list_model_set_model (this, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
gtk_flatten_list_model_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  GtkFlattenListModel *this = GTK_FLATTEN_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, gtk_flatten_list_model_get_item_type (G_LIST_MODEL (this)));
      break;

    case PROP_MODEL:
      g_value_set_object (value, this->model);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, gtk_flatten_list_model_get_n_items (G_LIST_MODEL (this)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_flatten_list_model_model_items_changed_cb (GListModel          *model,
                                               guint                position,
                                               guint                removed,
                                               guint                added,
                                               GtkFlattenListModel *this)
{
  FlattenNode *node;
  guint i, real_position, real_removed, real_added;

  node = gtk_flatten_list_model_get_nth_model (this->items, position, &real_position);

  real_removed = 0;
  for (i = 0; i < removed; i++)
    {
      FlattenNode *next = gtk_rb_tree_node_get_next (node);
      real_removed += g_list_model_get_n_items (node->model);
      gtk_rb_tree_remove (this->items, node);
      node = next;
    }

  real_added = gtk_flatten_list_model_add_items (this, node, position, added);

  if (real_removed > 0 || real_added > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), real_position, real_removed, real_added);
  if (real_removed != real_added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

static void
gtk_flatten_list_clear_model (GtkFlattenListModel *this)
{
  if (this->model)
    {
      g_signal_handlers_disconnect_by_func (this->model, gtk_flatten_list_model_model_items_changed_cb, this);
      g_clear_object (&this->model);
      g_clear_pointer (&this->items, gtk_rb_tree_unref);
    }
}

static void
gtk_flatten_list_model_dispose (GObject *object)
{
  GtkFlattenListModel *this = GTK_FLATTEN_LIST_MODEL (object);

  gtk_flatten_list_clear_model (this);

  G_OBJECT_CLASS (gtk_flatten_list_model_parent_class)->dispose (object);
}

static void
gtk_flatten_list_model_class_init (GtkFlattenListModelClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->set_property = gtk_flatten_list_model_set_property;
  gobject_class->get_property = gtk_flatten_list_model_get_property;
  gobject_class->dispose = gtk_flatten_list_model_dispose;

  /**
   * GtkFlattenListModel:item-type:
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
   * GtkFlattenListModel:model:
   *
   * The model being flattened.
   */
  properties[PROP_MODEL] =
      g_param_spec_object ("model", NULL, NULL,
                           G_TYPE_LIST_MODEL,
                           G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFlattenListModel:n-items:
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
gtk_flatten_list_model_init (GtkFlattenListModel *this)
{
}

/**
 * gtk_flatten_list_model_new:
 * @model: (nullable) (transfer full): the model to be flattened
 *
 * Creates a new `GtkFlattenListModel` that flattens @list.
 *
 * Returns: a new `GtkFlattenListModel`
 */
GtkFlattenListModel *
gtk_flatten_list_model_new (GListModel *model)
{
  GtkFlattenListModel *result;

  g_return_val_if_fail (model == NULL || G_IS_LIST_MODEL (model), NULL);

  result = g_object_new (GTK_TYPE_FLATTEN_LIST_MODEL,
                         "model", model,
                         NULL);

  /* we consume the reference */
  g_clear_object (&model);

  return result;
}

/**
 * gtk_flatten_list_model_set_model:
 * @this: a `GtkFlattenListModel`
 * @model: (nullable) (transfer none): the new model
 *
 * Sets a new model to be flattened.
 */
void
gtk_flatten_list_model_set_model (GtkFlattenListModel *this,
                                  GListModel          *model)
{
  guint removed, added = 0;

  g_return_if_fail (GTK_IS_FLATTEN_LIST_MODEL (this));
  g_return_if_fail (model == NULL || G_IS_LIST_MODEL (model));

  if (this->model == model)
    return;

  removed = g_list_model_get_n_items (G_LIST_MODEL (this)); 
  gtk_flatten_list_clear_model (this);

  this->model = model;

  if (model)
    {
      g_object_ref (model);
      g_signal_connect (model, "items-changed", G_CALLBACK (gtk_flatten_list_model_model_items_changed_cb), this);
      this->items = gtk_rb_tree_new (FlattenNode,
                                     FlattenAugment,
                                     gtk_flatten_list_model_augment,
                                     gtk_flatten_list_model_clear_node,
                                     NULL);

      added = gtk_flatten_list_model_add_items (this, NULL, 0, g_list_model_get_n_items (model));
    }

  if (removed > 0 || added > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), 0, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODEL]);
}

/**
 * gtk_flatten_list_model_get_model:
 * @this: a `GtkFlattenListModel`
 *
 * Gets the model set via gtk_flatten_list_model_set_model().
 *
 * Returns: (nullable) (transfer none): The model flattened by @this
 **/
GListModel *
gtk_flatten_list_model_get_model (GtkFlattenListModel *this)
{
  g_return_val_if_fail (GTK_IS_FLATTEN_LIST_MODEL (this), NULL);

  return this->model;
}

/**
 * gtk_flatten_list_model_get_model_for_item:
 * @this: a `GtkFlattenListModel`
 * @position: a position
 *
 * Returns the model containing the item at the given position.
 *
 * Returns: (transfer none) (nullable): the model containing the item at @position
 */
GListModel *
gtk_flatten_list_model_get_model_for_item (GtkFlattenListModel *this,
                                           guint                position)
{
  FlattenNode *node;

  if (!this->items)
    return NULL;

  node = gtk_flatten_list_model_get_nth (this->items, position, NULL);
  if (node == NULL)
    return NULL;

  return node->model;
}
