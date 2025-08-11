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
 * GtkPropertyLookupListModel:
 *
 * A list model that recursively looks up a property.
 *
 * `GtkPropertyLookupListModel` takes an object and a property and looks
 * up the next element using the property on the previous object.
 *
 * For example, one could use this list model with the [property@Gtk.Widget:parent]
 * property to get a list of a widget and all its ancestors.
 **/

#include "config.h"

#include "gtkpropertylookuplistmodelprivate.h"

#include "gtkprivate.h"

enum {
  PROP_0,
  PROP_ITEM_TYPE,
  PROP_N_ITEMS,
  PROP_OBJECT,
  PROP_PROPERTY,
  NUM_PROPERTIES
};

struct _GtkPropertyLookupListModel
{
  GObject parent_instance;

  GType item_type;
  char *property;
  GPtrArray *items; /* list of items - lazily expanded if there's a NULL sentinel */
};

struct _GtkPropertyLookupListModelClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static GType
gtk_property_lookup_list_model_get_item_type (GListModel *list)
{
  GtkPropertyLookupListModel *this = GTK_PROPERTY_LOOKUP_LIST_MODEL (list);

  return this->item_type;
}

static gboolean
gtk_property_lookup_list_model_is_infinite (GtkPropertyLookupListModel *this)
{
  if (this->items->len == 0)
    return FALSE;

  return g_ptr_array_index (this->items, this->items->len - 1) == NULL;
}

static void
gtk_property_lookup_list_model_notify_cb (GObject                    *object,
                                          GParamSpec                 *pspec,
                                          GtkPropertyLookupListModel *this);

static guint
gtk_property_lookup_list_model_clear (GtkPropertyLookupListModel *this,
                                      guint                       remaining)
{
  guint i;

  for (i = remaining; i < this->items->len; i++)
    {
      gpointer object = g_ptr_array_index (this->items, i);
      if (object == NULL)
        break;

      g_signal_handlers_disconnect_by_func (object, gtk_property_lookup_list_model_notify_cb, this);
      g_object_unref (object);
    }

  /* keeps the sentinel, yay! */
  g_ptr_array_remove_range (this->items, remaining, i - remaining);

  return i - remaining;
}

static guint
gtk_property_lookup_list_model_append (GtkPropertyLookupListModel *this,
                                       guint                       n_items)
{
  gpointer last, next;
  guint i, start;

  g_assert (this->items->len > 0);
  g_assert (!gtk_property_lookup_list_model_is_infinite (this));

  last = g_ptr_array_index (this->items, this->items->len - 1);
  start = this->items->len;
  for (i = start; i < n_items; i++)
    {
      g_object_get (last, this->property, &next, NULL);
      if (next == NULL)
        return i - start;
      
      g_signal_connect_closure_by_id (next,
                                      g_signal_lookup ("notify", G_OBJECT_TYPE (next)),
                                      g_quark_from_static_string (this->property),
                                      g_cclosure_new (G_CALLBACK (gtk_property_lookup_list_model_notify_cb), G_OBJECT (this), NULL),
                                      FALSE);

      g_ptr_array_add (this->items, next);
      last = next;
    }

  return i - start;
}

static void
gtk_property_lookup_list_model_ensure (GtkPropertyLookupListModel *this,
                                       guint                       n_items)
{
  if (!gtk_property_lookup_list_model_is_infinite (this))
    return;

  if (this->items->len - 1 >= n_items)
    return;
  
  /* remove NULL sentinel */
  g_ptr_array_remove_index (this->items, this->items->len - 1);

  if (this->items->len == 0)
    return;

  if (gtk_property_lookup_list_model_append (this, n_items) == n_items)
    {
      /* re-add NULL sentinel */
      g_ptr_array_add (this->items, NULL);
    }
}

static void
gtk_property_lookup_list_model_notify_cb (GObject                    *object,
                                          GParamSpec                 *pspec,
                                          GtkPropertyLookupListModel *this)
{
  guint position, removed, added;

  if (!g_ptr_array_find (this->items, object, &position))
    {
      /* can only happen if we forgot to disconnect a signal handler */
      g_assert_not_reached ();
    }
  /* we found the position of the item that notified, but the first change
   * is its child.
   */
  position++;

  removed = gtk_property_lookup_list_model_clear (this, position);

  if (!gtk_property_lookup_list_model_is_infinite (this))
    added = gtk_property_lookup_list_model_append (this, G_MAXUINT);
  else
    added = 0;

  if (removed > 0 || added > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), position, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

static guint
gtk_property_lookup_list_model_get_n_items (GListModel *list)
{
  GtkPropertyLookupListModel *this = GTK_PROPERTY_LOOKUP_LIST_MODEL (list);

  gtk_property_lookup_list_model_ensure (this, G_MAXUINT);

  return this->items->len;
}

static gpointer
gtk_property_lookup_list_model_get_item (GListModel *list,
                                         guint       position)
{
  GtkPropertyLookupListModel *this = GTK_PROPERTY_LOOKUP_LIST_MODEL (list);

  gtk_property_lookup_list_model_ensure (this, position + 1);

  if (position >= this->items->len)
    return NULL;

  return g_object_ref (g_ptr_array_index (this->items, position));
}

static void
gtk_property_lookup_list_model_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_property_lookup_list_model_get_item_type;
  iface->get_n_items = gtk_property_lookup_list_model_get_n_items;
  iface->get_item = gtk_property_lookup_list_model_get_item;
}

G_DEFINE_TYPE_WITH_CODE (GtkPropertyLookupListModel, gtk_property_lookup_list_model,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_property_lookup_list_model_list_model_init))

static gboolean
check_pspec (GType       type,
             GParamSpec *pspec)
{
  if (pspec == NULL)
    return FALSE;

  if (!G_IS_PARAM_SPEC_OBJECT (pspec))
    return FALSE;

  if (!g_type_is_a (G_PARAM_SPEC (pspec)->value_type, type))
    return FALSE;

  return TRUE;
}

static gboolean
lookup_pspec (GType       type,
              const char *name)
{
  gboolean result;

  if (g_type_is_a (type, G_TYPE_INTERFACE))
    {
      gpointer iface;
      GParamSpec *pspec;

      iface = g_type_default_interface_ref (type);
      pspec = g_object_interface_find_property (iface, name);
      result = check_pspec (type, pspec);
      g_type_default_interface_unref (iface);
    }
  else
    {
      GObjectClass *klass;
      GParamSpec *pspec;

      klass = g_type_class_ref (type);
      g_return_val_if_fail (klass != NULL, FALSE);
      pspec = g_object_class_find_property (klass, name);
      result = check_pspec (type, pspec);
      g_type_class_unref (klass);
    }

  return result;
}

static void
gtk_property_lookup_list_model_set_property (GObject      *object,
                                             guint         prop_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
{
  GtkPropertyLookupListModel *this = GTK_PROPERTY_LOOKUP_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_ITEM_TYPE:
      this->item_type = g_value_get_gtype (value);
      g_return_if_fail (this->item_type != 0);
      break;

    case PROP_OBJECT:
      gtk_property_lookup_list_model_set_object (this, g_value_get_object (value));
      break;

    case PROP_PROPERTY:
      this->property = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

  if (this->property && this->item_type &&
      !lookup_pspec (this->item_type, this->property))
    {
      g_critical ("type %s has no property named \"%s\"", g_type_name (this->item_type), this->property);
    }
}

static void 
gtk_property_lookup_list_model_get_property (GObject     *object,
                                             guint        prop_id,
                                             GValue      *value,
                                             GParamSpec  *pspec)
{
  GtkPropertyLookupListModel *this = GTK_PROPERTY_LOOKUP_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, this->item_type);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, gtk_property_lookup_list_model_get_n_items (G_LIST_MODEL (this)));
      break;

    case PROP_OBJECT:
      g_value_set_object (value, gtk_property_lookup_list_model_get_object (this));
      break;

    case PROP_PROPERTY:
      g_value_set_string (value, this->property);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_property_lookup_list_model_dispose (GObject *object)
{
  GtkPropertyLookupListModel *this = GTK_PROPERTY_LOOKUP_LIST_MODEL (object);

  gtk_property_lookup_list_model_clear (this, 0);

  G_OBJECT_CLASS (gtk_property_lookup_list_model_parent_class)->dispose (object);
}

static void
gtk_property_lookup_list_model_finalize (GObject *object)
{
  GtkPropertyLookupListModel *this = GTK_PROPERTY_LOOKUP_LIST_MODEL (object);

  g_ptr_array_unref (this->items);
  g_free (this->property);

  G_OBJECT_CLASS (gtk_property_lookup_list_model_parent_class)->finalize (object);
}

static void
gtk_property_lookup_list_model_class_init (GtkPropertyLookupListModelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = gtk_property_lookup_list_model_get_property;
  object_class->set_property = gtk_property_lookup_list_model_set_property;
  object_class->dispose = gtk_property_lookup_list_model_dispose;
  object_class->finalize = gtk_property_lookup_list_model_finalize;

  /**
   * GtkPropertyLookupListModel:item-type:
   *
   * The `GType` for elements of this object. See [method@Gio.ListModel.get_item_type].
   */
  properties[PROP_ITEM_TYPE] =
      g_param_spec_gtype ("item-type", NULL, NULL,
                          G_TYPE_OBJECT,
                          GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkPropertyLookupListModel:n-items:
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
   * GtkPropertyLookupListModel:property:
   *
   * Name of the property used for lookups
   */
  properties[PROP_PROPERTY] =
      g_param_spec_string ("property", NULL, NULL,
                           NULL,
                           GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkPropertyLookupListModel:object:
   *
   * The root object
   */
  properties[PROP_OBJECT] =
      g_param_spec_object ("object", NULL, NULL,
                           G_TYPE_OBJECT,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, NUM_PROPERTIES, properties);
}

static void
gtk_property_lookup_list_model_init (GtkPropertyLookupListModel *this)
{
  this->item_type = G_TYPE_OBJECT;
  this->items = g_ptr_array_new ();
  /* add sentinel */
  g_ptr_array_add (this->items, NULL);
}

GtkPropertyLookupListModel *
gtk_property_lookup_list_model_new (GType       item_type,
                                    const char *property_name)
{
  g_return_val_if_fail (g_type_is_a (item_type, G_TYPE_OBJECT), NULL);
  g_return_val_if_fail (property_name != NULL, NULL);

  return g_object_new (GTK_TYPE_PROPERTY_LOOKUP_LIST_MODEL,
                       "item-type", item_type,
                       "property", property_name,
                       NULL);
}

void
gtk_property_lookup_list_model_set_object (GtkPropertyLookupListModel *this,
                                           gpointer                    object)
{
  guint removed, added;

  g_return_if_fail (GTK_IS_PROPERTY_LOOKUP_LIST_MODEL (this));

  if (object)
    {
      if (this->items->len != 0 &&
          g_ptr_array_index (this->items, 0) == object)
        return;

      removed = gtk_property_lookup_list_model_clear (this, 0);

      g_ptr_array_insert (this->items, 0, g_object_ref (object));
      g_signal_connect_closure_by_id (object,
                                      g_signal_lookup ("notify", G_OBJECT_TYPE (object)),
                                      g_quark_from_static_string (this->property),
                                      g_cclosure_new (G_CALLBACK (gtk_property_lookup_list_model_notify_cb), G_OBJECT (this), NULL),
                                      FALSE);
      added = 1;
      if (!gtk_property_lookup_list_model_is_infinite (this))
        added += gtk_property_lookup_list_model_append (this, G_MAXUINT);
    }
  else
    {
      if (this->items->len == 0 ||
          g_ptr_array_index (this->items, 0) == NULL)
        return;

      removed = gtk_property_lookup_list_model_clear (this, 0);
      added = 0;
    }

  g_assert (removed != 0 || added != 0);

  g_list_model_items_changed (G_LIST_MODEL (this), 0, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

gpointer
gtk_property_lookup_list_model_get_object (GtkPropertyLookupListModel *this)
{
  g_return_val_if_fail (GTK_IS_PROPERTY_LOOKUP_LIST_MODEL (this), NULL);

  if (this->items->len == 0)
    return NULL;
  
  return g_ptr_array_index (this->items, 0);
}

