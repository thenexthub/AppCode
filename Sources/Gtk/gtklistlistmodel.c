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


/*
 * GtkListListModel:
 *
 * A list model that takes a list API and provides it as a `GListModel`.
 */

#include "config.h"

#include "gtklistlistmodelprivate.h"

struct _GtkListListModel
{
  GObject parent_instance;

  guint n_items;
  gpointer (* get_first) (gpointer);
  gpointer (* get_next) (gpointer, gpointer);
  gpointer (* get_previous) (gpointer, gpointer);
  gpointer (* get_last) (gpointer);
  gpointer (* get_item) (gpointer, gpointer);
  gpointer data;
  GDestroyNotify notify;

  guint cache_pos;
  gpointer cache_item;
};

struct _GtkListListModelClass
{
  GObjectClass parent_class;
};

enum {
  PROP_0,
  PROP_ITEM_TYPE,
  PROP_N_ITEMS,

  N_PROPS
};

static GParamSpec *properties[N_PROPS] = { NULL, };

static GType
gtk_list_list_model_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_list_list_model_get_n_items (GListModel *list)
{
  GtkListListModel *this = GTK_LIST_LIST_MODEL (list);

  return this->n_items;
}

static gboolean
gtk_list_list_model_cache_is_valid (GtkListListModel *this)
{
  return this->cache_item != NULL;
}

static void
gtk_list_list_model_invalidate_cache (GtkListListModel *this)
{
  this->cache_item = NULL;
}

static gpointer
gtk_list_list_model_get_item (GListModel *list,
                              guint       position)
{
  GtkListListModel *this = GTK_LIST_LIST_MODEL (list);
  gpointer result;
  guint i;
  guint start, end;

  if (position >= this->n_items)
    return NULL;

  start = 0;
  end = this->n_items;
  if (gtk_list_list_model_cache_is_valid (this))
    {
      if (this->cache_pos <= position)
        start = this->cache_pos;
      else
        end = this->cache_pos;
    }

  if (this->get_last &&
      position > (start + end) / 2)
    {
      if (end == this->cache_pos && gtk_list_list_model_cache_is_valid (this))
        result = this->get_previous (this->cache_item, this->data);
      else
        result = this->get_last (this->data);

      for (i = end - 1; i > position; i--)
        {
          result = this->get_previous (result, this->data);
        }
    }
  else
    {
      if (start == this->cache_pos && gtk_list_list_model_cache_is_valid (this))
        result = this->cache_item;
      else
        result = this->get_first (this->data);

      for (i = start; i < position; i++)
        {
          result = this->get_next (result, this->data);
        }
    }

  this->cache_item = result;
  this->cache_pos = position;

  return this->get_item (result, this->data);
}

static void
gtk_list_list_model_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_list_list_model_get_item_type;
  iface->get_n_items = gtk_list_list_model_get_n_items;
  iface->get_item = gtk_list_list_model_get_item;
}

G_DEFINE_TYPE_WITH_CODE (GtkListListModel, gtk_list_list_model,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_list_list_model_list_model_init))

static void
gtk_list_list_model_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GtkListListModel *this = GTK_LIST_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, G_TYPE_OBJECT);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, this->n_items);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_list_list_model_dispose (GObject *object)
{
  GtkListListModel *this = GTK_LIST_LIST_MODEL (object);

  if (this->notify)
    this->notify (this->data);

  this->n_items = 0;
  this->notify = NULL;

  G_OBJECT_CLASS (gtk_list_list_model_parent_class)->dispose (object);
}

static void
gtk_list_list_model_class_init (GtkListListModelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = gtk_list_list_model_get_property;
  object_class->dispose = gtk_list_list_model_dispose;

  properties[PROP_ITEM_TYPE] =
    g_param_spec_gtype ("item-type", NULL, NULL,
                        G_TYPE_OBJECT,
                        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  properties[PROP_N_ITEMS] =
    g_param_spec_uint ("n-items", NULL, NULL,
                       0, G_MAXUINT, 0,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gtk_list_list_model_init (GtkListListModel *this)
{
}

GtkListListModel *
gtk_list_list_model_new (gpointer       (* get_first) (gpointer),
                         gpointer       (* get_next) (gpointer, gpointer),
                         gpointer       (* get_previous) (gpointer, gpointer),
                         gpointer       (* get_last) (gpointer),
                         gpointer       (* get_item) (gpointer, gpointer),
                         gpointer       data,
                         GDestroyNotify notify)
{
  guint n_items;
  gpointer item;

  n_items = 0;
  for (item = get_first (data);
       item != NULL;
       item = get_next (item, data))
    n_items++;

  return gtk_list_list_model_new_with_size (n_items,
                                            get_first,
                                            get_next,
                                            get_previous,
                                            get_last,
                                            get_item,
                                            data,
                                            notify);
}

GtkListListModel *
gtk_list_list_model_new_with_size (guint          n_items,
                                   gpointer       (* get_first) (gpointer),
                                   gpointer       (* get_next) (gpointer, gpointer),
                                   gpointer       (* get_previous) (gpointer, gpointer),
                                   gpointer       (* get_last) (gpointer),
                                   gpointer       (* get_item) (gpointer, gpointer),
                                   gpointer       data,
                                   GDestroyNotify notify)
{
  GtkListListModel *result;

  g_return_val_if_fail (get_first != NULL, NULL);
  g_return_val_if_fail (get_next != NULL, NULL);
  g_return_val_if_fail (get_previous != NULL, NULL);
  g_return_val_if_fail (get_item != NULL, NULL);

  result = g_object_new (GTK_TYPE_LIST_LIST_MODEL, NULL);

  result->n_items = n_items;
  result->get_first = get_first;
  result->get_next = get_next;
  result->get_previous = get_previous;
  result->get_last = get_last;
  result->get_item = get_item;
  result->data = data;
  result->notify = notify;

  return result;
}

static guint
gtk_list_list_model_find (GtkListListModel *this,
                          gpointer          item)
{
  guint position;
  gpointer x;

  position = 0;
  for (x = this->get_first (this->data);
       x != item;
       x = this->get_next (x, this->data))
    position++;

  return position;
}

void
gtk_list_list_model_item_added (GtkListListModel *this,
                                gpointer          item)
{
  g_return_if_fail (GTK_IS_LIST_LIST_MODEL (this));
  g_return_if_fail (item != NULL);

  gtk_list_list_model_item_added_at (this, gtk_list_list_model_find (this, item));
}

void
gtk_list_list_model_item_added_at (GtkListListModel *this,
                                   guint             position)
{
  g_return_if_fail (GTK_IS_LIST_LIST_MODEL (this));
  g_return_if_fail (position <= this->n_items);

  this->n_items++;
  if (position <= this->cache_pos)
    this->cache_pos++;

  g_list_model_items_changed (G_LIST_MODEL (this), position, 0, 1);
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

void
gtk_list_list_model_item_removed (GtkListListModel *this,
                                  gpointer          previous)
{
  guint position;

  g_return_if_fail (GTK_IS_LIST_LIST_MODEL (this));

  if (previous == NULL)
    position = 0;
  else
    position = 1 + gtk_list_list_model_find (this, previous);

  gtk_list_list_model_item_removed_at (this, position);
}

void
gtk_list_list_model_item_moved (GtkListListModel *this,
                                gpointer          item,
                                gpointer          previous_previous)
{
  guint position, previous_position;
  guint min, max;

  g_return_if_fail (GTK_IS_LIST_LIST_MODEL (this));
  g_return_if_fail (item != previous_previous);

  position = gtk_list_list_model_find (this, item);

  if (previous_previous == NULL)
    {
      previous_position = 0;
    }
  else
    {
      previous_position = gtk_list_list_model_find (this, previous_previous);
      if (position > previous_position)
        previous_position++;
    }

  /* item didn't move */
  if (position == previous_position)
    return;

  min = MIN (position, previous_position);
  max = MAX (position, previous_position) + 1;

  if (this->cache_item == item)
    this->cache_pos = position;
  else if (this->cache_pos >= min && this->cache_pos < max)
    this->cache_pos += (this->cache_pos > position ? 1 : -1);

  g_list_model_items_changed (G_LIST_MODEL (this), min, max - min, max - min);
}

void
gtk_list_list_model_item_removed_at (GtkListListModel *this,
                                     guint             position)
{
  g_return_if_fail (GTK_IS_LIST_LIST_MODEL (this));
  g_return_if_fail (position < this->n_items);

  this->n_items -= 1;
  if (position == this->cache_pos)
    gtk_list_list_model_invalidate_cache (this);
  else if (position < this->cache_pos)
    this->cache_pos--;

  g_list_model_items_changed (G_LIST_MODEL (this), position, 1, 0);
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

void
gtk_list_list_model_clear (GtkListListModel *this)
{
  guint n_items;

  g_return_if_fail (GTK_IS_LIST_LIST_MODEL (this));

  n_items = this->n_items;
  
  if (this->notify)
    this->notify (this->data);

  this->n_items = 0;
  this->notify = NULL;

  gtk_list_list_model_invalidate_cache (this);

  if (n_items > 0)
    {
      g_list_model_items_changed (G_LIST_MODEL (this), 0, n_items, 0);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }
}


