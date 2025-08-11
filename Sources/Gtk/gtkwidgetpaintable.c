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

#include "gtkwidgetpaintableprivate.h"

#include "gtksnapshot.h"
#include "gtkrendernodepaintableprivate.h"
#include "gtkwidgetprivate.h"
#include "gtkprivate.h"

/**
 * GtkWidgetPaintable:
 *
 * A `GdkPaintable` that displays the contents of a widget.
 *
 * `GtkWidgetPaintable` will also take care of the widget not being in a
 * state where it can be drawn (like when it isn't shown) and just draw
 * nothing or where it does not have a size (like when it is hidden) and
 * report no size in that case.
 *
 * Of course, `GtkWidgetPaintable` allows you to monitor widgets for size
 * changes by emitting the [signal@Gdk.Paintable::invalidate-size] signal
 * whenever the size of the widget changes as well as for visual changes by
 * emitting the [signal@Gdk.Paintable::invalidate-contents] signal whenever
 * the widget changes.
 *
 * You can use a `GtkWidgetPaintable` everywhere a `GdkPaintable` is allowed,
 * including using it on a `GtkPicture` (or one of its parents) that it was
 * set on itself via gtk_picture_set_paintable(). The paintable will take care
 * of recursion when this happens. If you do this however, ensure that the
 * [property@Gtk.Picture:can-shrink] property is set to %TRUE or you might
 * end up with an infinitely growing widget.
 */
struct _GtkWidgetPaintable
{
  GObject parent_instance;

  GtkWidget *widget;
  guint snapshot_count;

  guint         pending_update_cb;      /* the idle source that updates the valid image to be the new current image */

  GdkPaintable *current_image;          /* the image that we are presenting */
  GdkPaintable *pending_image;          /* the image that we should be presenting */
};

struct _GtkWidgetPaintableClass
{
  GObjectClass parent_class;
};

enum {
  PROP_0,
  PROP_WIDGET,

  N_PROPS,
};

static GParamSpec *properties[N_PROPS] = { NULL, };

static void
gtk_widget_paintable_paintable_snapshot (GdkPaintable *paintable,
                                         GdkSnapshot  *snapshot,
                                         double        width,
                                         double        height)
{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (paintable);

  if (this->snapshot_count > 4)
    return;
  else if (this->snapshot_count > 0)
    {
      graphene_rect_t bounds;

      gtk_snapshot_push_clip (snapshot,
                              &GRAPHENE_RECT_INIT(0, 0, width, height));

      if (gtk_widget_compute_bounds (this->widget, this->widget, &bounds))
        {
          gtk_snapshot_scale (snapshot, width / bounds.size.width, height / bounds.size.height);
          gtk_snapshot_translate (snapshot, &bounds.origin);
        }

      gtk_widget_snapshot (this->widget, snapshot);

      gtk_snapshot_pop (snapshot);
    }
  else
    {
      gdk_paintable_snapshot (this->current_image, snapshot, width, height);
    }
}

static GdkPaintable *
gtk_widget_paintable_paintable_get_current_image (GdkPaintable *paintable)
{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (paintable);

  return g_object_ref (this->current_image);
}

static int
gtk_widget_paintable_paintable_get_intrinsic_width (GdkPaintable *paintable)
{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (paintable);

  return gdk_paintable_get_intrinsic_width (this->current_image);
}

static int
gtk_widget_paintable_paintable_get_intrinsic_height (GdkPaintable *paintable)
{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (paintable);

  return gdk_paintable_get_intrinsic_height (this->current_image);
}

static void
gtk_widget_paintable_paintable_init (GdkPaintableInterface *iface)
{
  iface->snapshot = gtk_widget_paintable_paintable_snapshot;
  iface->get_current_image = gtk_widget_paintable_paintable_get_current_image;
  iface->get_intrinsic_width = gtk_widget_paintable_paintable_get_intrinsic_width;
  iface->get_intrinsic_height = gtk_widget_paintable_paintable_get_intrinsic_height;
}

G_DEFINE_TYPE_EXTENDED (GtkWidgetPaintable, gtk_widget_paintable, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (GDK_TYPE_PAINTABLE,
                                               gtk_widget_paintable_paintable_init))

static void
gtk_widget_paintable_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)

{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (object);

  switch (prop_id)
    {
    case PROP_WIDGET:
      gtk_widget_paintable_set_widget (this, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_widget_paintable_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (object);

  switch (prop_id)
    {
    case PROP_WIDGET:
      g_value_set_object (value, this->widget);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_widget_paintable_unset_widget (GtkWidgetPaintable *this)
{
  if (this->widget == NULL)
    return;

  this->widget->priv->paintables = g_slist_remove (this->widget->priv->paintables,
                                                   this);

  this->widget = NULL;

  g_clear_object (&this->pending_image);
  if (this->pending_update_cb)
    {
      g_source_remove (this->pending_update_cb);
      this->pending_update_cb = 0;
    }
}

static void
gtk_widget_paintable_dispose (GObject *object)
{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (object);

  gtk_widget_paintable_unset_widget (this);

  G_OBJECT_CLASS (gtk_widget_paintable_parent_class)->dispose (object);
}

static void
gtk_widget_paintable_finalize (GObject *object)
{
  GtkWidgetPaintable *this = GTK_WIDGET_PAINTABLE (object);

  g_object_unref (this->current_image);

  G_OBJECT_CLASS (gtk_widget_paintable_parent_class)->finalize (object);
}

static void
gtk_widget_paintable_class_init (GtkWidgetPaintableClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = gtk_widget_paintable_get_property;
  gobject_class->set_property = gtk_widget_paintable_set_property;
  gobject_class->dispose = gtk_widget_paintable_dispose;
  gobject_class->finalize = gtk_widget_paintable_finalize;

  /**
   * GtkWidgetPaintable:widget:
   *
   * The observed widget or %NULL if none.
   */
  properties[PROP_WIDGET] =
    g_param_spec_object ("widget", NULL, NULL,
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);
}

static void
gtk_widget_paintable_init (GtkWidgetPaintable *this)
{
  this->current_image = gdk_paintable_new_empty (0, 0);
}

/**
 * gtk_widget_paintable_new:
 * @widget: (nullable) (transfer none): a `GtkWidget`
 *
 * Creates a new widget paintable observing the given widget.
 *
 * Returns: (transfer full) (type GtkWidgetPaintable): a new `GtkWidgetPaintable`
 */
GdkPaintable *
gtk_widget_paintable_new (GtkWidget *widget)
{
  g_return_val_if_fail (widget == NULL || GTK_IS_WIDGET (widget), NULL);

  return g_object_new (GTK_TYPE_WIDGET_PAINTABLE,
                       "widget", widget,
                       NULL);
}

static GdkPaintable *
gtk_widget_paintable_snapshot_widget (GtkWidgetPaintable *this)
{
  graphene_rect_t bounds;

  if (this->widget == NULL)
    return gdk_paintable_new_empty (0, 0);

  if (!gtk_widget_compute_bounds (this->widget, this->widget, &bounds))
    return gdk_paintable_new_empty (0, 0);

  if (this->widget->priv->render_node == NULL)
    return gdk_paintable_new_empty (bounds.size.width, bounds.size.height);
  
  return gtk_render_node_paintable_new (this->widget->priv->render_node, &bounds);
}

/**
 * gtk_widget_paintable_get_widget:
 * @this: a `GtkWidgetPaintable`
 *
 * Returns the widget that is observed or %NULL if none.
 *
 * Returns: (transfer none) (nullable): the observed widget.
 */
GtkWidget *
gtk_widget_paintable_get_widget (GtkWidgetPaintable *this)
{
  g_return_val_if_fail (GTK_IS_WIDGET_PAINTABLE (this), NULL);

  return this->widget;
}

/**
 * gtk_widget_paintable_set_widget:
 * @this: a `GtkWidgetPaintable`
 * @widget: (nullable): the widget to observe
 *
 * Sets the widget that should be observed.
 */
void
gtk_widget_paintable_set_widget (GtkWidgetPaintable *this,
                                 GtkWidget          *widget)
{

  g_return_if_fail (GTK_IS_WIDGET_PAINTABLE (this));
  g_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));

  if (this->widget == widget)
    return;

  gtk_widget_paintable_unset_widget (this);

  /* We do not ref the widget to not cause ref cycles when a widget
   * is told to observe itself or one of its parent.
   */
  this->widget = widget;

  if (widget)
    widget->priv->paintables = g_slist_prepend (widget->priv->paintables, this);

  g_object_unref (this->current_image);
  this->current_image = gtk_widget_paintable_snapshot_widget (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_WIDGET]);
  gdk_paintable_invalidate_size (GDK_PAINTABLE (this));
  gdk_paintable_invalidate_contents (GDK_PAINTABLE (this));
}

static gboolean
gtk_widget_paintable_update_func (gpointer data)
{
  GtkWidgetPaintable *this = data;
  GdkPaintable *old_image;

  if (this->current_image != this->pending_image)
    {
      old_image = this->current_image;
      this->current_image = this->pending_image;
      this->pending_image = NULL;
      this->pending_update_cb = 0;

      if (gdk_paintable_get_intrinsic_width (this->current_image) != gdk_paintable_get_intrinsic_width (old_image) ||
          gdk_paintable_get_intrinsic_height (this->current_image) != gdk_paintable_get_intrinsic_height (old_image))
        gdk_paintable_invalidate_size (GDK_PAINTABLE (this));

      g_object_unref (old_image);

      gdk_paintable_invalidate_contents (GDK_PAINTABLE (this));
    }
  else
    {
      g_clear_object (&this->pending_image);
      this->pending_update_cb = 0;
    }

  return G_SOURCE_REMOVE;
}

void
gtk_widget_paintable_update_image (GtkWidgetPaintable *this)
{
  GdkPaintable *pending_image;

  if (this->pending_update_cb == 0)
    {
      this->pending_update_cb = g_idle_add_full (G_PRIORITY_HIGH,
                                                 gtk_widget_paintable_update_func,
                                                 this,
                                                 NULL);
      gdk_source_set_static_name_by_id (this->pending_update_cb, "[gtk] gtk_widget_paintable_update_func");
    }

  pending_image = gtk_widget_paintable_snapshot_widget (this);
  g_set_object (&this->pending_image, pending_image);
  g_object_unref (pending_image);
}

void
gtk_widget_paintable_push_snapshot_count (GtkWidgetPaintable *this)
{
  this->snapshot_count++;
}

void
gtk_widget_paintable_pop_snapshot_count (GtkWidgetPaintable *this)
{
  this->snapshot_count--;
}
