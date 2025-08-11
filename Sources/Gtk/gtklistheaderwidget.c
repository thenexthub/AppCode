/*
 * Copyright © 2023 Benjamin Otte
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

#include "gtklistheaderwidgetprivate.h"

#include "gtkbinlayout.h"
#include "gtklistheaderprivate.h"
#include "gtklistitemfactoryprivate.h"
#include "gtklistbaseprivate.h"
#include "gtkwidget.h"

typedef struct _GtkListHeaderWidgetPrivate GtkListHeaderWidgetPrivate;
struct _GtkListHeaderWidgetPrivate
{
  GtkListItemFactory *factory;

  GtkListHeader *header;
};

enum {
  PROP_0,
  PROP_FACTORY,

  N_PROPS
};

G_DEFINE_TYPE_WITH_PRIVATE (GtkListHeaderWidget, gtk_list_header_widget, GTK_TYPE_LIST_HEADER_BASE)

static GParamSpec *properties[N_PROPS] = { NULL, };

static void
gtk_list_header_widget_setup_func (gpointer object,
                                   gpointer data)
{
  GtkListHeaderWidget *this = GTK_LIST_HEADER_WIDGET (data);
  GtkListHeaderWidgetPrivate *priv = gtk_list_header_widget_get_instance_private (this);
  GtkListHeader *header = object;

  priv->header = header;
  header->owner = this;

  gtk_list_header_widget_set_child (this, header->child);

  gtk_list_header_do_notify (header,
                             gtk_list_header_base_get_item (GTK_LIST_HEADER_BASE (this)) != NULL,
                             gtk_list_header_base_get_start (GTK_LIST_HEADER_BASE (this)) != GTK_INVALID_LIST_POSITION,
                             gtk_list_header_base_get_end (GTK_LIST_HEADER_BASE (this)) != GTK_INVALID_LIST_POSITION,
                             gtk_list_header_base_get_start (GTK_LIST_HEADER_BASE (this)) != gtk_list_header_base_get_end (GTK_LIST_HEADER_BASE (this)));
}

static void
gtk_list_header_widget_setup_factory (GtkListHeaderWidget *this)
{
  GtkListHeaderWidgetPrivate *priv = gtk_list_header_widget_get_instance_private (this);
  GtkListHeader *header;

  header = gtk_list_header_new ();

  gtk_list_item_factory_setup (priv->factory,
                               G_OBJECT (header),
                               gtk_list_header_base_get_item (GTK_LIST_HEADER_BASE (this)) != NULL,
                               gtk_list_header_widget_setup_func,
                               this);

  g_assert (priv->header == header);
}

static void
gtk_list_header_widget_teardown_func (gpointer object,
                                      gpointer data)
{
  GtkListHeaderWidget *this = GTK_LIST_HEADER_WIDGET (data);
  GtkListHeaderWidgetPrivate *priv = gtk_list_header_widget_get_instance_private (this);
  GtkListHeader *header = object;

  header->owner = NULL;
  priv->header = NULL;

  gtk_list_header_widget_set_child (this, NULL);

  gtk_list_header_do_notify (header,
                             gtk_list_header_base_get_item (GTK_LIST_HEADER_BASE (this)) != NULL,
                             gtk_list_header_base_get_start (GTK_LIST_HEADER_BASE (this)) != GTK_INVALID_LIST_POSITION,
                             gtk_list_header_base_get_end (GTK_LIST_HEADER_BASE (this)) != GTK_INVALID_LIST_POSITION,
                             gtk_list_header_base_get_start (GTK_LIST_HEADER_BASE (this)) != gtk_list_header_base_get_end (GTK_LIST_HEADER_BASE (this)));
}

static void
gtk_list_header_widget_teardown_factory (GtkListHeaderWidget *this)
{
  GtkListHeaderWidgetPrivate *priv = gtk_list_header_widget_get_instance_private (this);
  gpointer header = priv->header;

  gtk_list_item_factory_teardown (priv->factory,
                                  header,
                                  gtk_list_header_base_get_item (GTK_LIST_HEADER_BASE (this)) != NULL,
                                  gtk_list_header_widget_teardown_func,
                                  this);

  g_assert (priv->header == NULL);
  g_object_unref (header);
}

typedef struct {
  GtkListHeaderWidget *widget;
  gpointer item;
  guint start;
  guint end;
} GtkListHeaderWidgetUpdate;

static void
gtk_list_header_widget_update_func (gpointer object,
                                    gpointer data)
{
  GtkListHeaderWidgetUpdate *update = data;
  GtkListHeaderWidget *this = update->widget;
  GtkListHeaderBase *base = GTK_LIST_HEADER_BASE (this);
  /* Track notify manually instead of freeze/thaw_notify for performance reasons. */
  gboolean notify_item, notify_start, notify_end, notify_n_items;

  /* FIXME: It's kinda evil to notify external objects from here... */
  notify_item = gtk_list_header_base_get_item (base) != update->item;
  notify_start = gtk_list_header_base_get_start (base) != update->start;
  notify_end = gtk_list_header_base_get_end (base) != update->end;
  notify_n_items = gtk_list_header_base_get_end (base) - gtk_list_header_base_get_start (base) != update->end - update->start;

  GTK_LIST_HEADER_BASE_CLASS (gtk_list_header_widget_parent_class)->update (base,
                                                                            update->item,
                                                                            update->start,
                                                                            update->end);

  if (object)
    gtk_list_header_do_notify (object, notify_item, notify_start, notify_end, notify_n_items);
}

static void
gtk_list_header_widget_update (GtkListHeaderBase *base,
                               gpointer           item,
                               guint              start,
                               guint              end)
{
  GtkListHeaderWidget *this = GTK_LIST_HEADER_WIDGET (base);
  GtkListHeaderWidgetPrivate *priv = gtk_list_header_widget_get_instance_private (this);
  GtkListHeaderWidgetUpdate update = { this, item, start, end };

  if (priv->header)
    {
      gtk_list_item_factory_update (priv->factory,
                                    G_OBJECT (priv->header),
                                    gtk_list_header_base_get_item (GTK_LIST_HEADER_BASE (this)) != NULL,
                                    item != NULL,
                                    gtk_list_header_widget_update_func,
                                    &update);
    }
  else
    {
      gtk_list_header_widget_update_func (NULL, &update);
    }
}

static void
gtk_list_header_widget_set_property (GObject      *object,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  GtkListHeaderWidget *this = GTK_LIST_HEADER_WIDGET (object);

  switch (property_id)
    {
    case PROP_FACTORY:
      gtk_list_header_widget_set_factory (this, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_list_header_widget_clear_factory (GtkListHeaderWidget *this)
{
  GtkListHeaderWidgetPrivate *priv = gtk_list_header_widget_get_instance_private (this);

  if (priv->factory == NULL)
    return;

  if (priv->header)
    gtk_list_header_widget_teardown_factory (this);

  g_clear_object (&priv->factory);
}

static void
gtk_list_header_widget_dispose (GObject *object)
{
  GtkListHeaderWidget *this = GTK_LIST_HEADER_WIDGET (object);

  gtk_list_header_widget_clear_factory (this);

  G_OBJECT_CLASS (gtk_list_header_widget_parent_class)->dispose (object);
}

static void
gtk_list_header_widget_class_init (GtkListHeaderWidgetClass *klass)
{
  GtkListHeaderBaseClass *base_class = GTK_LIST_HEADER_BASE_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  base_class->update = gtk_list_header_widget_update;

  gobject_class->set_property = gtk_list_header_widget_set_property;
  gobject_class->dispose = gtk_list_header_widget_dispose;

  properties[PROP_FACTORY] =
    g_param_spec_object ("factory", NULL, NULL,
                         GTK_TYPE_LIST_ITEM_FACTORY,
                         G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);

  gtk_widget_class_set_css_name (widget_class, I_("header"));
  gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_ROW_HEADER);
  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);
}

static void
gtk_list_header_widget_init (GtkListHeaderWidget *this)
{
}

void
gtk_list_header_widget_set_factory (GtkListHeaderWidget *this,
                                    GtkListItemFactory  *factory)
{
  GtkListHeaderWidgetPrivate *priv = gtk_list_header_widget_get_instance_private (this);

  if (priv->factory == factory)
    return;

  gtk_list_header_widget_clear_factory (this);

  if (factory)
    {
      priv->factory = g_object_ref (factory);

      gtk_list_header_widget_setup_factory (this);
    }

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_FACTORY]);
}

GtkWidget *
gtk_list_header_widget_new (GtkListItemFactory *factory)
{
  return g_object_new (GTK_TYPE_LIST_HEADER_WIDGET,
                       "factory", factory,
                       NULL);
}

void
gtk_list_header_widget_set_child (GtkListHeaderWidget *this,
                                  GtkWidget           *child)
{
  GtkWidget *cur_child = gtk_widget_get_first_child (GTK_WIDGET (this));

  if (cur_child == child)
    return;

  g_clear_pointer (&cur_child, gtk_widget_unparent);

  if (child)
    gtk_widget_set_parent (child, GTK_WIDGET (this));
}

