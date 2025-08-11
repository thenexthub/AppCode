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

#include "gtkrendernodepaintableprivate.h"

#include "gtksnapshot.h"

struct _GtkRenderNodePaintable
{
  GObject parent_instance;

  GskRenderNode *node;
  graphene_rect_t bounds;
};

struct _GtkRenderNodePaintableClass
{
  GObjectClass parent_class;
};

static void
gtk_render_node_paintable_paintable_snapshot (GdkPaintable *paintable,
                                              GdkSnapshot  *snapshot,
                                              double        width,
                                              double        height)
{
  GtkRenderNodePaintable *this = GTK_RENDER_NODE_PAINTABLE (paintable);

  if (this->bounds.size.width <= 0 ||
      this->bounds.size.height <= 0 ||
      this->node == NULL)
    return;

  gtk_snapshot_save (snapshot);

  gtk_snapshot_scale (snapshot,
                      width / ceilf (this->bounds.size.width),
                      height / ceilf (this->bounds.size.height));
  gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (-this->bounds.origin.x, -this->bounds.origin.y));

  gtk_snapshot_push_clip (snapshot, &this->bounds);

  gtk_snapshot_append_node (snapshot, this->node);
  //gtk_snapshot_append_color (snapshot, &(GdkRGBA) { 1, 0, 0, 1 }, &this->bounds);

  gtk_snapshot_pop (snapshot);

  gtk_snapshot_restore (snapshot);
}

static GdkPaintableFlags
gtk_render_node_paintable_paintable_get_flags (GdkPaintable *paintable)
{
  return GDK_PAINTABLE_STATIC_CONTENTS | GDK_PAINTABLE_STATIC_SIZE;
}

static int
gtk_render_node_paintable_paintable_get_intrinsic_width (GdkPaintable *paintable)
{
  GtkRenderNodePaintable *this = GTK_RENDER_NODE_PAINTABLE (paintable);

  return ceilf (this->bounds.size.width);
}

static int
gtk_render_node_paintable_paintable_get_intrinsic_height (GdkPaintable *paintable)
{
  GtkRenderNodePaintable *this = GTK_RENDER_NODE_PAINTABLE (paintable);

  return ceilf (this->bounds.size.height);
}

static void
gtk_render_node_paintable_paintable_init (GdkPaintableInterface *iface)
{
  iface->snapshot = gtk_render_node_paintable_paintable_snapshot;
  iface->get_flags = gtk_render_node_paintable_paintable_get_flags;
  iface->get_intrinsic_width = gtk_render_node_paintable_paintable_get_intrinsic_width;
  iface->get_intrinsic_height = gtk_render_node_paintable_paintable_get_intrinsic_height;
}

G_DEFINE_TYPE_EXTENDED (GtkRenderNodePaintable, gtk_render_node_paintable, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (GDK_TYPE_PAINTABLE,
                                               gtk_render_node_paintable_paintable_init))

static void
gtk_render_node_paintable_dispose (GObject *object)
{
  GtkRenderNodePaintable *this = GTK_RENDER_NODE_PAINTABLE (object);

  g_clear_pointer (&this->node, gsk_render_node_unref);

  G_OBJECT_CLASS (gtk_render_node_paintable_parent_class)->dispose (object);
}

static void
gtk_render_node_paintable_class_init (GtkRenderNodePaintableClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = gtk_render_node_paintable_dispose;
}

static void
gtk_render_node_paintable_init (GtkRenderNodePaintable *this)
{
}

GdkPaintable *
gtk_render_node_paintable_new (GskRenderNode         *node,
                               const graphene_rect_t *bounds)
{
  GtkRenderNodePaintable *this;

  g_return_val_if_fail (node == NULL || GSK_IS_RENDER_NODE (node), NULL);
  g_return_val_if_fail (bounds != NULL, NULL);

  this = g_object_new (GTK_TYPE_RENDER_NODE_PAINTABLE, NULL);

  this->node = node ? gsk_render_node_ref (node) : NULL;
  this->bounds = *bounds;

  return GDK_PAINTABLE (this);
}

GskRenderNode *
gtk_render_node_paintable_get_render_node (GtkRenderNodePaintable *this)
{
  g_return_val_if_fail (GTK_IS_RENDER_NODE_PAINTABLE (this), NULL);

  return this->node;
}
