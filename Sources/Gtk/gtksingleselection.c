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

#include "gtksingleselection.h"

#include "gtkbitset.h"
#include "gtksectionmodelprivate.h"
#include "gtkselectionmodel.h"

/**
 * GtkSingleSelection:
 *
 * A selection model that allows selecting a single item.
 *
 * Note that the selection is *persistent* -- if the selected item is removed
 * and re-added in the same [signal@Gio.ListModel::items-changed] emission, it
 * stays selected. In particular, this means that changing the sort order of an
 * underlying sort model will preserve the selection.
 */
struct _GtkSingleSelection
{
  GObject parent_instance;

  GListModel *model;
  guint selected;
  gpointer selected_item;

  guint autoselect : 1;
  guint can_unselect : 1;
};

struct _GtkSingleSelectionClass
{
  GObjectClass parent_class;
};

enum {
  PROP_0,
  PROP_AUTOSELECT,
  PROP_CAN_UNSELECT,
  PROP_ITEM_TYPE,
  PROP_MODEL,
  PROP_N_ITEMS,
  PROP_SELECTED,
  PROP_SELECTED_ITEM,
  N_PROPS
};

static GParamSpec *properties[N_PROPS] = { NULL, };

static GType
gtk_single_selection_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_single_selection_get_n_items (GListModel *list)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (list);

  if (this->model == NULL)
    return 0;

  return g_list_model_get_n_items (this->model);
}

static gpointer
gtk_single_selection_get_item (GListModel *list,
                               guint       position)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (list);

  if (this->model == NULL)
    return NULL;

  return g_list_model_get_item (this->model, position);
}

static void
gtk_single_selection_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_single_selection_get_item_type;
  iface->get_n_items = gtk_single_selection_get_n_items;
  iface->get_item = gtk_single_selection_get_item;
}

static void
gtk_single_selection_get_section (GtkSectionModel *model,
                                  guint            position,
                                  guint           *out_start,
                                  guint           *out_end)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (model);

  gtk_list_model_get_section (this->model, position, out_start, out_end);
}

static void
gtk_single_selection_section_model_init (GtkSectionModelInterface *iface)
{
  iface->get_section = gtk_single_selection_get_section;
}

static gboolean
gtk_single_selection_is_selected (GtkSelectionModel *model,
                                  guint              position)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (model);

  return this->selected == position;
}

static GtkBitset *
gtk_single_selection_get_selection_in_range (GtkSelectionModel *model,
                                             guint              position,
                                             guint              n_items)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (model);
  GtkBitset *result;

  result = gtk_bitset_new_empty ();
  if (this->selected != GTK_INVALID_LIST_POSITION)
    gtk_bitset_add (result, this->selected);

  return result;
}

static gboolean
gtk_single_selection_select_item (GtkSelectionModel *model,
                                  guint              position,
                                  gboolean           exclusive)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (model);

  /* XXX: Should we check that position < n_items here? */
  gtk_single_selection_set_selected (this, position);

  return TRUE;
}

static gboolean
gtk_single_selection_unselect_item (GtkSelectionModel *model,
                                    guint              position)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (model);

  if (!this->can_unselect || this->autoselect)
    return FALSE;

  if (this->selected == position)
    gtk_single_selection_set_selected (this, GTK_INVALID_LIST_POSITION);

  return TRUE;
}

static gboolean
gtk_single_selection_unselect_all (GtkSelectionModel *model)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (model);

  return gtk_single_selection_unselect_item (model, this->selected);
}

static void
gtk_single_selection_selection_model_init (GtkSelectionModelInterface *iface)
{
  iface->is_selected = gtk_single_selection_is_selected; 
  iface->get_selection_in_range = gtk_single_selection_get_selection_in_range; 
  iface->select_item = gtk_single_selection_select_item; 
  iface->unselect_all = gtk_single_selection_unselect_all;
  iface->unselect_item = gtk_single_selection_unselect_item; 
}

G_DEFINE_TYPE_EXTENDED (GtkSingleSelection, gtk_single_selection, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL,
                                               gtk_single_selection_list_model_init)
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_SECTION_MODEL,
                                               gtk_single_selection_section_model_init)
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_SELECTION_MODEL,
                                               gtk_single_selection_selection_model_init))

static void
gtk_single_selection_items_changed_cb (GListModel         *model,
                                       guint               position,
                                       guint               removed,
                                       guint               added,
                                       GtkSingleSelection *this)
{
  g_object_freeze_notify (G_OBJECT (this));

  if (this->selected_item == NULL)
    {
      if (this->autoselect)
        {
          this->selected_item = g_list_model_get_item (this->model, 0);
          if (this->selected_item)
            {
              this->selected = 0;
              g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED]);
              g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED_ITEM]);
            }
        }
    }
  else if (this->selected < position)
    {
      /* unchanged */
    }
  else if (this->selected >= position + removed)
    {
      this->selected += added - removed;
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED]);
    }
  else
    {
      guint i;

      for (i = 0; i < added; i++)
        {
          gpointer item = g_list_model_get_item (model, position + i);
          if (item == this->selected_item)
            {
              /* the item moved */
              if (this->selected != position + i)
                {
                  this->selected = position + i;
                  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED]);
                }

              g_object_unref (item);
              break;
            }
          g_object_unref (item);
        }
      if (i == added)
        {
          guint old_selected = this->selected;

          /* the item really was deleted */
          g_clear_object (&this->selected_item);
          if (this->autoselect)
            {
              this->selected = position + (this->selected - position) * added / removed;
              this->selected_item = g_list_model_get_item (this->model, this->selected);
              if (this->selected_item == NULL)
                {
                  if (position > 0)
                    {
                      this->selected = position - 1;
                      this->selected_item = g_list_model_get_item (this->model, this->selected);
                      g_assert (this->selected_item);
                      /* We pretend the newly selected item was part of the original model change.
                       * This way we get around inconsistent state (no item selected) during
                       * the items-changed emission. */
                      position--;
                      removed++;
                      added++;
                    }
                  else
                    this->selected = GTK_INVALID_LIST_POSITION;
                }
              else
                {
                  if (this->selected == position + added)
                    {
                      /* We pretend the newly selected item was part of the original model change.
                       * This way we get around inconsistent state (no item selected) during
                       * the items-changed emission. */
                      removed++;
                      added++;
                    }
                }
            }
          else
            {
              g_clear_object (&this->selected_item);
              this->selected = GTK_INVALID_LIST_POSITION;
            }
          if (old_selected != this->selected)
            g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED]);
          /* the item was deleted above, so this is guaranteed to be new, even if the position didn't change */
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED_ITEM]);
        }
    }

  g_list_model_items_changed (G_LIST_MODEL (this), position, removed, added);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);

  g_object_thaw_notify (G_OBJECT (this));
}

static void
gtk_single_selection_sections_changed_cb (GtkSectionModel *model,
                                          unsigned int     position,
                                          unsigned int     n_items,
                                          gpointer         user_data)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (user_data);

  gtk_section_model_sections_changed (GTK_SECTION_MODEL (this), position, n_items);
}

static void
gtk_single_selection_clear_model (GtkSingleSelection *this)
{
  if (this->model == NULL)
    return;

  g_signal_handlers_disconnect_by_func (this->model,
                                        gtk_single_selection_items_changed_cb,
                                        this);
  g_signal_handlers_disconnect_by_func (this->model,
                                        gtk_single_selection_sections_changed_cb,
                                        this);
  g_clear_object (&this->model);
}

static void
gtk_single_selection_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)

{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (object);

  switch (prop_id)
    {
    case PROP_AUTOSELECT:
      gtk_single_selection_set_autoselect (this, g_value_get_boolean (value));
      break;

    case PROP_CAN_UNSELECT:
      gtk_single_selection_set_can_unselect (this, g_value_get_boolean (value));
      break;

    case PROP_MODEL:
      gtk_single_selection_set_model (this, g_value_get_object (value));
      break;

    case PROP_SELECTED:
      gtk_single_selection_set_selected (this, g_value_get_uint (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_single_selection_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (object);

  switch (prop_id)
    {
    case PROP_AUTOSELECT:
      g_value_set_boolean (value, this->autoselect);
      break;

    case PROP_CAN_UNSELECT:
      g_value_set_boolean (value, this->can_unselect);
      break;

    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, gtk_single_selection_get_item_type (G_LIST_MODEL (this)));
      break;

    case PROP_MODEL:
      g_value_set_object (value, this->model);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, gtk_single_selection_get_n_items (G_LIST_MODEL (this)));
      break;

    case PROP_SELECTED:
      g_value_set_uint (value, this->selected);
      break;

    case PROP_SELECTED_ITEM:
      g_value_set_object (value, this->selected_item);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_single_selection_dispose (GObject *object)
{
  GtkSingleSelection *this = GTK_SINGLE_SELECTION (object);

  gtk_single_selection_clear_model (this);

  this->selected = GTK_INVALID_LIST_POSITION;
  g_clear_object (&this->selected_item);

  G_OBJECT_CLASS (gtk_single_selection_parent_class)->dispose (object);
}

static void
gtk_single_selection_class_init (GtkSingleSelectionClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = gtk_single_selection_get_property;
  gobject_class->set_property = gtk_single_selection_set_property;
  gobject_class->dispose = gtk_single_selection_dispose;

  /**
   * GtkSingleSelection:autoselect:
   *
   * If the selection will always select an item.
   */
  properties[PROP_AUTOSELECT] =
    g_param_spec_boolean ("autoselect", NULL, NULL,
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkSingleSelection:can-unselect:
   *
   * If unselecting the selected item is allowed.
   */
  properties[PROP_CAN_UNSELECT] =
    g_param_spec_boolean ("can-unselect", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkSingleSelection:item-type:
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
   * GtkSingleSelection:model:
   *
   * The model being managed.
   */
  properties[PROP_MODEL] =
    g_param_spec_object ("model", NULL, NULL,
                         G_TYPE_LIST_MODEL,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSingleSelection:n-items:
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
   * GtkSingleSelection:selected:
   *
   * Position of the selected item.
   */
  properties[PROP_SELECTED] =
    g_param_spec_uint ("selected", NULL, NULL,
                       0, G_MAXUINT, GTK_INVALID_LIST_POSITION,
                       G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkSingleSelection:selected-item:
   *
   * The selected item.
   */
  properties[PROP_SELECTED_ITEM] =
    g_param_spec_object ("selected-item", NULL, NULL,
                         G_TYPE_OBJECT,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);
}

static void
gtk_single_selection_init (GtkSingleSelection *this)
{
  this->selected = GTK_INVALID_LIST_POSITION;
  this->autoselect = TRUE;
}

/**
 * gtk_single_selection_new:
 * @model: (nullable) (transfer full): the `GListModel` to manage
 *
 * Creates a new selection to handle @model.
 *
 * Returns: (transfer full) (type GtkSingleSelection): a new `GtkSingleSelection`
 */
GtkSingleSelection *
gtk_single_selection_new (GListModel *model)
{
  GtkSingleSelection *this;

  g_return_val_if_fail (model == NULL || G_IS_LIST_MODEL (model), NULL);

  this = g_object_new (GTK_TYPE_SINGLE_SELECTION,
                       "model", model,
                       NULL);

  /* consume the reference */
  g_clear_object (&model);

  return this;
}

/**
 * gtk_single_selection_get_model:
 * @this: a `GtkSingleSelection`
 *
 * Gets the model that @this is wrapping.
 *
 * Returns: (transfer none) (nullable): The model being wrapped
 */
GListModel *
gtk_single_selection_get_model (GtkSingleSelection *this)
{
  g_return_val_if_fail (GTK_IS_SINGLE_SELECTION (this), NULL);

  return this->model;
}

/**
 * gtk_single_selection_set_model:
 * @this: a `GtkSingleSelection`
 * @model: (nullable): A `GListModel` to wrap
 *
 * Sets the model that @this should wrap.
 *
 * If @model is %NULL, @this will be empty.
 */
void
gtk_single_selection_set_model (GtkSingleSelection *this,
                                GListModel         *model)
{
  guint n_items_before;

  g_return_if_fail (GTK_IS_SINGLE_SELECTION (this));
  g_return_if_fail (model == NULL || G_IS_LIST_MODEL (model));

  if (this->model == model)
    return;

  g_object_freeze_notify (G_OBJECT (this));

  n_items_before = this->model ? g_list_model_get_n_items (this->model) : 0;
  gtk_single_selection_clear_model (this);

  if (model)
    {
      this->model = g_object_ref (model);
      g_signal_connect (this->model, "items-changed",
                        G_CALLBACK (gtk_single_selection_items_changed_cb), this);
      if (GTK_IS_SECTION_MODEL (this->model))
        g_signal_connect (this->model, "sections-changed",
                          G_CALLBACK (gtk_single_selection_sections_changed_cb), this);
      gtk_single_selection_items_changed_cb (this->model,
                                             0,
                                             n_items_before,
                                             g_list_model_get_n_items (model),
                                             this);
    }
  else
    {
      if (this->selected != GTK_INVALID_LIST_POSITION)
        {
          this->selected = GTK_INVALID_LIST_POSITION;
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED]);
        }
      if (this->selected_item)
        {
          g_clear_object (&this->selected_item);
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED_ITEM]);
        }
      g_list_model_items_changed (G_LIST_MODEL (this), 0, n_items_before, 0);
      if (n_items_before)
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODEL]);

  g_object_thaw_notify (G_OBJECT (this));
}

/**
 * gtk_single_selection_get_selected:
 * @this: a `GtkSingleSelection`
 *
 * Gets the position of the selected item.
 *
 * If no item is selected, %GTK_INVALID_LIST_POSITION is returned.
 *
 * Returns: The position of the selected item
 */
guint
gtk_single_selection_get_selected (GtkSingleSelection *this)
{
  g_return_val_if_fail (GTK_IS_SINGLE_SELECTION (this), GTK_INVALID_LIST_POSITION);

  return this->selected;
}

/**
 * gtk_single_selection_set_selected:
 * @this: a `GtkSingleSelection`
 * @position: the item to select or %GTK_INVALID_LIST_POSITION
 *
 * Selects the item at the given position.
 *
 * If the list does not have an item at @position or
 * %GTK_INVALID_LIST_POSITION is given, the behavior depends on the
 * value of the [property@Gtk.SingleSelection:autoselect] property:
 * If it is set, no change will occur and the old item will stay
 * selected. If it is unset, the selection will be unset and no item
 * will be selected. This also applies if [property@Gtk.SingleSelection:can-unselect]
 * is set to %FALSE.
 */
void
gtk_single_selection_set_selected (GtkSingleSelection *this,
                                   guint               position)
{
  gpointer new_selected = NULL;
  guint old_position;

  g_return_if_fail (GTK_IS_SINGLE_SELECTION (this));

  if (this->selected == position)
    return;

  if (this->model)
    new_selected = g_list_model_get_item (this->model, position);

  if (new_selected == NULL)
    {
      if (!this->can_unselect || this->autoselect)
        return;

      position = GTK_INVALID_LIST_POSITION;
    }

  if (this->selected == position)
    return;

  old_position = this->selected;
  this->selected = position;
  g_clear_object (&this->selected_item);
  this->selected_item = new_selected;

  if (old_position == GTK_INVALID_LIST_POSITION)
    gtk_selection_model_selection_changed (GTK_SELECTION_MODEL (this), position, 1);
  else if (position == GTK_INVALID_LIST_POSITION)
    gtk_selection_model_selection_changed (GTK_SELECTION_MODEL (this), old_position, 1);
  else if (position < old_position)
    gtk_selection_model_selection_changed (GTK_SELECTION_MODEL (this), position, old_position - position + 1);
  else
    gtk_selection_model_selection_changed (GTK_SELECTION_MODEL (this), old_position, position - old_position + 1);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED]);
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SELECTED_ITEM]);
}

/**
 * gtk_single_selection_get_selected_item:
 * @this: a `GtkSingleSelection`
 *
 * Gets the selected item.
 *
 * If no item is selected, %NULL is returned.
 *
 * Returns: (transfer none) (type GObject) (nullable): The selected item
 */
gpointer
gtk_single_selection_get_selected_item (GtkSingleSelection *this)
{
  g_return_val_if_fail (GTK_IS_SINGLE_SELECTION (this), NULL);

  return this->selected_item;
}

/**
 * gtk_single_selection_get_autoselect:
 * @this: a `GtkSingleSelection`
 *
 * Checks if autoselect has been enabled or disabled via
 * gtk_single_selection_set_autoselect().
 *
 * Returns: %TRUE if autoselect is enabled
 **/
gboolean
gtk_single_selection_get_autoselect (GtkSingleSelection *this)
{
  g_return_val_if_fail (GTK_IS_SINGLE_SELECTION (this), TRUE);

  return this->autoselect;
}

/**
 * gtk_single_selection_set_autoselect:
 * @this: a `GtkSingleSelection`
 * @autoselect: %TRUE to always select an item
 *
 * Enables or disables autoselect.
 *
 * If @autoselect is %TRUE, @this will enforce that an item is always
 * selected. It will select a new item when the currently selected
 * item is deleted and it will disallow unselecting the current item.
 */
void
gtk_single_selection_set_autoselect (GtkSingleSelection *this,
                                     gboolean            autoselect)
{
  g_return_if_fail (GTK_IS_SINGLE_SELECTION (this));

  if (this->autoselect == autoselect)
    return;

  this->autoselect = autoselect;

  g_object_freeze_notify (G_OBJECT (this));
  
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_AUTOSELECT]);

  if (this->autoselect && !this->selected_item)
    gtk_single_selection_set_selected (this, 0);

  g_object_thaw_notify (G_OBJECT (this));
}

/**
 * gtk_single_selection_get_can_unselect:
 * @this: a `GtkSingleSelection`
 *
 * If %TRUE, gtk_selection_model_unselect_item() is supported and allows
 * unselecting the selected item.
 *
 * Returns: %TRUE to support unselecting
 */
gboolean
gtk_single_selection_get_can_unselect (GtkSingleSelection *this)
{
  g_return_val_if_fail (GTK_IS_SINGLE_SELECTION (this), FALSE);

  return this->can_unselect;
}

/**
 * gtk_single_selection_set_can_unselect:
 * @this: a `GtkSingleSelection`
 * @can_unselect: %TRUE to allow unselecting
 *
 * If %TRUE, unselecting the current item via
 * gtk_selection_model_unselect_item() is supported.
 *
 * Note that setting [property@Gtk.SingleSelection:autoselect] will
 * cause unselecting to not work, so it practically makes no sense
 * to set both at the same time.
 */
void
gtk_single_selection_set_can_unselect (GtkSingleSelection *this,
                                       gboolean            can_unselect)
{
  g_return_if_fail (GTK_IS_SINGLE_SELECTION (this));

  if (this->can_unselect == can_unselect)
    return;

  this->can_unselect = can_unselect;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_CAN_UNSELECT]);
}
