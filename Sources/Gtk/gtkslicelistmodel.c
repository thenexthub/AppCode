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

#include "gtkslicelistmodel.h"
#include "gtksectionmodelprivate.h"

#include "gtkprivate.h"

/**
 * GtkSliceListModel:
 *
 * A list model that presents a slice of another model.
 *
 * This is useful when implementing paging by setting the size to the number
 * of elements per page and updating the offset whenever a different page is
 * opened.
 *
 * `GtkSliceListModel` passes through sections from the underlying model.
 */

#define DEFAULT_SIZE 10

enum {
  PROP_0,
  PROP_ITEM_TYPE,
  PROP_MODEL,
  PROP_N_ITEMS,
  PROP_OFFSET,
  PROP_SIZE,
  NUM_PROPERTIES
};

struct _GtkSliceListModel
{
  GObject parent_instance;

  GListModel *model;
  guint offset;
  guint size;
};

struct _GtkSliceListModelClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static GType
gtk_slice_list_model_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_slice_list_model_get_n_items (GListModel *list)
{
  GtkSliceListModel *this = GTK_SLICE_LIST_MODEL (list);
  guint n_items;
  
  if (this->model == NULL)
    return 0;

  /* XXX: This can be done without calling g_list_model_get_n_items() on the parent model
   * by checking if model.get_item(offset + size) != NULL */
  n_items = g_list_model_get_n_items (this->model);
  if (n_items <= this->offset)
    return 0;

  n_items -= this->offset;
  return MIN (n_items, this->size);
}

static gpointer
gtk_slice_list_model_get_item (GListModel *list,
                               guint       position)
{
  GtkSliceListModel *this = GTK_SLICE_LIST_MODEL (list);

  if (this->model == NULL)
    return NULL;

  if (position >= this->size)
    return NULL;

  return g_list_model_get_item (this->model, position + this->offset);
}

static void
gtk_slice_list_model_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_slice_list_model_get_item_type;
  iface->get_n_items = gtk_slice_list_model_get_n_items;
  iface->get_item = gtk_slice_list_model_get_item;
}

static void
gtk_slice_list_model_get_section (GtkSectionModel *model,
                                  guint            position,
                                  guint           *start,
                                  guint           *end)
{
  GtkSliceListModel *this = GTK_SLICE_LIST_MODEL (model);
  unsigned int n_items;

  n_items = g_list_model_get_n_items (G_LIST_MODEL (this));
  if (position >= n_items)
    {
      *start = n_items;
      *end = G_MAXUINT;
    }
  else
    {
      gtk_list_model_get_section (this->model, position + this->offset, start, end);

      *start = MAX (*start, this->offset) - this->offset;
      *end = MIN (*end - this->offset, n_items);
    }
}

static void
gtk_slice_list_model_sections_changed_cb (GtkSectionModel *model,
                                          unsigned int     position,
                                          unsigned int     n_items,
                                          gpointer         user_data)
{
  GtkSliceListModel *this = GTK_SLICE_LIST_MODEL (user_data);
  unsigned int start = position;
  unsigned int end = position + n_items;
  unsigned int size;

  if (end <= this->offset)
    return;

  size = g_list_model_get_n_items (G_LIST_MODEL (this));

  end = MIN (end - this->offset, size);

  if (start <= this->offset)
    start = 0;
  else
    start = start - this->offset;

  if (start >= size)
    return;

  gtk_section_model_sections_changed (GTK_SECTION_MODEL (this), start, end - start);
}

static void
gtk_slice_list_model_section_model_init (GtkSectionModelInterface *iface)
{
  iface->get_section = gtk_slice_list_model_get_section;
}

G_DEFINE_TYPE_WITH_CODE (GtkSliceListModel, gtk_slice_list_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_slice_list_model_model_init)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_SECTION_MODEL, gtk_slice_list_model_section_model_init))


static void
gtk_slice_list_model_items_changed_cb (GListModel        *model,
                                       guint              position,
                                       guint              removed,
                                       guint              added,
                                       GtkSliceListModel *this)
{
  if (position >= this->offset + this->size)
    return;

  if (position < this->offset)
    {
      guint skip = MIN (removed, added);
      skip = MIN (skip, this->offset - position);

      position += skip;
      removed -= skip;
      added -= skip;
    }

  if (removed == added)
    {
      guint changed = removed;

      if (changed == 0)
        return;

      g_assert (position >= this->offset);
      position -= this->offset;
      changed = MIN (changed, this->size - position);

      g_list_model_items_changed (G_LIST_MODEL (this), position, changed, changed);
    }
  else
    {
      guint n_after, n_before;
      guint skip;

      if (position > this->offset)
        skip = position - this->offset;
      else
        skip = 0;

      n_after = g_list_model_get_n_items (this->model);
      n_before = n_after - added + removed;
      n_after = CLAMP (n_after, this->offset, this->offset + this->size) - this->offset;
      n_before = CLAMP (n_before, this->offset, this->offset + this->size) - this->offset;

      g_list_model_items_changed (G_LIST_MODEL (this), skip, n_before - skip, n_after - skip);
      if (n_before != n_after)
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }
}

static void
gtk_slice_list_model_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GtkSliceListModel *this = GTK_SLICE_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_MODEL:
      gtk_slice_list_model_set_model (this, g_value_get_object (value));
      break;

    case PROP_OFFSET:
      gtk_slice_list_model_set_offset (this, g_value_get_uint (value));
      break;

    case PROP_SIZE:
      gtk_slice_list_model_set_size (this, g_value_get_uint (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
gtk_slice_list_model_get_property (GObject     *object,
                                   guint        prop_id,
                                   GValue      *value,
                                   GParamSpec  *pspec)
{
  GtkSliceListModel *this = GTK_SLICE_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, gtk_slice_list_model_get_item_type (G_LIST_MODEL (this)));
      break;

    case PROP_MODEL:
      g_value_set_object (value, this->model);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, gtk_slice_list_model_get_n_items (G_LIST_MODEL (this)));
      break;

    case PROP_OFFSET:
      g_value_set_uint (value, this->offset);
      break;

    case PROP_SIZE:
      g_value_set_uint (value, this->size);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_slice_list_model_clear_model (GtkSliceListModel *this)
{
  if (this->model == NULL)
    return;

  g_signal_handlers_disconnect_by_func (this->model, gtk_slice_list_model_sections_changed_cb, this);
  g_signal_handlers_disconnect_by_func (this->model, gtk_slice_list_model_items_changed_cb, this);
  g_clear_object (&this->model);
}

static void
gtk_slice_list_model_dispose (GObject *object)
{
  GtkSliceListModel *this = GTK_SLICE_LIST_MODEL (object);

  gtk_slice_list_model_clear_model (this);

  G_OBJECT_CLASS (gtk_slice_list_model_parent_class)->dispose (object);
};

static void
gtk_slice_list_model_class_init (GtkSliceListModelClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->set_property = gtk_slice_list_model_set_property;
  gobject_class->get_property = gtk_slice_list_model_get_property;
  gobject_class->dispose = gtk_slice_list_model_dispose;

  /**
   * GtkSliceListModel:item-type:
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
   * GtkSliceListModel:model:
   *
   * Child model to take slice from.
   */
  properties[PROP_MODEL] =
      g_param_spec_object ("model", NULL, NULL,
                           G_TYPE_LIST_MODEL,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSliceListModel:n-items:
   *
   * The number of items. See [method@Gio.ListModel.get_n_items].
   *
   * Since: 4.8
   **/
  properties[PROP_N_ITEMS] =
    g_param_spec_uint ("n-items", NULL, NULL,
                       0, G_MAXUINT, 0,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkSliceListModel:offset:
   *
   * Offset of slice.
   */
  properties[PROP_OFFSET] =
      g_param_spec_uint ("offset", NULL, NULL,
                         0, G_MAXUINT, 0,
                         GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSliceListModel:size:
   *
   * Maximum size of slice.
   */
  properties[PROP_SIZE] =
      g_param_spec_uint ("size", NULL, NULL,
                         0, G_MAXUINT, DEFAULT_SIZE,
                         GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (gobject_class, NUM_PROPERTIES, properties);
}

static void
gtk_slice_list_model_init (GtkSliceListModel *this)
{
  this->size = DEFAULT_SIZE;
}

/**
 * gtk_slice_list_model_new:
 * @model: (transfer full) (nullable): The model to use
 * @offset: the offset of the slice
 * @size: maximum size of the slice
 *
 * Creates a new slice model.
 *
 * It presents the slice from @offset to offset + @size
 * of the given @model.
 *
 * Returns: A new `GtkSliceListModel`
 */
GtkSliceListModel *
gtk_slice_list_model_new (GListModel *model,
                          guint       offset,
                          guint       size)
{
  GtkSliceListModel *this;

  g_return_val_if_fail (model == NULL || G_IS_LIST_MODEL (model), NULL);

  this = g_object_new (GTK_TYPE_SLICE_LIST_MODEL,
                       "model", model,
                       "offset", offset,
                       "size", size,
                       NULL);

  /* consume the reference */
  g_clear_object (&model);

  return this;
}

/**
 * gtk_slice_list_model_set_model:
 * @this: a `GtkSliceListModel`
 * @model: (nullable): The model to be sliced
 *
 * Sets the model to show a slice of.
 *
 * The model's item type must conform to @this's item type.
 */
void
gtk_slice_list_model_set_model (GtkSliceListModel *this,
                                GListModel      *model)
{
  guint removed, added;

  g_return_if_fail (GTK_IS_SLICE_LIST_MODEL (this));
  g_return_if_fail (model == NULL || G_IS_LIST_MODEL (model));

  if (this->model == model)
    return;

  removed = g_list_model_get_n_items (G_LIST_MODEL (this));
  gtk_slice_list_model_clear_model (this);

  if (model)
    {
      this->model = g_object_ref (model);
      g_signal_connect (model, "items-changed", G_CALLBACK (gtk_slice_list_model_items_changed_cb), this);
      added = g_list_model_get_n_items (G_LIST_MODEL (this));

      if (GTK_IS_SECTION_MODEL (model))
        g_signal_connect (model, "sections-changed", G_CALLBACK (gtk_slice_list_model_sections_changed_cb), this);
    }
  else
    {
      added = 0;
    }

  if (removed > 0 || added > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), 0, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODEL]);
}

/**
 * gtk_slice_list_model_get_model:
 * @this: a `GtkSliceListModel`
 *
 * Gets the model that is currently being used or %NULL if none.
 *
 * Returns: (nullable) (transfer none): The model in use
 */
GListModel *
gtk_slice_list_model_get_model (GtkSliceListModel *this)
{
  g_return_val_if_fail (GTK_IS_SLICE_LIST_MODEL (this), NULL);

  return this->model;
}

/**
 * gtk_slice_list_model_set_offset:
 * @this: a `GtkSliceListModel`
 * @offset: the new offset to use
 *
 * Sets the offset into the original model for this slice.
 *
 * If the offset is too large for the sliced model,
 * @this will end up empty.
 */
void
gtk_slice_list_model_set_offset (GtkSliceListModel *this,
                                 guint              offset)
{
  guint before, after;

  g_return_if_fail (GTK_IS_SLICE_LIST_MODEL (this));

  if (this->offset == offset)
    return;

  before = g_list_model_get_n_items (G_LIST_MODEL (this));

  this->offset = offset;

  after = g_list_model_get_n_items (G_LIST_MODEL (this));

  if (before > 0 || after > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), 0, before, after);
  if (before != after)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_OFFSET]);
}

/**
 * gtk_slice_list_model_get_offset:
 * @this: a `GtkSliceListModel`
 *
 * Gets the offset set via gtk_slice_list_model_set_offset().
 *
 * Returns: The offset
 */
guint
gtk_slice_list_model_get_offset (GtkSliceListModel *this)
{
  g_return_val_if_fail (GTK_IS_SLICE_LIST_MODEL (this), 0);

  return this->offset;
}

/**
 * gtk_slice_list_model_set_size:
 * @this: a `GtkSliceListModel`
 * @size: the maximum size
 *
 * Sets the maximum size. @this will never have more items
 * than @size.
 *
 * It can however have fewer items if the offset is too large
 * or the model sliced from doesn't have enough items.
 */
void
gtk_slice_list_model_set_size (GtkSliceListModel *this,
                               guint              size)
{
  guint before, after;

  g_return_if_fail (GTK_IS_SLICE_LIST_MODEL (this));

  if (this->size == size)
    return;

  before = g_list_model_get_n_items (G_LIST_MODEL (this));

  this->size = size;

  after = g_list_model_get_n_items (G_LIST_MODEL (this));

  if (before > after)
    {
      g_list_model_items_changed (G_LIST_MODEL (this), after, before - after, 0);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }
  else if (before < after)
    {
      g_list_model_items_changed (G_LIST_MODEL (this), before, 0, after - before);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }
  /* else nothing */

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SIZE]);
}

/**
 * gtk_slice_list_model_get_size:
 * @this: a `GtkSliceListModel`
 *
 * Gets the size set via gtk_slice_list_model_set_size().
 *
 * Returns: The size
 */
guint
gtk_slice_list_model_get_size (GtkSliceListModel *this)
{
  g_return_val_if_fail (GTK_IS_SLICE_LIST_MODEL (this), DEFAULT_SIZE);

  return this->size;
}
