/*
 * GTK - The GIMP Toolkit
 * Copyright (C) 2022 Red Hat, Inc.
 * All rights reserved.
 *
 * This Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gtkcolordialogbutton.h"

#include "gtkbinlayout.h"
#include "gtkbutton.h"
#include "gtkcolorswatchprivate.h"
#include "gtkdragsource.h"
#include "gtkdroptarget.h"
#include <glib/gi18n-lib.h>
#include "gtkmain.h"
#include "gtkmarshalers.h"
#include "gtkprivate.h"
#include "gtksnapshot.h"
#include "gtkwidgetprivate.h"
#include "gdk/gdkrgbaprivate.h"


static gboolean drop           (GtkDropTarget        *dest,
                                const GValue         *value,
                                double                x,
                                double                y,
                                GtkColorDialogButton *this);
static GdkContentProvider *
                  drag_prepare (GtkDragSource        *source,
                                double                x,
                                double                y,
                                GtkColorDialogButton *this);
static void     activated      (GtkColorDialogButton *this);
static void     button_clicked (GtkColorDialogButton *this);
static void     update_button_sensitivity
                               (GtkColorDialogButton *this);

/**
 * GtkColorDialogButton:
 *
 * Opens a color chooser dialog to select a color.
 *
 * <picture>
 *   <source srcset="color-button-dark.png" media="(prefers-color-scheme: dark)">
 *   <img alt="An example GtkColorDialogButton" src="color-button.png">
 * </picture>
 *
 * It is suitable widget for selecting a color in a preference dialog.
 *
 * # CSS nodes
 *
 * ```
 * colorbutton
 * ╰── button.color
 *     ╰── [content]
 * ```
 *
 * `GtkColorDialogButton` has a single CSS node with name colorbutton which
 * contains a button node. To differentiate it from a plain `GtkButton`,
 * it gets the .color style class.
 *
 * Since: 4.10
 */

/* {{{ GObject implementation */

struct _GtkColorDialogButton
{
  GtkWidget parent_instance;

  GtkWidget *button;
  GtkWidget *swatch;

  GtkColorDialog *dialog;
  GCancellable *cancellable;
  GdkRGBA color;
};

/* Properties */
enum
{
  PROP_DIALOG = 1,
  PROP_RGBA,
  NUM_PROPERTIES
};

/* Signals */
enum
{
  SIGNAL_ACTIVATE = 1,
  NUM_SIGNALS
};

static GParamSpec *properties[NUM_PROPERTIES];

static unsigned int color_dialog_button_signals[NUM_SIGNALS] = { 0 };

G_DEFINE_TYPE (GtkColorDialogButton, gtk_color_dialog_button, GTK_TYPE_WIDGET)

static void
gtk_color_dialog_button_init (GtkColorDialogButton *this)
{
  PangoLayout *layout;
  PangoRectangle rect;
  GtkDragSource *source;
  GtkDropTarget *dest;

  g_signal_connect_swapped (this, "activate", G_CALLBACK (activated), this);

  this->color = GDK_RGBA ("00000000");

  this->button = gtk_button_new ();
  g_signal_connect_swapped (this->button, "clicked", G_CALLBACK (button_clicked), this);
  gtk_widget_set_parent (this->button, GTK_WIDGET (this));

  this->swatch = g_object_new (GTK_TYPE_COLOR_SWATCH,
                               "accessible-role", GTK_ACCESSIBLE_ROLE_IMG,
                               "selectable", FALSE,
                               "has-menu", FALSE,
                               "can-drag", FALSE,
                               NULL);
  gtk_widget_set_can_focus (this->swatch, FALSE);
  gtk_widget_remove_css_class (this->swatch, "activatable");

  layout = gtk_widget_create_pango_layout (GTK_WIDGET (this), "Black");
  pango_layout_get_pixel_extents (layout, NULL, &rect);
  g_object_unref (layout);

  gtk_widget_set_size_request (this->swatch, rect.width, rect.height);

  gtk_button_set_child (GTK_BUTTON (this->button), this->swatch);

  dest = gtk_drop_target_new (GDK_TYPE_RGBA, GDK_ACTION_COPY);
  g_signal_connect (dest, "drop", G_CALLBACK (drop), this);
  gtk_widget_add_controller (GTK_WIDGET (this->button), GTK_EVENT_CONTROLLER (dest));

  source = gtk_drag_source_new ();
  g_signal_connect (source, "prepare", G_CALLBACK (drag_prepare), this);
  gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (source),
                                              GTK_PHASE_CAPTURE);
  gtk_widget_add_controller (this->button, GTK_EVENT_CONTROLLER (source));
  gtk_widget_add_css_class (this->button, "color");

  gtk_color_dialog_button_set_rgba (this, &(GdkRGBA) { 0.75, 0.25, 0.25, 1.0 });
}

static void
gtk_color_dialog_button_unroot (GtkWidget *widget)
{
  GtkColorDialogButton *this = GTK_COLOR_DIALOG_BUTTON (widget);

  if (this->cancellable)
    {
      g_cancellable_cancel (this->cancellable);
      g_clear_object (&this->cancellable);
      update_button_sensitivity (this);
    }

  GTK_WIDGET_CLASS (gtk_color_dialog_button_parent_class)->unroot (widget);
}

static void
gtk_color_dialog_button_set_property (GObject      *object,
                                      unsigned int  param_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  GtkColorDialogButton *this = GTK_COLOR_DIALOG_BUTTON (object);

  switch (param_id)
    {
    case PROP_DIALOG:
      gtk_color_dialog_button_set_dialog (this, g_value_get_object (value));
      break;

    case PROP_RGBA:
      gtk_color_dialog_button_set_rgba (this, g_value_get_boxed (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}

static void
gtk_color_dialog_button_get_property (GObject      *object,
                                      unsigned int  param_id,
                                      GValue       *value,
                                      GParamSpec   *pspec)
{
  GtkColorDialogButton *this = GTK_COLOR_DIALOG_BUTTON (object);

  switch (param_id)
    {
    case PROP_DIALOG:
      g_value_set_object (value, this->dialog);
      break;

    case PROP_RGBA:
      g_value_set_boxed (value, &this->color);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}

static void
gtk_color_dialog_button_dispose (GObject *object)
{
  GtkColorDialogButton *this = GTK_COLOR_DIALOG_BUTTON (object);

  g_clear_pointer (&this->button, gtk_widget_unparent);

  G_OBJECT_CLASS (gtk_color_dialog_button_parent_class)->dispose (object);
}

static void
gtk_color_dialog_button_finalize (GObject *object)
{
  GtkColorDialogButton *this = GTK_COLOR_DIALOG_BUTTON (object);

  g_assert (this->cancellable == NULL);
  g_clear_object (&this->dialog);

  G_OBJECT_CLASS (gtk_color_dialog_button_parent_class)->finalize (object);
}

static void
gtk_color_dialog_button_class_init (GtkColorDialogButtonClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->get_property = gtk_color_dialog_button_get_property;
  object_class->set_property = gtk_color_dialog_button_set_property;
  object_class->dispose = gtk_color_dialog_button_dispose;
  object_class->finalize = gtk_color_dialog_button_finalize;

  widget_class->grab_focus = gtk_widget_grab_focus_child;
  widget_class->focus = gtk_widget_focus_child;
  widget_class->unroot = gtk_color_dialog_button_unroot;

  /**
   * GtkColorDialogButton:dialog:
   *
   * The `GtkColorDialog` that contains parameters for
   * the color chooser dialog.
   *
   * Since: 4.10
   */
  properties[PROP_DIALOG] =
      g_param_spec_object ("dialog", NULL, NULL,
                           GTK_TYPE_COLOR_DIALOG,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkColorDialogButton:rgba:
   *
   * The selected color.
   *
   * This property can be set to give the button its initial
   * color, and it will be updated to reflect the users choice
   * in the color chooser dialog.
   *
   * Listen to `notify::rgba` to get informed about changes
   * to the buttons color.
   *
   * Since: 4.10
   */
  properties[PROP_RGBA] =
      g_param_spec_boxed ("rgba", NULL, NULL,
                          GDK_TYPE_RGBA,
                          G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, NUM_PROPERTIES, properties);

  /**
   * GtkColorDialogButton::activate:
   * @widget: the object which received the signal
   *
   * Emitted when the color dialog button is activated.
   *
   * The `::activate` signal on `GtkColorDialogButton` is an action signal
   * and emitting it causes the button to pop up its dialog.
   *
   * Since: 4.14
   */
  color_dialog_button_signals[SIGNAL_ACTIVATE] =
    g_signal_new (I_ ("activate"),
                  G_TYPE_FROM_CLASS (class),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 0);

  gtk_widget_class_set_activate_signal (widget_class, color_dialog_button_signals[SIGNAL_ACTIVATE]);
  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_css_name (widget_class, "colorbutton");
  gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_GROUP);
}

/* }}} */
/* {{{ Private API, callbacks */

static guint
scale_round (double value,
             double scale)
{
  value = floor (value * scale + 0.5);
  value = CLAMP (value, 0, scale);
  return (guint)value;
}

static char *
accessible_color_name (const GdkRGBA *color)
{
  if (color->alpha < 1.0)
    return g_strdup_printf (_("Red %d%%, Green %d%%, Blue %d%%, Alpha %d%%"),
                            scale_round (color->red, 100),
                            scale_round (color->green, 100),
                            scale_round (color->blue, 100),
                            scale_round (color->alpha, 100));
  else
    return g_strdup_printf (_("Red %d%%, Green %d%%, Blue %d%%"),
                            scale_round (color->red, 100),
                            scale_round (color->green, 100),
                            scale_round (color->blue, 100));
}

static gboolean
drop (GtkDropTarget        *dest,
      const GValue         *value,
      double                x,
      double                y,
      GtkColorDialogButton *this)
{
  GdkRGBA *color = g_value_get_boxed (value);

  gtk_color_dialog_button_set_rgba (this, color);

  return TRUE;
}

static GdkContentProvider *
drag_prepare (GtkDragSource        *source,
              double                x,
              double                y,
              GtkColorDialogButton *this)
{
  GdkRGBA color;

  gtk_color_swatch_get_rgba (GTK_COLOR_SWATCH (this->swatch), &color);

  return gdk_content_provider_new_typed (GDK_TYPE_RGBA, &color);
}

static void
update_button_sensitivity (GtkColorDialogButton *this)
{
  if (this->button)
    gtk_widget_set_sensitive (this->button,
                              this->dialog != NULL && this->cancellable == NULL);
}

static void
color_chosen (GObject      *source,
              GAsyncResult *result,
              gpointer      data)
{
  GtkColorDialog *dialog = GTK_COLOR_DIALOG (source);
  GtkColorDialogButton *this = data;
  GdkRGBA *color;

  color = gtk_color_dialog_choose_rgba_finish (dialog, result, NULL);
  if (color)
    {
      gtk_color_dialog_button_set_rgba (this, color);
      gdk_rgba_free (color);
    }

  g_clear_object (&this->cancellable);
  update_button_sensitivity (this);
}

static void
activated (GtkColorDialogButton *this)
{
  gtk_widget_activate (this->button);
}

static void
button_clicked (GtkColorDialogButton *this)
{
  GtkRoot *root = gtk_widget_get_root (GTK_WIDGET (this));
  GtkWindow *parent = NULL;

  g_assert (this->cancellable == NULL);
  this->cancellable = g_cancellable_new ();

  update_button_sensitivity (this);

  if (GTK_IS_WINDOW (root))
    parent = GTK_WINDOW (root);

  gtk_color_dialog_choose_rgba (this->dialog, parent, &this->color,
                                this->cancellable, color_chosen, this);
}

/* }}} */
/* {{{ Constructor */

/**
 * gtk_color_dialog_button_new:
 * @dialog: (nullable) (transfer full): the `GtkColorDialog` to use
 *
 * Creates a new `GtkColorDialogButton` with the
 * given `GtkColorDialog`.
 *
 * You can pass `NULL` to this function and set a `GtkColorDialog`
 * later. The button will be insensitive until that happens.
 *
 * Returns: the new `GtkColorDialogButton`
 *
 * Since: 4.10
 */
GtkWidget *
gtk_color_dialog_button_new (GtkColorDialog *dialog)
{
  GtkWidget *this;

  g_return_val_if_fail (dialog == NULL || GTK_IS_COLOR_DIALOG (dialog), NULL);

  this = g_object_new (GTK_TYPE_COLOR_DIALOG_BUTTON,
                       "dialog", dialog,
                       NULL);

  g_clear_object (&dialog);

  return this;
}

/* }}} */
/* {{{ Getters and setters */

/**
 * gtk_color_dialog_button_get_dialog:
 * @this: a `GtkColorDialogButton`
 *
 * Returns the `GtkColorDialog` of @this.
 *
 * Returns: (transfer none) (nullable): the `GtkColorDialog`
 *
 * Since: 4.10
 */
GtkColorDialog *
gtk_color_dialog_button_get_dialog (GtkColorDialogButton *this)
{
  g_return_val_if_fail (GTK_IS_COLOR_DIALOG_BUTTON (this), NULL);

  return this->dialog;
}

/**
 * gtk_color_dialog_button_set_dialog:
 * @this: a `GtkColorDialogButton`
 * @dialog: the new `GtkColorDialog`
 *
 * Sets a `GtkColorDialog` object to use for
 * creating the color chooser dialog that is
 * presented when the user clicks the button.
 *
 * Since: 4.10
 */
void
gtk_color_dialog_button_set_dialog (GtkColorDialogButton *this,
                                    GtkColorDialog       *dialog)
{
  g_return_if_fail (GTK_IS_COLOR_DIALOG_BUTTON (this));
  g_return_if_fail (dialog == NULL || GTK_IS_COLOR_DIALOG (dialog));

  if (!g_set_object (&this->dialog, dialog))
    return;

  update_button_sensitivity (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_DIALOG]);
}

/**
 * gtk_color_dialog_button_get_rgba:
 * @this: a `GtkColorDialogButton`
 *
 * Returns the color of the button.
 *
 * This function is what should be used to obtain
 * the color that was chosen by the user. To get
 * informed about changes, listen to "notify::rgba".
 *
 * Returns: the color
 *
 * Since: 4.10
 */
const GdkRGBA *
gtk_color_dialog_button_get_rgba (GtkColorDialogButton *this)
{
  g_return_val_if_fail (GTK_IS_COLOR_DIALOG_BUTTON (this), NULL);

  return &this->color;
}

/**
 * gtk_color_dialog_button_set_rgba:
 * @this: a `GtkColorDialogButton`
 * @color: the new color
 *
 * Sets the color of the button.
 *
 * Since: 4.10
 */
void
gtk_color_dialog_button_set_rgba (GtkColorDialogButton *this,
                                  const GdkRGBA        *color)
{
  char *text;

  g_return_if_fail (GTK_IS_COLOR_DIALOG_BUTTON (this));
  g_return_if_fail (color != NULL);

  if (gdk_rgba_equal (&this->color, color))
    return;

  this->color = *color;
  gtk_color_swatch_set_rgba (GTK_COLOR_SWATCH (this->swatch), color);

  text = accessible_color_name (color);
  gtk_accessible_update_property (GTK_ACCESSIBLE (this->swatch),
                                  GTK_ACCESSIBLE_PROPERTY_LABEL, text,
                                  -1);
  g_free (text);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_RGBA]);
}

/* }}} */

/* vim:set foldmethod=marker: */
