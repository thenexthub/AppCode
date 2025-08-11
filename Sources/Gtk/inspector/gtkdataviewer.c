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

#include "gtkdataviewer.h"

#include "gtkbinlayout.h"
#include "gtklabel.h"
#include "gtkpicture.h"
#include "gtkcolorswatchprivate.h"
#include "gtkbox.h"


struct _GtkDataViewer
{
  GtkWidget parent_instance;

  GtkWidget *contents;
  GCancellable *cancellable;
  GError *error;

  enum {
    NOT_LOADED = 0,
    LOADING_DONE,
    LOADING_EXTERNALLY,
    LOADING_INTERNALLY,
    LOADING_FAILED
  } loading;
};

enum
{
  PROP_0,
  PROP_LOADING,

  N_PROPS
};

enum {
  LOAD,
  LAST_SIGNAL
};

G_DEFINE_TYPE (GtkDataViewer, gtk_data_viewer, GTK_TYPE_WIDGET)

static GParamSpec *properties[N_PROPS] = { NULL, };
static guint signals[LAST_SIGNAL];

static void
gtk_data_viewer_ensure_loaded (GtkDataViewer *this)
{
  gboolean started_loading;

  if (this->loading != NOT_LOADED)
    return;

  this->loading = LOADING_EXTERNALLY;
  this->cancellable = g_cancellable_new ();
  g_signal_emit (this, signals[LOAD], 0, this->cancellable, &started_loading);

  if (!started_loading)
    {
      this->loading = LOADING_FAILED; /* avoid notify::is_loading */
      gtk_data_viewer_load_error (this, g_error_new (G_IO_ERROR, G_IO_ERROR_FAILED, "Nothing to load"));
    }

  g_assert (this->loading != NOT_LOADED);

  if (gtk_data_viewer_is_loading (this))
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
}

static void
gtk_data_viewer_realize (GtkWidget *widget)
{
  GtkDataViewer *this = GTK_DATA_VIEWER (widget);

  GTK_WIDGET_CLASS (gtk_data_viewer_parent_class)->realize (widget);

  gtk_data_viewer_ensure_loaded (this);
}

static void
gtk_data_viewer_unrealize (GtkWidget *widget)
{
  GtkDataViewer *this = GTK_DATA_VIEWER (widget);

  GTK_WIDGET_CLASS (gtk_data_viewer_parent_class)->unrealize (widget);

  gtk_data_viewer_reset (this);
}

static void
gtk_data_viewer_dispose (GObject *object)
{
  //GtkDataViewer *this = GTK_DATA_VIEWER (object);

  G_OBJECT_CLASS (gtk_data_viewer_parent_class)->dispose (object);
}

static void
gtk_data_viewer_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GtkDataViewer *this = GTK_DATA_VIEWER (object);

  switch (property_id)
    {
    case PROP_LOADING:
      g_value_set_boolean (value, gtk_data_viewer_is_loading (this));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_data_viewer_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  //GtkDataViewer *this = GTK_DATA_VIEWER (object);

  switch (property_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_data_viewer_class_init (GtkDataViewerClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  widget_class->realize = gtk_data_viewer_realize;
  widget_class->unrealize = gtk_data_viewer_unrealize;

  gobject_class->dispose = gtk_data_viewer_dispose;
  gobject_class->get_property = gtk_data_viewer_get_property;
  gobject_class->set_property = gtk_data_viewer_set_property;

  properties[PROP_LOADING] =
    g_param_spec_boolean ("loading", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);

  signals[LOAD] =
      g_signal_new ("load",
                    G_TYPE_FROM_CLASS (klass),
                    G_SIGNAL_RUN_LAST,
                    0,
                    g_signal_accumulator_first_wins, NULL,
                    NULL,
                    G_TYPE_BOOLEAN, 1,
                    G_TYPE_CANCELLABLE);

  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_css_name (widget_class, "frame");
}

static void
gtk_data_viewer_init (GtkDataViewer *this)
{
}

GtkWidget *
gtk_data_viewer_new (void)
{
  return g_object_new (GTK_TYPE_DATA_VIEWER, NULL);
}

gboolean
gtk_data_viewer_is_loading (GtkDataViewer *this)
{
  g_return_val_if_fail (GTK_IS_DATA_VIEWER (this), FALSE);

  return this->loading == LOADING_EXTERNALLY ||
         this->loading == LOADING_INTERNALLY;
}

void
gtk_data_viewer_reset (GtkDataViewer *this)
{
  gboolean was_loading;

  g_return_if_fail (GTK_IS_DATA_VIEWER (this));

  g_object_freeze_notify (G_OBJECT (this));

  was_loading = gtk_data_viewer_is_loading (this);

  g_clear_pointer (&this->contents, gtk_widget_unparent);
  g_clear_error (&this->error);
  g_cancellable_cancel (this->cancellable);
  g_clear_object (&this->cancellable);

  this->loading = NOT_LOADED;

  if (gtk_widget_get_realized (GTK_WIDGET (this)))
    gtk_data_viewer_ensure_loaded (this);

  if (was_loading != gtk_data_viewer_is_loading (this))
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);

  g_object_thaw_notify (G_OBJECT (this));
}

void
gtk_data_viewer_load_value (GtkDataViewer *this,
                            const GValue  *value)
{
  gboolean was_loading;

  g_return_if_fail (GTK_IS_DATA_VIEWER (this));

  was_loading = gtk_data_viewer_is_loading (this);
  this->loading = LOADING_DONE;

  g_clear_pointer (&this->contents, gtk_widget_unparent);
  g_cancellable_cancel (this->cancellable);
  g_clear_object (&this->cancellable);

  if (g_type_is_a (G_VALUE_TYPE (value), G_TYPE_STRING))
    {
      this->contents = gtk_label_new (g_value_get_string (value));
      gtk_label_set_wrap (GTK_LABEL (this->contents), TRUE);
      gtk_widget_set_parent (this->contents, GTK_WIDGET (this));
    }
  else if (g_type_is_a (G_VALUE_TYPE (value), GDK_TYPE_PAINTABLE))
    {
      this->contents = gtk_picture_new_for_paintable (g_value_get_object (value));
      gtk_widget_set_size_request (this->contents, 256, 256);
      gtk_widget_set_parent (this->contents, GTK_WIDGET (this));
    }
  else if (g_type_is_a (G_VALUE_TYPE (value), GDK_TYPE_PIXBUF))
    {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
      this->contents = gtk_picture_new_for_pixbuf (g_value_get_object (value));
G_GNUC_END_IGNORE_DEPRECATIONS
      gtk_widget_set_size_request (this->contents, 256, 256);
      gtk_widget_set_parent (this->contents, GTK_WIDGET (this));
    }
  else if (g_type_is_a (G_VALUE_TYPE (value), GDK_TYPE_RGBA))
    {
      const GdkRGBA *color = g_value_get_boxed (value);

      this->contents = gtk_color_swatch_new ();
      gtk_color_swatch_set_rgba (GTK_COLOR_SWATCH (this->contents), color);
      gtk_widget_set_size_request (this->contents, 48, 32);
      gtk_widget_set_halign (this->contents, GTK_ALIGN_CENTER);
      gtk_widget_set_parent (this->contents, GTK_WIDGET (this));
    }
  else if (g_type_is_a (G_VALUE_TYPE (value), G_TYPE_FILE))
    {
      GFile *file = g_value_get_object (value);

      this->contents = gtk_label_new (g_file_peek_path (file));
      gtk_label_set_ellipsize (GTK_LABEL (this->contents), PANGO_ELLIPSIZE_START);
      gtk_widget_set_halign (this->contents, GTK_ALIGN_CENTER);
      gtk_widget_set_parent (this->contents, GTK_WIDGET (this));
    }
  else if (g_type_is_a (G_VALUE_TYPE (value), GDK_TYPE_FILE_LIST))
    {
      GList *l;

      this->contents = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
      gtk_widget_set_parent (this->contents, GTK_WIDGET (this));

      for (l = g_value_get_boxed (value); l; l = l->next)
        {
          GFile *file = l->data;
          GtkWidget *label = gtk_label_new (g_file_peek_path (file));
          gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_START);
          gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
          gtk_box_append (GTK_BOX (this->contents), label);
        }
    }
  else
    {
      gtk_data_viewer_load_error (this,
                                  g_error_new (G_IO_ERROR,
                                               G_IO_ERROR_FAILED,
                                               "Cannot display objects of type \"%s\"", G_VALUE_TYPE_NAME (value)));
    }

  if (was_loading)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
}

static void
gtk_data_viewer_load_stream_done (GObject      *source,
                                  GAsyncResult *res,
                                  gpointer      data)
{
  GtkDataViewer *this = data;
  GError *error = NULL;
  GValue value = G_VALUE_INIT;

  if (!gdk_content_deserialize_finish (res, &value, &error))
    {
      if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        gtk_data_viewer_load_error (this, error);
      else
        g_clear_error (&error);

      g_object_unref (this);
      return;
    }

  gtk_data_viewer_load_value (this, &value);
  g_object_unref (this);
  g_value_unset (&value);
}

void
gtk_data_viewer_load_stream (GtkDataViewer *this,
                             GInputStream  *stream,
                             const char    *mime_type)
{
  GdkContentFormats *formats;
  const GType *gtypes;
  gboolean was_loading;

  g_return_if_fail (GTK_IS_DATA_VIEWER (this));
  g_return_if_fail (G_IS_INPUT_STREAM (stream));
  g_return_if_fail (mime_type != NULL);

  was_loading = gtk_data_viewer_is_loading (this);
  this->loading = LOADING_INTERNALLY;
  if (this->cancellable == NULL)
    this->cancellable = g_cancellable_new ();

  formats = gdk_content_formats_new (&mime_type, 1);
  formats = gdk_content_formats_union_deserialize_gtypes (formats);
  gtypes = gdk_content_formats_get_gtypes (formats, NULL);
  if (gtypes)
    {
      gdk_content_deserialize_async (stream,
                                     mime_type,
                                     gtypes[0],
                                     G_PRIORITY_DEFAULT,
                                     this->cancellable,
                                     gtk_data_viewer_load_stream_done,
                                     g_object_ref (this));

      if (!was_loading)
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
    }
  else
    {
      gtk_data_viewer_load_error (this,
                                  g_error_new (G_IO_ERROR, G_IO_ERROR_FAILED,
                                               "Cannot display data of type \"%s\"", mime_type));
    }

  gdk_content_formats_unref (formats);
}

void
gtk_data_viewer_load_error (GtkDataViewer *this,
                            GError        *error)
{
  gboolean was_loading;

  g_return_if_fail (GTK_IS_DATA_VIEWER (this));

  was_loading = gtk_data_viewer_is_loading (this);
  this->loading = LOADING_FAILED;

  g_clear_pointer (&this->contents, gtk_widget_unparent);
  g_clear_error (&this->error);
  g_cancellable_cancel (this->cancellable);
  g_clear_object (&this->cancellable);

  this->error = error;
  this->contents = gtk_label_new (error->message);
  gtk_widget_add_css_class (this->contents, "error");
  gtk_widget_set_halign (this->contents, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (this->contents, GTK_ALIGN_CENTER);
  gtk_widget_set_parent (this->contents, GTK_WIDGET (this));

  if (was_loading)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
}

