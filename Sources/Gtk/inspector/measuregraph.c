/*
 * Copyright © 2021 Benjamin Otte
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

#include "measuregraph.h"

/* gdk_texture_new_for_surface() */
#include "gdk/gdktextureprivate.h"

#define MAX_SIZES 2048

typedef struct _Size Size;
struct _Size
{
  int min;
  int nat;
};

struct _GtkInspectorMeasureGraph
{
  GObject parent_instance;

  GdkPaintable *texture;
  Size width;
  Size height;
  Size width_for_height[MAX_SIZES];
  Size height_for_width[MAX_SIZES];
};

struct _GtkInspectorMeasureGraphClass
{
  GObjectClass parent_class;
};

static void
gtk_inspector_measure_graph_ensure_texture (GtkInspectorMeasureGraph *this)
{
  int i, width, height;
  cairo_surface_t *surface;
  cairo_t *cr;

  if (this->texture)
    return;

  if (this->width.nat == 0 || this->height.nat == 0)
    {
      this->texture = gdk_paintable_new_empty (0, 0);
      return;
    }

  width = this->width.nat;
  for (i = 0; i < MAX_SIZES; i++)
    width = MAX (width, this->width_for_height[i].nat);
  width = MIN (width, MAX_SIZES);
  height = this->height.nat;
  for (i = 0; i < MAX_SIZES; i++)
    height = MAX (height, this->height_for_width[i].nat);
  height = MIN (height, MAX_SIZES);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
  cr = cairo_create (surface);
  cairo_set_operator (cr, CAIRO_OPERATOR_ADD);
  
  cairo_set_source_rgba (cr, 0.5, 0, 0, 1);
  cairo_rectangle (cr, 0, 0, this->width.min, height);
  cairo_fill (cr);
  cairo_set_source_rgba (cr, 1, 0, 0, 1);
  for (i = this->width.min; i < width; i++)
    cairo_rectangle (cr, i, 0, 1, this->height_for_width[i].min);
  cairo_fill (cr);
  cairo_set_source_rgba (cr, 1, 0, 0, 0.3);
  for (i = this->width.min; i < width; i++)
    cairo_rectangle (cr, i, this->height_for_width[i].min, 1, this->height_for_width[i].nat - this->height_for_width[i].min);
  cairo_fill (cr);

  cairo_set_source_rgba (cr, 0, 0, 0.5, 1);
  cairo_rectangle (cr, 0, 0, width, this->height.min);
  cairo_fill (cr);
  cairo_set_source_rgba (cr, 0, 0, 1, 1);
  for (i = this->height.min; i < height; i++)
    cairo_rectangle (cr, 0, i, this->width_for_height[i].min, 1);
  cairo_fill (cr);
  cairo_set_source_rgba (cr, 0, 0, 1, 0.3);
  for (i = this->height.min; i < height; i++)
    cairo_rectangle (cr, this->width_for_height[i].min, i, this->width_for_height[i].nat - this->width_for_height[i].min, 1);
  cairo_fill (cr);

  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  cairo_set_source_rgba (cr, 0, 0, 0, 1);
  cairo_rectangle (cr, this->width.nat, 0, 1, height);
  cairo_rectangle (cr, 0, this->height.nat, width, 1);
  cairo_fill (cr);

  cairo_destroy (cr);
  this->texture = GDK_PAINTABLE (gdk_texture_new_for_surface (surface));
  cairo_surface_destroy (surface);
}

static void
gtk_inspector_measure_graph_paintable_snapshot (GdkPaintable *paintable,
                                                GdkSnapshot  *snapshot,
                                                double        width,
                                                double        height)
{
  GtkInspectorMeasureGraph *this = GTK_INSPECTOR_MEASURE_GRAPH (paintable);

  gtk_inspector_measure_graph_ensure_texture (this);

  if (this->texture == NULL)
    return;

  gdk_paintable_snapshot (this->texture, snapshot, width, height);
}

static int
gtk_inspector_measure_graph_paintable_get_intrinsic_width (GdkPaintable *paintable)
{
  GtkInspectorMeasureGraph *this = GTK_INSPECTOR_MEASURE_GRAPH (paintable);

  gtk_inspector_measure_graph_ensure_texture (this);

  return gdk_paintable_get_intrinsic_width (this->texture);
}

static int
gtk_inspector_measure_graph_paintable_get_intrinsic_height (GdkPaintable *paintable)
{
  GtkInspectorMeasureGraph *this = GTK_INSPECTOR_MEASURE_GRAPH (paintable);

  gtk_inspector_measure_graph_ensure_texture (this);

  return gdk_paintable_get_intrinsic_height (this->texture);
}

static double
gtk_inspector_measure_graph_paintable_get_intrinsic_aspect_ratio (GdkPaintable *paintable)
{
  GtkInspectorMeasureGraph *this = GTK_INSPECTOR_MEASURE_GRAPH (paintable);

  gtk_inspector_measure_graph_ensure_texture (this);

  return gdk_paintable_get_intrinsic_aspect_ratio (this->texture);
}

static void
gtk_inspector_measure_graph_paintable_init (GdkPaintableInterface *iface)
{
  iface->snapshot = gtk_inspector_measure_graph_paintable_snapshot;
  iface->get_intrinsic_width = gtk_inspector_measure_graph_paintable_get_intrinsic_width;
  iface->get_intrinsic_height = gtk_inspector_measure_graph_paintable_get_intrinsic_height;
  iface->get_intrinsic_aspect_ratio = gtk_inspector_measure_graph_paintable_get_intrinsic_aspect_ratio;
}

G_DEFINE_TYPE_EXTENDED (GtkInspectorMeasureGraph, gtk_inspector_measure_graph, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (GDK_TYPE_PAINTABLE,
                                               gtk_inspector_measure_graph_paintable_init))

static void
gtk_inspector_measure_graph_dispose (GObject *object)
{
  GtkInspectorMeasureGraph *this = GTK_INSPECTOR_MEASURE_GRAPH (object);

  g_clear_object (&this->texture);

  G_OBJECT_CLASS (gtk_inspector_measure_graph_parent_class)->dispose (object);
}

static void
gtk_inspector_measure_graph_class_init (GtkInspectorMeasureGraphClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = gtk_inspector_measure_graph_dispose;
}

static void
gtk_inspector_measure_graph_init (GtkInspectorMeasureGraph *this)
{
}

GtkInspectorMeasureGraph *
gtk_inspector_measure_graph_new (void)
{
  return g_object_new (GTK_TYPE_INSPECTOR_MEASURE_GRAPH, NULL);
}

void
gtk_inspector_measure_graph_clear (GtkInspectorMeasureGraph *this)
{
  g_clear_object (&this->texture);

  memset (&this->width, 0, sizeof (this->width));
  memset (&this->height, 0, sizeof (this->height));
  memset (&this->width_for_height, 0, sizeof (this->width_for_height));
  memset (&this->height_for_width, 0, sizeof (this->height_for_width));

  gdk_paintable_invalidate_size (GDK_PAINTABLE (this));
  gdk_paintable_invalidate_contents (GDK_PAINTABLE (this));
}

void
gtk_inspector_measure_graph_measure (GtkInspectorMeasureGraph *this,
                                     GtkWidget                *widget)
{
  int i;

  g_clear_object (&this->texture);

  gtk_widget_measure (widget, GTK_ORIENTATION_HORIZONTAL, -1, &this->width.min, &this->width.nat, NULL, NULL);
  gtk_widget_measure (widget, GTK_ORIENTATION_VERTICAL, -1 ,&this->height.min, &this->height.nat, NULL, NULL);

  memset (&this->width_for_height, 0, sizeof (Size) * MIN (this->height.min, MAX_SIZES));
  for (i = this->height.min; i < MAX_SIZES; i++)
    gtk_widget_measure (widget, GTK_ORIENTATION_HORIZONTAL, i, &this->width_for_height[i].min, &this->width_for_height[i].nat, NULL, NULL);
  memset (&this->height_for_width, 0, sizeof (Size) * MIN (this->width.min, MAX_SIZES));
  for (i = this->width.min; i < MAX_SIZES; i++)
    gtk_widget_measure (widget, GTK_ORIENTATION_VERTICAL, i, &this->height_for_width[i].min, &this->height_for_width[i].nat, NULL, NULL);

  gdk_paintable_invalidate_size (GDK_PAINTABLE (this));
  gdk_paintable_invalidate_contents (GDK_PAINTABLE (this));
}

GdkTexture *
gtk_inspector_measure_graph_get_texture (GtkInspectorMeasureGraph *this)
{
  gtk_inspector_measure_graph_ensure_texture (this);

  if (!GDK_IS_TEXTURE (this->texture))
    return NULL;

  return GDK_TEXTURE (this->texture);
}

