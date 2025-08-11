/* gtkappchooserbutton.c: an app-chooser button
 *
 * Copyright (C) 2010 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Cosimo Cecchi <ccecchi@redhat.com>
 */

/**
 * GtkAppChooserButton:
 *
 * The `GtkAppChooserButton` lets the user select an application.
 *
 * <picture>
 *   <source srcset="appchooserbutton-dark.png" media="(prefers-color-scheme: dark)">
 *   <img alt="An example GtkAppChooserButton" src="appchooserbutton.png">
 * </picture>
 *
 * Initially, a `GtkAppChooserButton` selects the first application
 * in its list, which will either be the most-recently used application
 * or, if [property@Gtk.AppChooserButton:show-default-item] is %TRUE, the
 * default application.
 *
 * The list of applications shown in a `GtkAppChooserButton` includes
 * the recommended applications for the given content type. When
 * [property@Gtk.AppChooserButton:show-default-item] is set, the default
 * application is also included. To immutable the user chooser other applications,
 * you can set the [property@Gtk.AppChooserButton:show-dialog-item] property,
 * which allows to open a full [class@Gtk.AppChooserDialog].
 *
 * It is possible to add custom items to the list, using
 * [method@Gtk.AppChooserButton.append_custom_item]. These items cause
 * the [signal@Gtk.AppChooserButton::custom-item-activated] signal to be
 * emitted when they are selected.
 *
 * To track changes in the selected application, use the
 * [signal@Gtk.AppChooserButton::changed] signal.
 *
 * ## CSS nodes
 *
 * `GtkAppChooserButton` has a single CSS node with the name “appchooserbutton”.
 *
 * Deprecated: 4.10: The application selection widgets should be
 *   implemented according to the design of each platform and/or
 *   application requiring them.
 */
#include "config.h"

#include "gtkappchooserbutton.h"

#include "gtkappchooser.h"
#include "gtkappchooserdialog.h"
#include "gtkappchooserprivate.h"
#include "gtkcelllayout.h"
#include "gtkcellrendererpixbuf.h"
#include "gtkcellrenderertext.h"
#include "gtkcombobox.h"
#include "gtkwidgetprivate.h"
#include "gtkdialog.h"
#include <glib/gi18n-lib.h>
#include "gtkmarshalers.h"
#include "gtkliststore.h"
#include "gtkprivate.h"

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

enum {
  PROP_SHOW_DIALOG_ITEM = 1,
  PROP_SHOW_DEFAULT_ITEM,
  PROP_HEADING,
  PROP_MODAL,
  NUM_PROPERTIES,

  PROP_CONTENT_TYPE = NUM_PROPERTIES
};

enum {
  SIGNAL_CHANGED,
  SIGNAL_CUSTOM_ITEM_ACTIVATED,
  ACTIVATE,
  NUM_SIGNALS
};

enum {
  COLUMN_APP_INFO,
  COLUMN_NAME,
  COLUMN_LABEL,
  COLUMN_ICON,
  COLUMN_CUSTOM,
  COLUMN_SEPARATOR,
  NUM_COLUMNS,
};

#define CUSTOM_ITEM_OTHER_APP "gtk-internal-item-other-app"

static void app_chooser_iface_init  (GtkAppChooserIface *iface);

static void real_insert_custom_item (GtkAppChooserButton *this,
                                     const char          *name,
                                     const char          *label,
                                     GIcon               *icon,
                                     gboolean             custom,
                                     GtkTreeIter         *iter);

static void real_insert_separator   (GtkAppChooserButton *this,
                                     gboolean             custom,
                                     GtkTreeIter         *iter);

static guint signals[NUM_SIGNALS] = { 0, };
static GParamSpec *properties[NUM_PROPERTIES];

typedef struct _GtkAppChooserButtonClass   GtkAppChooserButtonClass;

struct _GtkAppChooserButton {
  GtkWidget parent_instance;

  GtkWidget *combobox;
  GtkListStore *store;

  char *content_type;
  char *heading;
  int last_active;
  gboolean show_dialog_item;
  gboolean show_default_item;
  gboolean modal;

  GHashTable *custom_item_names;
};

struct _GtkAppChooserButtonClass {
  GtkWidgetClass parent_class;

  void (* changed)               (GtkAppChooserButton *this);
  void (* custom_item_activated) (GtkAppChooserButton *this,
                                  const char *item_name);
  void (* activate)              (GtkAppChooserButton *this);
};

G_DEFINE_TYPE_WITH_CODE (GtkAppChooserButton, gtk_app_chooser_button, GTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_APP_CHOOSER,
                                                app_chooser_iface_init));

static gboolean
row_separator_func (GtkTreeModel *model,
                    GtkTreeIter  *iter,
                    gpointer      user_data)
{
  gboolean separator;

  gtk_tree_model_get (model, iter,
                      COLUMN_SEPARATOR, &separator,
                      -1);

  return separator;
}

static void
get_first_iter (GtkListStore *store,
                GtkTreeIter  *iter)
{
  GtkTreeIter iter2;

  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), iter))
    {
      /* the model is empty, append */
      gtk_list_store_append (store, iter);
    }
  else
    {
      gtk_list_store_insert_before (store, &iter2, iter);
      *iter = iter2;
    }
}

typedef struct {
  GtkAppChooserButton *this;
  GAppInfo *info;
  int active_index;
} SelectAppData;

static void
select_app_data_free (SelectAppData *data)
{
  g_clear_object (&data->this);
  g_clear_object (&data->info);

  g_slice_free (SelectAppData, data);
}

static gboolean
select_application_func_cb (GtkTreeModel *model,
                            GtkTreePath  *path,
                            GtkTreeIter  *iter,
                            gpointer      user_data)
{
  SelectAppData *data = user_data;
  GtkAppChooserButton *this = data->this;
  GAppInfo *app_to_match = data->info;
  GAppInfo *app = NULL;
  gboolean custom;
  gboolean result;

  gtk_tree_model_get (model, iter,
                      COLUMN_APP_INFO, &app,
                      COLUMN_CUSTOM, &custom,
                      -1);

  /* custom items are always after GAppInfos, so iterating further here
   * is just useless.
   */
  if (custom)
    result = TRUE;
  else if (g_app_info_equal (app, app_to_match))
    {
      gtk_combo_box_set_active_iter (GTK_COMBO_BOX (this->combobox), iter);
      result = TRUE;
    }
  else
    result = FALSE;

  g_object_unref (app);

  return result;
}

static void
gtk_app_chooser_button_select_application (GtkAppChooserButton *this,
                                           GAppInfo            *info)
{
  SelectAppData *data;

  data = g_slice_new0 (SelectAppData);
  data->this = g_object_ref (this);
  data->info = g_object_ref (info);

  gtk_tree_model_foreach (GTK_TREE_MODEL (this->store),
                          select_application_func_cb, data);

  select_app_data_free (data);
}

static void
other_application_dialog_response_cb (GtkDialog *dialog,
                                      int        response_id,
                                      gpointer   user_data)
{
  GtkAppChooserButton *this = user_data;
  GAppInfo *info;

  if (response_id != GTK_RESPONSE_OK)
    {
      /* reset the active item, otherwise we are stuck on
       * 'Other application…'
       */
      gtk_combo_box_set_active (GTK_COMBO_BOX (this->combobox), this->last_active);
      gtk_window_destroy (GTK_WINDOW (dialog));
      return;
    }

  info = gtk_app_chooser_get_app_info (GTK_APP_CHOOSER (dialog));

  gtk_window_destroy (GTK_WINDOW (dialog));

  /* refresh the combobox to get the new application */
  gtk_app_chooser_refresh (GTK_APP_CHOOSER (this));
  gtk_app_chooser_button_select_application (this, info);

  g_object_unref (info);
}

static void
other_application_item_activated_cb (GtkAppChooserButton *this)
{
  GtkWidget *dialog, *widget;
  GtkRoot *root;

  root = gtk_widget_get_root (GTK_WIDGET (this));
  dialog = gtk_app_chooser_dialog_new_for_content_type (GTK_WINDOW (root),
                                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                                        this->content_type);
  gtk_window_set_modal (GTK_WINDOW (dialog), this->modal | gtk_window_get_modal (GTK_WINDOW (root)));
  gtk_app_chooser_dialog_set_heading (GTK_APP_CHOOSER_DIALOG (dialog), this->heading);

  widget = gtk_app_chooser_dialog_get_widget (GTK_APP_CHOOSER_DIALOG (dialog));
  g_object_set (widget,
                "show-fallback", TRUE,
                "show-other", TRUE,
                NULL);
  gtk_widget_show (dialog);

  g_signal_connect (dialog, "response",
                    G_CALLBACK (other_application_dialog_response_cb), this);
}

static void
gtk_app_chooser_button_ensure_dialog_item (GtkAppChooserButton *this,
                                           GtkTreeIter         *prev_iter)
{
  GtkTreeIter iter, iter2;

  if (!this->show_dialog_item || !this->content_type)
    return;

  if (prev_iter == NULL)
    gtk_list_store_append (this->store, &iter);
  else
    gtk_list_store_insert_after (this->store, &iter, prev_iter);

  real_insert_separator (this, FALSE, &iter);
  iter2 = iter;

  gtk_list_store_insert_after (this->store, &iter, &iter2);
  real_insert_custom_item (this, CUSTOM_ITEM_OTHER_APP,
                           _("Other app…"), NULL,
                           FALSE, &iter);
}

static void
insert_one_application (GtkAppChooserButton *this,
                        GAppInfo            *app,
                        GtkTreeIter         *iter)
{
  GIcon *icon;

  icon = g_app_info_get_icon (app);

  if (icon == NULL)
    icon = g_themed_icon_new ("application-x-executable");
  else
    g_object_ref (icon);

  gtk_list_store_set (this->store, iter,
                      COLUMN_APP_INFO, app,
                      COLUMN_LABEL, g_app_info_get_name (app),
                      COLUMN_ICON, icon,
                      COLUMN_CUSTOM, FALSE,
                      -1);

  g_object_unref (icon);
}

static void
gtk_app_chooser_button_populate (GtkAppChooserButton *this)
{
  GList *recommended_apps = NULL, *l;
  GAppInfo *app, *default_app = NULL;
  GtkTreeIter iter, iter2;
  gboolean cycled_recommended;

#ifndef G_OS_WIN32
  if (this->content_type)
    recommended_apps = g_app_info_get_recommended_for_type (this->content_type);
#endif
  cycled_recommended = FALSE;

  if (this->show_default_item)
    {
      if (this->content_type)
        default_app = g_app_info_get_default_for_type (this->content_type, FALSE);

      if (default_app != NULL)
        {
          get_first_iter (this->store, &iter);
          cycled_recommended = TRUE;

          insert_one_application (this, default_app, &iter);

          g_object_unref (default_app);
        }
    }

  for (l = recommended_apps; l != NULL; l = l->next)
    {
      app = l->data;

      if (default_app != NULL && g_app_info_equal (app, default_app))
        continue;

      if (cycled_recommended)
        {
          gtk_list_store_insert_after (this->store, &iter2, &iter);
          iter = iter2;
        }
      else
        {
          get_first_iter (this->store, &iter);
          cycled_recommended = TRUE;
        }

      insert_one_application (this, app, &iter);
    }

  if (recommended_apps != NULL)
    g_list_free_full (recommended_apps, g_object_unref);

  if (!cycled_recommended)
    gtk_app_chooser_button_ensure_dialog_item (this, NULL);
  else
    gtk_app_chooser_button_ensure_dialog_item (this, &iter);

  gtk_combo_box_set_active (GTK_COMBO_BOX (this->combobox), 0);
}

static void
gtk_app_chooser_button_build_ui (GtkAppChooserButton *this)
{
  GtkCellRenderer *cell;
  GtkCellArea *area;

  gtk_combo_box_set_model (GTK_COMBO_BOX (this->combobox),
                           GTK_TREE_MODEL (this->store));

  area = gtk_cell_layout_get_area (GTK_CELL_LAYOUT (this->combobox));

  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (this->combobox),
                                        row_separator_func, NULL, NULL);

  cell = gtk_cell_renderer_pixbuf_new ();
  gtk_cell_area_add_with_properties (area, cell,
                                     "align", FALSE,
                                     "expand", FALSE,
                                     "fixed-size", FALSE,
                                     NULL);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (this->combobox), cell,
                                  "gicon", COLUMN_ICON,
                                  NULL);

  cell = gtk_cell_renderer_text_new ();
  gtk_cell_area_add_with_properties (area, cell,
                                     "align", FALSE,
                                     "expand", TRUE,
                                     NULL);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (this->combobox), cell,
                                  "text", COLUMN_LABEL,
                                  NULL);

  gtk_app_chooser_button_populate (this);
}

static void
gtk_app_chooser_button_remove_non_custom (GtkAppChooserButton *this)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean custom, res;

  model = GTK_TREE_MODEL (this->store);

  if (!gtk_tree_model_get_iter_first (model, &iter))
    return;

  do {
    gtk_tree_model_get (model, &iter,
                        COLUMN_CUSTOM, &custom,
                        -1);
    if (custom)
      res = gtk_tree_model_iter_next (model, &iter);
    else
      res = gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
  } while (res);
}

static void
gtk_app_chooser_button_changed (GtkComboBox *object,
                                gpointer     user_data)
{
  GtkAppChooserButton *this = user_data;
  GtkTreeIter iter;
  char *name = NULL;
  gboolean custom;
  GQuark name_quark;

  if (!gtk_combo_box_get_active_iter (object, &iter))
    return;

  gtk_tree_model_get (GTK_TREE_MODEL (this->store), &iter,
                      COLUMN_NAME, &name,
                      COLUMN_CUSTOM, &custom,
                      -1);

  if (name != NULL)
    {
      if (custom)
        {
          name_quark = g_quark_from_string (name);
          g_signal_emit (this, signals[SIGNAL_CUSTOM_ITEM_ACTIVATED], name_quark, name);
          this->last_active = gtk_combo_box_get_active (object);
        }
      else
        {
          /* trigger the dialog internally */
          other_application_item_activated_cb (this);
        }

      g_free (name);
    }
  else
    this->last_active = gtk_combo_box_get_active (object);

  g_signal_emit (this, signals[SIGNAL_CHANGED], 0);
}

static void
gtk_app_chooser_button_refresh (GtkAppChooser *object)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (object);

  gtk_app_chooser_button_remove_non_custom (this);
  gtk_app_chooser_button_populate (this);
}

static GAppInfo *
gtk_app_chooser_button_get_app_info (GtkAppChooser *object)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (object);
  GtkTreeIter iter;
  GAppInfo *info;

  if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (this->combobox), &iter))
    return NULL;

  gtk_tree_model_get (GTK_TREE_MODEL (this->store), &iter,
                      COLUMN_APP_INFO, &info,
                      -1);

  return info;
}

static void
gtk_app_chooser_button_constructed (GObject *obj)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (obj);

  if (G_OBJECT_CLASS (gtk_app_chooser_button_parent_class)->constructed != NULL)
    G_OBJECT_CLASS (gtk_app_chooser_button_parent_class)->constructed (obj);

  gtk_app_chooser_button_build_ui (this);
}

static void
gtk_app_chooser_button_set_property (GObject      *obj,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (obj);

  switch (property_id)
    {
    case PROP_CONTENT_TYPE:
      this->content_type = g_value_dup_string (value);
      break;
    case PROP_SHOW_DIALOG_ITEM:
      gtk_app_chooser_button_set_show_dialog_item (this, g_value_get_boolean (value));
      break;
    case PROP_SHOW_DEFAULT_ITEM:
      gtk_app_chooser_button_set_show_default_item (this, g_value_get_boolean (value));
      break;
    case PROP_HEADING:
      gtk_app_chooser_button_set_heading (this, g_value_get_string (value));
      break;
    case PROP_MODAL:
      gtk_app_chooser_button_set_modal (this, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, property_id, pspec);
      break;
    }
}

static void
gtk_app_chooser_button_get_property (GObject    *obj,
                                     guint       property_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (obj);

  switch (property_id)
    {
    case PROP_CONTENT_TYPE:
      g_value_set_string (value, this->content_type);
      break;
    case PROP_SHOW_DIALOG_ITEM:
      g_value_set_boolean (value, this->show_dialog_item);
      break;
    case PROP_SHOW_DEFAULT_ITEM:
      g_value_set_boolean (value, this->show_default_item);
      break;
    case PROP_HEADING:
      g_value_set_string (value, this->heading);
      break;
    case PROP_MODAL:
      g_value_set_boolean (value, this->modal);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, property_id, pspec);
      break;
    }
}

static void
gtk_app_chooser_button_finalize (GObject *obj)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (obj);

  g_hash_table_destroy (this->custom_item_names);
  g_free (this->content_type);
  g_free (this->heading);
  g_object_unref (this->store);
  gtk_widget_unparent (this->combobox);

  G_OBJECT_CLASS (gtk_app_chooser_button_parent_class)->finalize (obj);
}

static void
gtk_app_chooser_button_measure (GtkWidget       *widget,
                                GtkOrientation  orientation,
                                int             for_size,
                                int            *minimum,
                                int            *natural,
                                int            *minimum_baseline,
                                int            *natural_baseline)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (widget);

  gtk_widget_measure (this->combobox, orientation, for_size,
                      minimum, natural,
                      minimum_baseline, natural_baseline);
}

static void
gtk_app_chooser_button_size_allocate (GtkWidget *widget,
                                      int        width,
                                      int        height,
                                      int        baseline)
{
  GtkAppChooserButton *this = GTK_APP_CHOOSER_BUTTON (widget);

  gtk_widget_size_allocate (this->combobox, &(GtkAllocation){0, 0, width, height}, baseline);
}

static void
app_chooser_iface_init (GtkAppChooserIface *iface)
{
  iface->get_app_info = gtk_app_chooser_button_get_app_info;
  iface->refresh = gtk_app_chooser_button_refresh;
}

static void
gtk_app_chooser_button_activate (GtkAppChooserButton *this)
{
  gtk_widget_activate (this->combobox);
}

static void
gtk_app_chooser_button_class_init (GtkAppChooserButtonClass *klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  oclass->set_property = gtk_app_chooser_button_set_property;
  oclass->get_property = gtk_app_chooser_button_get_property;
  oclass->finalize = gtk_app_chooser_button_finalize;
  oclass->constructed = gtk_app_chooser_button_constructed;

  widget_class->measure = gtk_app_chooser_button_measure;
  widget_class->size_allocate = gtk_app_chooser_button_size_allocate;
  widget_class->grab_focus = gtk_widget_grab_focus_child;
  widget_class->focus = gtk_widget_focus_child;

  klass->activate = gtk_app_chooser_button_activate;

  g_object_class_override_property (oclass, PROP_CONTENT_TYPE, "content-type");

  /**
   * GtkAppChooserButton:show-dialog-item:
   *
   * Determines whether the dropdown menu shows an item to open
   * a `GtkAppChooserDialog`.
   */
  properties[PROP_SHOW_DIALOG_ITEM] =
    g_param_spec_boolean ("show-dialog-item", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE|G_PARAM_CONSTRUCT|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkAppChooserButton:show-default-item:
   *
   * Determines whether the dropdown menu shows the default application
   * on top for the provided content type.
   */
  properties[PROP_SHOW_DEFAULT_ITEM] =
    g_param_spec_boolean ("show-default-item", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE|G_PARAM_CONSTRUCT|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkAppChooserButton:heading:
   *
   * The text to show at the top of the dialog that can be
   * opened from the button.
   *
   * The string may contain Pango markup.
   */
  properties[PROP_HEADING] =
    g_param_spec_string ("heading", NULL, NULL,
                         NULL,
                         G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkAppChooserButton:modal:
   *
   * Whether the app chooser dialog should be modal.
   */
  properties[PROP_MODAL] =
    g_param_spec_boolean ("modal", NULL, NULL,
                          TRUE,
                          G_PARAM_READWRITE|G_PARAM_CONSTRUCT|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);
  g_object_class_install_properties (oclass, NUM_PROPERTIES, properties);

  /**
   * GtkAppChooserButton::changed:
   * @this: the object which received the signal
   *
   * Emitted when the active application changes.
   */
  signals[SIGNAL_CHANGED] =
    g_signal_new (I_("changed"),
                  G_OBJECT_CLASS_TYPE (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtkAppChooserButtonClass, changed),
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 0);


  /**
   * GtkAppChooserButton::custom-item-activated:
   * @this: the object which received the signal
   * @item_name: the name of the activated item
   *
   * Emitted when a custom item is activated.
   *
   * Use [method@Gtk.AppChooserButton.append_custom_item],
   * to add custom items.
   */
  signals[SIGNAL_CUSTOM_ITEM_ACTIVATED] =
    g_signal_new (I_("custom-item-activated"),
                  GTK_TYPE_APP_CHOOSER_BUTTON,
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_DETAILED,
                  G_STRUCT_OFFSET (GtkAppChooserButtonClass, custom_item_activated),
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE,
                  1, G_TYPE_STRING);

  /**
   * GtkAppChooserButton::activate:
   * @widget: the object which received the signal.
   *
   * Emitted to when the button is activated.
   *
   * The `::activate` signal on `GtkAppChooserButton` is an action signal and
   * emitting it causes the button to pop up its dialog.
   *
   * Since: 4.4
   */
  signals[ACTIVATE] =
      g_signal_new (I_ ("activate"),
                    G_OBJECT_CLASS_TYPE (oclass),
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                    G_STRUCT_OFFSET (GtkAppChooserButtonClass, activate),
                    NULL, NULL,
                    NULL,
                    G_TYPE_NONE, 0);

  gtk_widget_class_set_activate_signal (widget_class, signals[ACTIVATE]);


  gtk_widget_class_set_css_name (widget_class, I_("appchooserbutton"));
}

static void
gtk_app_chooser_button_init (GtkAppChooserButton *this)
{
  this->modal = TRUE;

  this->custom_item_names = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  this->store = gtk_list_store_new (NUM_COLUMNS,
                                    G_TYPE_APP_INFO,
                                    G_TYPE_STRING, /* name */
                                    G_TYPE_STRING, /* label */
                                    G_TYPE_ICON,
                                    G_TYPE_BOOLEAN, /* separator */
                                    G_TYPE_BOOLEAN); /* custom */
  this->combobox = gtk_combo_box_new_with_model (GTK_TREE_MODEL (this->store));
  gtk_widget_set_parent (this->combobox, GTK_WIDGET (this));

  g_signal_connect (this->combobox, "changed",
                    G_CALLBACK (gtk_app_chooser_button_changed), this);
}

static gboolean
app_chooser_button_iter_from_custom_name (GtkAppChooserButton *this,
                                          const char          *name,
                                          GtkTreeIter         *set_me)
{
  GtkTreeIter iter;
  char *custom_name = NULL;

  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (this->store), &iter))
    return FALSE;

  do {
    gtk_tree_model_get (GTK_TREE_MODEL (this->store), &iter,
                        COLUMN_NAME, &custom_name,
                        -1);

    if (g_strcmp0 (custom_name, name) == 0)
      {
        g_free (custom_name);
        *set_me = iter;

        return TRUE;
      }

    g_free (custom_name);
  } while (gtk_tree_model_iter_next (GTK_TREE_MODEL (this->store), &iter));

  return FALSE;
}

static void
real_insert_custom_item (GtkAppChooserButton *this,
                         const char          *name,
                         const char          *label,
                         GIcon               *icon,
                         gboolean             custom,
                         GtkTreeIter         *iter)
{
  if (custom)
    {
      if (g_hash_table_lookup (this->custom_item_names, name) != NULL)
        {
          g_warning ("Attempting to add custom item %s to GtkAppChooserButton, "
                     "when there's already an item with the same name", name);
          return;
        }

      g_hash_table_insert (this->custom_item_names,
                           g_strdup (name), GINT_TO_POINTER (1));
    }

  gtk_list_store_set (this->store, iter,
                      COLUMN_NAME, name,
                      COLUMN_LABEL, label,
                      COLUMN_ICON, icon,
                      COLUMN_CUSTOM, custom,
                      COLUMN_SEPARATOR, FALSE,
                      -1);
}

static void
real_insert_separator (GtkAppChooserButton *this,
                       gboolean             custom,
                       GtkTreeIter         *iter)
{
  gtk_list_store_set (this->store, iter,
                      COLUMN_CUSTOM, custom,
                      COLUMN_SEPARATOR, TRUE,
                      -1);
}

/**
 * gtk_app_chooser_button_new:
 * @content_type: the content type to show applications for
 *
 * Creates a new `GtkAppChooserButton` for applications
 * that can handle content of the given type.
 *
 * Returns: a newly created `GtkAppChooserButton`
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
GtkWidget *
gtk_app_chooser_button_new (const char *content_type)
{
  g_return_val_if_fail (content_type != NULL, NULL);

  return g_object_new (GTK_TYPE_APP_CHOOSER_BUTTON,
                       "content-type", content_type,
                       NULL);
}

/**
 * gtk_app_chooser_button_append_separator:
 * @this: a `GtkAppChooserButton`
 *
 * Appends a separator to the list of applications that is shown
 * in the popup.
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
void
gtk_app_chooser_button_append_separator (GtkAppChooserButton *this)
{
  GtkTreeIter iter;

  g_return_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this));

  gtk_list_store_append (this->store, &iter);
  real_insert_separator (this, TRUE, &iter);
}

/**
 * gtk_app_chooser_button_append_custom_item:
 * @this: a `GtkAppChooserButton`
 * @name: the name of the custom item
 * @label: the label for the custom item
 * @icon: the icon for the custom item
 *
 * Appends a custom item to the list of applications that is shown
 * in the popup.
 *
 * The item name must be unique per-widget. Clients can use the
 * provided name as a detail for the
 * [signal@Gtk.AppChooserButton::custom-item-activated] signal, to add a
 * callback for the activation of a particular custom item in the list.
 *
 * See also [method@Gtk.AppChooserButton.append_separator].
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
void
gtk_app_chooser_button_append_custom_item (GtkAppChooserButton *this,
                                           const char          *name,
                                           const char          *label,
                                           GIcon               *icon)
{
  GtkTreeIter iter;

  g_return_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this));
  g_return_if_fail (name != NULL);

  gtk_list_store_append (this->store, &iter);
  real_insert_custom_item (this, name, label, icon, TRUE, &iter);
}

/**
 * gtk_app_chooser_button_set_active_custom_item:
 * @this: a `GtkAppChooserButton`
 * @name: the name of the custom item
 *
 * Selects a custom item.
 *
 * See [method@Gtk.AppChooserButton.append_custom_item].
 *
 * Use [method@Gtk.AppChooser.refresh] to bring the selection
 * to its initial state.
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
void
gtk_app_chooser_button_set_active_custom_item (GtkAppChooserButton *this,
                                               const char          *name)
{
  GtkTreeIter iter;

  g_return_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this));
  g_return_if_fail (name != NULL);

  if (!g_hash_table_contains (this->custom_item_names, name) ||
      !app_chooser_button_iter_from_custom_name (this, name, &iter))
    {
      g_warning ("Can't find the item named %s in the app chooser.", name);
      return;
    }

  gtk_combo_box_set_active_iter (GTK_COMBO_BOX (this->combobox), &iter);
}

/**
 * gtk_app_chooser_button_get_show_dialog_item:
 * @this: a `GtkAppChooserButton`
 *
 * Returns whether the dropdown menu shows an item
 * for a `GtkAppChooserDialog`.
 *
 * Returns: the value of [property@Gtk.AppChooserButton:show-dialog-item]
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
gboolean
gtk_app_chooser_button_get_show_dialog_item (GtkAppChooserButton *this)
{
  g_return_val_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this), FALSE);

  return this->show_dialog_item;
}

/**
 * gtk_app_chooser_button_set_show_dialog_item:
 * @this: a `GtkAppChooserButton`
 * @setting: the new value for [property@Gtk.AppChooserButton:show-dialog-item]
 *
 * Sets whether the dropdown menu of this button should show an
 * entry to trigger a `GtkAppChooserDialog`.
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
void
gtk_app_chooser_button_set_show_dialog_item (GtkAppChooserButton *this,
                                             gboolean             setting)
{
  if (this->show_dialog_item != setting)
    {
      this->show_dialog_item = setting;

      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SHOW_DIALOG_ITEM]);

      gtk_app_chooser_refresh (GTK_APP_CHOOSER (this));
    }
}

/**
 * gtk_app_chooser_button_get_show_default_item:
 * @this: a `GtkAppChooserButton`
 *
 * Returns whether the dropdown menu should show the default
 * application at the top.
 *
 * Returns: the value of [property@Gtk.AppChooserButton:show-default-item]
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
gboolean
gtk_app_chooser_button_get_show_default_item (GtkAppChooserButton *this)
{
  g_return_val_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this), FALSE);

  return this->show_default_item;
}

/**
 * gtk_app_chooser_button_set_show_default_item:
 * @this: a `GtkAppChooserButton`
 * @setting: the new value for [property@Gtk.AppChooserButton:show-default-item]
 *
 * Sets whether the dropdown menu of this button should show the
 * default application for the given content type at top.
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
void
gtk_app_chooser_button_set_show_default_item (GtkAppChooserButton *this,
                                              gboolean             setting)
{
  g_return_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this));

  if (this->show_default_item != setting)
    {
      this->show_default_item = setting;

      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SHOW_DEFAULT_ITEM]);

      gtk_app_chooser_refresh (GTK_APP_CHOOSER (this));
    }
}

/**
 * gtk_app_chooser_button_set_heading:
 * @this: a `GtkAppChooserButton`
 * @heading: a string containing Pango markup
 *
 * Sets the text to display at the top of the dialog.
 *
 * If the heading is not set, the dialog displays a default text.
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
void
gtk_app_chooser_button_set_heading (GtkAppChooserButton *this,
                                    const char          *heading)
{
  g_return_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this));

  g_free (this->heading);
  this->heading = g_strdup (heading);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_HEADING]);
}

/**
 * gtk_app_chooser_button_get_heading:
 * @this: a `GtkAppChooserButton`
 *
 * Returns the text to display at the top of the dialog.
 *
 * Returns: (nullable): the text to display at the top of the dialog,
 *   or %NULL, in which case a default text is displayed
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
const char *
gtk_app_chooser_button_get_heading (GtkAppChooserButton *this)
{
  g_return_val_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this), NULL);

  return this->heading;
}

/**
 * gtk_app_chooser_button_set_modal:
 * @this: a `GtkAppChooserButton`
 * @modal: %TRUE to make the dialog modal
 *
 * Sets whether the dialog should be modal.
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
void
gtk_app_chooser_button_set_modal (GtkAppChooserButton *this,
                                  gboolean             modal)
{
  g_return_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this));

  if (this->modal == modal)
    return;

  this->modal = modal;

  g_object_notify (G_OBJECT (this), "modal");
}

/**
 * gtk_app_chooser_button_get_modal:
 * @this: a `GtkAppChooserButton`
 *
 * Gets whether the dialog is modal.
 *
 * Returns: %TRUE if the dialog is modal
 *
 * Deprecated: 4.10: This widget will be removed in GTK 5
 */
gboolean
gtk_app_chooser_button_get_modal (GtkAppChooserButton *this)
{
  g_return_val_if_fail (GTK_IS_APP_CHOOSER_BUTTON (this), FALSE);

  return this->modal;
}

