/* GTK - The GIMP Toolkit
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * Authors: Cosimo Cecchi <cosimoc@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gtkiconhelperprivate.h"

#include <math.h>

#include "gtkcssenumvalueprivate.h"
#include "gtkcssnumbervalueprivate.h"
#include "gtkcssstyleprivate.h"
#include "gtkcsstransientnodeprivate.h"
#include "gtkiconthemeprivate.h"
#include "gtkrendericonprivate.h"
#include "gtkscalerprivate.h"
#include "gtksnapshot.h"
#include "gtkwidgetprivate.h"
#include "gdk/gdkprofilerprivate.h"
#include "gtksymbolicpaintable.h"

struct _GtkIconHelper
{
  GObject parent_instance;

  GtkImageDefinition *def;

  int pixel_size;

  guint use_fallback : 1;
  guint texture_is_symbolic : 1;

  GtkWidget *owner;
  GtkCssNode *node;
  GdkPaintable *paintable;
};

static GtkIconLookupFlags
get_icon_lookup_flags (GtkIconHelper *this,
                       GtkCssStyle   *style)
{
  GtkIconLookupFlags flags;
  GtkCssIconStyle icon_style;

  flags = 0;

  icon_style = _gtk_css_icon_style_value_get (style->icon->icon_style);

  switch (icon_style)
    {
    case GTK_CSS_ICON_STYLE_REGULAR:
      flags |= GTK_ICON_LOOKUP_FORCE_REGULAR;
      break;
    case GTK_CSS_ICON_STYLE_SYMBOLIC:
      flags |= GTK_ICON_LOOKUP_FORCE_SYMBOLIC;
      break;
    case GTK_CSS_ICON_STYLE_REQUESTED:
      break;
    default:
      g_assert_not_reached ();
      return 0;
    }

  return flags;
}

static GdkPaintable *
ensure_paintable_for_gicon (GtkIconHelper    *this,
                            GtkCssStyle      *style,
                            int               scale,
                            GtkTextDirection  dir,
                            gboolean          preload,
                            GIcon            *gicon,
                            gboolean         *symbolic)
{
  GtkIconTheme *icon_theme;
  int width, height;
  GtkIconPaintable *icon;
  GtkIconLookupFlags flags;

  icon_theme = gtk_icon_theme_get_for_display (gtk_widget_get_display (this->owner));
  flags = get_icon_lookup_flags (this, style);
  if (preload)
    flags |= GTK_ICON_LOOKUP_PRELOAD;

  width = height = gtk_icon_helper_get_size (this);

  icon = gtk_icon_theme_lookup_by_gicon (icon_theme,
                                         gicon,
                                         MIN (width, height),
                                         scale,
                                         dir,
                                         flags);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  *symbolic = gtk_icon_paintable_is_symbolic (icon);
G_GNUC_END_IGNORE_DEPRECATIONS

  return GDK_PAINTABLE (icon);
}

static GdkPaintable *
gtk_icon_helper_load_paintable (GtkIconHelper   *this,
                                gboolean         preload,
                                gboolean        *out_symbolic)
{
  GdkPaintable *paintable;
  GIcon *gicon;
  gboolean symbolic;

  switch (gtk_image_definition_get_storage_type (this->def))
    {
    case GTK_IMAGE_PAINTABLE:
      paintable = g_object_ref (gtk_image_definition_get_paintable (this->def));
      symbolic = GTK_IS_SYMBOLIC_PAINTABLE (paintable);
      break;

    case GTK_IMAGE_ICON_NAME:
      if (this->use_fallback)
        gicon = g_themed_icon_new_with_default_fallbacks (gtk_image_definition_get_icon_name (this->def));
      else
        gicon = g_themed_icon_new (gtk_image_definition_get_icon_name (this->def));
      paintable = ensure_paintable_for_gicon (this,
                                              gtk_css_node_get_style (this->node),
                                              gtk_widget_get_scale_factor (this->owner),
                                              gtk_widget_get_direction (this->owner),
                                              preload,
                                              gicon,
                                              &symbolic);
      g_object_unref (gicon);
      break;

    case GTK_IMAGE_GICON:
      paintable = ensure_paintable_for_gicon (this,
                                              gtk_css_node_get_style (this->node),
                                              gtk_widget_get_scale_factor (this->owner),
                                              gtk_widget_get_direction (this->owner),
                                              preload,
                                              gtk_image_definition_get_gicon (this->def),
                                              &symbolic);
      break;

    case GTK_IMAGE_EMPTY:
    default:
      paintable = NULL;
      symbolic = FALSE;
      break;
    }

  *out_symbolic = symbolic;

  return paintable;
}

static void
gtk_icon_helper_ensure_paintable (GtkIconHelper *this, gboolean preload)
{
  gboolean symbolic;

  if (this->paintable)
    return;

  this->paintable = gtk_icon_helper_load_paintable (this, preload, &symbolic);
  this->texture_is_symbolic = symbolic;
}

static void
gtk_icon_helper_paintable_snapshot (GdkPaintable *paintable,
                                    GdkSnapshot  *snapshot,
                                    double        width,
                                    double        height)
{
  GtkIconHelper *this = GTK_ICON_HELPER (paintable);
  GtkCssStyle *style;

  style = gtk_css_node_get_style (this->node);

  gtk_icon_helper_ensure_paintable (this, FALSE);
  if (this->paintable == NULL)
    return;

  switch (gtk_image_definition_get_storage_type (this->def))
    {
    case GTK_IMAGE_ICON_NAME:
    case GTK_IMAGE_GICON:
      {
        double x, y, w, h;

        /* Never scale up icons. */
        w = gdk_paintable_get_intrinsic_width (this->paintable);
        h = gdk_paintable_get_intrinsic_height (this->paintable);
        w = MIN (w, width);
        h = MIN (h, height);
        x = (width - w) / 2;
        y = (height - h) / 2;

        if (w == 0 || h == 0)
          return;

        if (x  != 0 || y != 0)
          {
            gtk_snapshot_save (snapshot);
            gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (x, y));
            gtk_css_style_snapshot_icon_paintable (style,
                                                   snapshot,
                                                   this->paintable,
                                                   w, h);
            gtk_snapshot_restore (snapshot);
          }
        else
          {
            gtk_css_style_snapshot_icon_paintable (style,
                                                   snapshot,
                                                   this->paintable,
                                                   w, h);

          }

      }
      break;

    case GTK_IMAGE_EMPTY:
      break;

    case GTK_IMAGE_PAINTABLE:
    default:
      {
        double image_ratio = (double) width / height;
        double ratio;
        double x, y, w, h;

        if (this->paintable == NULL)
          break;

        ratio = gdk_paintable_get_intrinsic_aspect_ratio (this->paintable);
        if (ratio == 0)
          {
            w = MIN (width, gtk_icon_helper_get_size (this));
            h = MIN (height, gtk_icon_helper_get_size (this));
          }
        else if (ratio > image_ratio)
          {
            w = MIN (width, gtk_icon_helper_get_size (this));
            h = width / ratio;
          }
        else
          {
            h = MIN (height, gtk_icon_helper_get_size (this));
            w = h * ratio;
          }

        x = floor (width - ceil (w)) / 2;
        y = floor (height - ceil (h)) / 2;

        if (x != 0 || y != 0)
          {
            gtk_snapshot_save (snapshot);
            gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (x, y));
            gtk_css_style_snapshot_icon_paintable (style,
                                                   snapshot,
                                                   this->paintable,
                                                   w, h);
            gtk_snapshot_restore (snapshot);
          }
        else
          {
            gtk_css_style_snapshot_icon_paintable (style,
                                                   snapshot,
                                                   this->paintable,
                                                   w, h);
          }
      }
      break;
    }
}

static GdkPaintable *
gtk_icon_helper_paintable_get_current_image (GdkPaintable *paintable)
{
  GtkIconHelper *this = GTK_ICON_HELPER (paintable);

  gtk_icon_helper_ensure_paintable (this, FALSE);
  if (this->paintable == NULL)
    return NULL;

  return gdk_paintable_get_current_image (this->paintable);
}

static int
gtk_icon_helper_paintable_get_intrinsic_width (GdkPaintable *paintable)
{
  GtkIconHelper *this = GTK_ICON_HELPER (paintable);

  return gtk_icon_helper_get_size (this);
}

static int
gtk_icon_helper_paintable_get_intrinsic_height (GdkPaintable *paintable)
{
  GtkIconHelper *this = GTK_ICON_HELPER (paintable);

  return gtk_icon_helper_get_size (this);
}

static double gtk_icon_helper_paintable_get_intrinsic_aspect_ratio (GdkPaintable *paintable)
{
  return 1.0;
};

static void
gtk_icon_helper_paintable_init (GdkPaintableInterface *iface)
{
  iface->snapshot = gtk_icon_helper_paintable_snapshot;
  iface->get_current_image = gtk_icon_helper_paintable_get_current_image;
  iface->get_intrinsic_width = gtk_icon_helper_paintable_get_intrinsic_width;
  iface->get_intrinsic_height = gtk_icon_helper_paintable_get_intrinsic_height;
  iface->get_intrinsic_aspect_ratio = gtk_icon_helper_paintable_get_intrinsic_aspect_ratio;
}

G_DEFINE_TYPE_WITH_CODE (GtkIconHelper, gtk_icon_helper, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GDK_TYPE_PAINTABLE,
						gtk_icon_helper_paintable_init))

void
gtk_icon_helper_invalidate (GtkIconHelper *this)
{
  g_clear_object (&this->paintable);
  this->texture_is_symbolic = FALSE;

  gtk_widget_queue_draw (this->owner);
}

void
gtk_icon_helper_invalidate_for_change (GtkIconHelper     *this,
                                       GtkCssStyleChange *change)
{
  if (change == NULL ||
      gtk_css_style_change_affects (change, GTK_CSS_AFFECTS_ICON_TEXTURE |
                                            GTK_CSS_AFFECTS_ICON_SIZE))
    {
      /* Avoid the queue_resize in gtk_icon_helper_invalidate */
      g_clear_object (&this->paintable);
      this->texture_is_symbolic = FALSE;
      gtk_widget_queue_draw (this->owner);
    }

  if (change == NULL ||
      gtk_css_style_change_affects (change, GTK_CSS_AFFECTS_ICON_SIZE))
    {
      gtk_widget_queue_resize (this->owner);
    }
  else if (gtk_css_style_change_affects (change, GTK_CSS_AFFECTS_ICON_REDRAW) ||
           (this->texture_is_symbolic &&
            gtk_css_style_change_affects (change, GTK_CSS_AFFECTS_ICON_REDRAW_SYMBOLIC)))
    {
      gtk_widget_queue_draw (this->owner);
    }

  /* The css size is valid now, preload */
  gtk_icon_helper_ensure_paintable (this, TRUE);
}

static void
gtk_icon_helper_take_definition (GtkIconHelper      *this,
                                 GtkImageDefinition *def)
{
  _gtk_icon_helper_clear (this);

  if (def == NULL)
    return;

  gtk_image_definition_unref (this->def);
  this->def = def;

  gtk_icon_helper_invalidate (this);
}

void
_gtk_icon_helper_clear (GtkIconHelper *this)
{
  g_clear_object (&this->paintable);
  this->texture_is_symbolic = FALSE;

  if (gtk_image_definition_get_storage_type (this->def) != GTK_IMAGE_EMPTY)
    {
      gtk_image_definition_unref (this->def);
      this->def = gtk_image_definition_new_empty ();
      gtk_icon_helper_invalidate (this);
    }
}

static void
gtk_icon_helper_finalize (GObject *object)
{
  GtkIconHelper *this = GTK_ICON_HELPER (object);

  _gtk_icon_helper_clear (this);
  g_signal_handlers_disconnect_by_func (this->owner, G_CALLBACK (gtk_icon_helper_invalidate), this);
  gtk_image_definition_unref (this->def);

  G_OBJECT_CLASS (gtk_icon_helper_parent_class)->finalize (object);
}

void
gtk_icon_helper_class_init (GtkIconHelperClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = gtk_icon_helper_finalize;
}

void
gtk_icon_helper_init (GtkIconHelper *this)
{
  this->def = gtk_image_definition_new_empty ();
}

GtkIconHelper *
gtk_icon_helper_new (GtkCssNode *css_node,
                     GtkWidget  *owner)
{
  GtkIconHelper *this;
  
  this = g_object_new (GTK_TYPE_ICON_HELPER, NULL);

  this->pixel_size = -1;
  this->texture_is_symbolic = FALSE;

  this->node = css_node;
  this->owner = owner;
  g_signal_connect_swapped (owner, "direction-changed", G_CALLBACK (gtk_icon_helper_invalidate), this);
  g_signal_connect_swapped (owner, "notify::scale-factor", G_CALLBACK (gtk_icon_helper_invalidate), this);

  return this;
}

int
gtk_icon_helper_get_size (GtkIconHelper *this)
{
  GtkCssStyle *style;

  if (this->pixel_size != -1)
    return this->pixel_size;

  style = gtk_css_node_get_style (this->node);
  return gtk_css_number_value_get (style->icon->icon_size, 100);
}

void
_gtk_icon_helper_set_definition (GtkIconHelper *this,
                                 GtkImageDefinition *def)
{
  if (def)
    gtk_icon_helper_take_definition (this, gtk_image_definition_ref (def));
  else
    _gtk_icon_helper_clear (this);
}

void
_gtk_icon_helper_set_gicon (GtkIconHelper *this,
                            GIcon         *gicon)
{
  gtk_icon_helper_take_definition (this, gtk_image_definition_new_gicon (gicon));
}

void
_gtk_icon_helper_set_icon_name (GtkIconHelper *this,
                                const char    *icon_name)
{
  gtk_icon_helper_take_definition (this, gtk_image_definition_new_icon_name (icon_name));
}

void
_gtk_icon_helper_set_paintable (GtkIconHelper *this,
			        GdkPaintable  *paintable)
{
  gtk_icon_helper_take_definition (this, gtk_image_definition_new_paintable (paintable));
}

gboolean
_gtk_icon_helper_set_pixel_size (GtkIconHelper *this,
                                 int            pixel_size)
{
  if (this->pixel_size != pixel_size)
    {
      this->pixel_size = pixel_size;
      gtk_icon_helper_invalidate (this);
      return TRUE;
    }
  return FALSE;
}

gboolean
_gtk_icon_helper_set_use_fallback (GtkIconHelper *this,
                                   gboolean       use_fallback)
{
  if (this->use_fallback != use_fallback)
    {
      this->use_fallback = use_fallback;
      gtk_icon_helper_invalidate (this);
      return TRUE;
    }
  return FALSE;
}

GtkImageType
_gtk_icon_helper_get_storage_type (GtkIconHelper *this)
{
  return gtk_image_definition_get_storage_type (this->def);
}

gboolean
_gtk_icon_helper_get_use_fallback (GtkIconHelper *this)
{
  return this->use_fallback;
}

int
_gtk_icon_helper_get_pixel_size (GtkIconHelper *this)
{
  return this->pixel_size;
}

GtkImageDefinition *
gtk_icon_helper_get_definition (GtkIconHelper *this)
{
  return this->def;
}

GIcon *
_gtk_icon_helper_peek_gicon (GtkIconHelper *this)
{
  return gtk_image_definition_get_gicon (this->def);
}

GdkPaintable *
_gtk_icon_helper_peek_paintable (GtkIconHelper *this)
{
  return gtk_image_definition_get_paintable (this->def);
}

const char *
_gtk_icon_helper_get_icon_name (GtkIconHelper *this)
{
  return gtk_image_definition_get_icon_name (this->def);
}

gboolean
_gtk_icon_helper_get_is_empty (GtkIconHelper *this)
{
  return gtk_image_definition_get_storage_type (this->def) == GTK_IMAGE_EMPTY;
}

void
gtk_icon_size_set_style_classes (GtkCssNode  *cssnode,
                                 GtkIconSize  icon_size)
{
  struct {
    GtkIconSize icon_size;
    const char *class_name;
  } class_names[] = {
    { GTK_ICON_SIZE_NORMAL, "normal-icons" },
    { GTK_ICON_SIZE_LARGE, "large-icons" }
  };
  guint i;

  for (i = 0; i < G_N_ELEMENTS (class_names); i++)
    {
      if (icon_size == class_names[i].icon_size)
        gtk_css_node_add_class (cssnode, g_quark_from_static_string (class_names[i].class_name));
      else
        gtk_css_node_remove_class (cssnode, g_quark_from_static_string (class_names[i].class_name));
    }
}
