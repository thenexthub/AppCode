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

#include "gtkfiledialog.h"

#include "deprecated/gtkdialog.h"
#include "gtkfilechoosernativeprivate.h"
#include "gtkdialogerror.h"
#include <glib/gi18n-lib.h>
#include "gdk/gdkprivate.h"
#include "gdk/gdkdebugprivate.h"
#include "gtktextencodingprivate.h"

/**
 * GtkFileDialog:
 *
 * Asynchronous API to present a file chooser dialog.
 *
 * `GtkFileDialog` collects the arguments that are needed to present
 * the dialog to the user, such as a title for the dialog and whether
 * it should be modal.
 *
 * The dialog is shown with [method@Gtk.FileDialog.open],
 * [method@Gtk.FileDialog.save], etc.
 *
 * Since: 4.10
 */

/* {{{ GObject implementation */

struct _GtkFileDialog
{
  GObject parent_instance;

  char *title;
  char *accept_label;
  unsigned int modal : 1;

  GListModel *filters;
  GtkFileFilter *default_filter;
  GFile *initial_folder;
  char *initial_name;
  GFile *initial_file;
};

enum
{
  PROP_0,
  PROP_ACCEPT_LABEL,
  PROP_DEFAULT_FILTER,
  PROP_FILTERS,
  PROP_INITIAL_FILE,
  PROP_INITIAL_FOLDER,
  PROP_INITIAL_NAME,
  PROP_MODAL,
  PROP_TITLE,

  NUM_PROPERTIES
};

static GParamSpec *properties[NUM_PROPERTIES];

G_DEFINE_TYPE (GtkFileDialog, gtk_file_dialog, G_TYPE_OBJECT)

static void
gtk_file_dialog_init (GtkFileDialog *this)
{
  this->modal = TRUE;
}

static void
gtk_file_dialog_finalize (GObject *object)
{
  GtkFileDialog *this = GTK_FILE_DIALOG (object);

  g_free (this->title);
  g_free (this->accept_label);
  g_clear_object (&this->filters);
  g_clear_object (&this->default_filter);
  g_clear_object (&this->initial_folder);
  g_free (this->initial_name);

  G_OBJECT_CLASS (gtk_file_dialog_parent_class)->finalize (object);
}

static void
gtk_file_dialog_get_property (GObject      *object,
                              unsigned int  property_id,
                              GValue       *value,
                              GParamSpec   *pspec)
{
  GtkFileDialog *this = GTK_FILE_DIALOG (object);

  switch (property_id)
    {
    case PROP_TITLE:
      g_value_set_string (value, this->title);
      break;

    case PROP_MODAL:
      g_value_set_boolean (value, this->modal);
      break;

    case PROP_FILTERS:
      g_value_set_object (value, this->filters);
      break;

    case PROP_DEFAULT_FILTER:
      g_value_set_object (value, this->default_filter);
      break;

    case PROP_INITIAL_FILE:
      g_value_set_object (value, this->initial_file);
      break;

    case PROP_INITIAL_FOLDER:
      g_value_set_object (value, this->initial_folder);
      break;

    case PROP_INITIAL_NAME:
      g_value_set_string (value, this->initial_name);
      break;

    case PROP_ACCEPT_LABEL:
      g_value_set_string (value, this->accept_label);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_file_dialog_set_property (GObject      *object,
                              unsigned int  prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GtkFileDialog *this = GTK_FILE_DIALOG (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      gtk_file_dialog_set_title (this, g_value_get_string (value));
      break;

    case PROP_MODAL:
      gtk_file_dialog_set_modal (this, g_value_get_boolean (value));
      break;

    case PROP_FILTERS:
      gtk_file_dialog_set_filters (this, g_value_get_object (value));
      break;

    case PROP_DEFAULT_FILTER:
      gtk_file_dialog_set_default_filter (this, g_value_get_object (value));
      break;

    case PROP_INITIAL_FILE:
      gtk_file_dialog_set_initial_file (this, g_value_get_object (value));
      break;

    case PROP_INITIAL_FOLDER:
      gtk_file_dialog_set_initial_folder (this, g_value_get_object (value));
      break;

    case PROP_INITIAL_NAME:
      gtk_file_dialog_set_initial_name (this, g_value_get_string (value));
      break;

    case PROP_ACCEPT_LABEL:
      gtk_file_dialog_set_accept_label (this, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_file_dialog_class_init (GtkFileDialogClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->finalize = gtk_file_dialog_finalize;
  object_class->get_property = gtk_file_dialog_get_property;
  object_class->set_property = gtk_file_dialog_set_property;

  /**
   * GtkFileDialog:title:
   *
   * A title that may be shown on the file chooser dialog.
   *
   * Since: 4.10
   */
  properties[PROP_TITLE] =
      g_param_spec_string ("title", NULL, NULL,
                           NULL,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFileDialog:modal:
   *
   * Whether the file chooser dialog is modal.
   *
   * Since: 4.10
   */
  properties[PROP_MODAL] =
      g_param_spec_boolean ("modal", NULL, NULL,
                            TRUE,
                            G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFileDialog:filters:
   *
   * The list of filters.
   *
   * See [property@Gtk.FileDialog:default-filter] about how these
   * two properties interact.
   *
   * Since: 4.10
   */
  properties[PROP_FILTERS] =
      g_param_spec_object ("filters", NULL, NULL,
                           G_TYPE_LIST_MODEL,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFileDialog:default-filter:
   *
   * The default filter.
   *
   * This filter is initially active in the file chooser dialog.
   *
   * If the default filter is `NULL`, the first filter of [property@Gtk.FileDialog:filters]
   * is used as the default filter. If that property contains no filter, the dialog will
   * be unfiltered.
   *
   * If [property@Gtk.FileDialog:filters] is not `NULL`, the default filter should be
   * part of the list. If it is not, the dialog may choose to not make it available.
   *
   * Since: 4.10
   */
  properties[PROP_DEFAULT_FILTER] =
      g_param_spec_object ("default-filter", NULL, NULL,
                           GTK_TYPE_FILE_FILTER,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFileDialog:initial-file:
   *
   * The initial file.
   *
   * This file is initially selected in the file chooser dialog
   *
   * This is a utility property that sets both [property@Gtk.FileDialog:initial-folder]
   * and [property@Gtk.FileDialog:initial-name].
   *
   * Since: 4.10
   */
  properties[PROP_INITIAL_FILE] =
      g_param_spec_object ("initial-file", NULL, NULL,
                           G_TYPE_FILE,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFileDialog:initial-folder:
   *
   * The initial folder.
   *
   * This is the directory that is initially opened in the file chooser dialog.
   *
   * Since: 4.10
   */
  properties[PROP_INITIAL_FOLDER] =
      g_param_spec_object ("initial-folder", NULL, NULL,
                           G_TYPE_FILE,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFileDialog:initial-name:
   *
   * The initial name.
   *
   * This is the name of the file that is initially selected in the file chooser dialog.
   *
   * Since: 4.10
   */
  properties[PROP_INITIAL_NAME] =
      g_param_spec_string ("initial-name", NULL, NULL,
                           NULL,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkFileDialog:accept-label:
   *
   * Label for the file chooser's accept button.
   *
   * Since: 4.10
   */
  properties[PROP_ACCEPT_LABEL] =
      g_param_spec_string ("accept-label", NULL, NULL,
                           NULL,
                           G_PARAM_READWRITE|G_PARAM_STATIC_STRINGS|G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, NUM_PROPERTIES, properties);
}

/* }}} */
/* {{{ Utilities */

static void
file_chooser_set_filters (GtkFileChooser *chooser,
                          GListModel     *filters)
{
  if (!filters)
    return;

  for (unsigned int i = 0; i < g_list_model_get_n_items (filters); i++)
    {
      GtkFileFilter *filter = g_list_model_get_item (filters, i);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
      gtk_file_chooser_add_filter (chooser, filter);
G_GNUC_END_IGNORE_DEPRECATIONS
      g_object_unref (filter);
    }
}

/* }}} */
/* {{{ API: Constructor */

/**
 * gtk_file_dialog_new:
 *
 * Creates a new `GtkFileDialog` object.
 *
 * Returns: the new `GtkFileDialog`
 *
 * Since: 4.10
 */
GtkFileDialog *
gtk_file_dialog_new (void)
{
  return g_object_new (GTK_TYPE_FILE_DIALOG, NULL);
}

/* }}} */
/* {{{ API: Getters and setters */

/**
 * gtk_file_dialog_get_title:
 * @this: a file dialog
 *
 * Returns the title that will be shown on the file chooser dialog.
 *
 * Returns: the title
 *
 * Since: 4.10
 */
const char *
gtk_file_dialog_get_title (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);

  return this->title;
}

/**
 * gtk_file_dialog_set_title:
 * @this: a file dialog
 * @title: the new title
 *
 * Sets the title that will be shown on the file chooser dialog.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_title (GtkFileDialog *this,
                           const char    *title)
{
  char *new_title;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));
  g_return_if_fail (title != NULL);

  if (g_strcmp0 (this->title, title) == 0)
    return;

  new_title = g_strdup (title);
  g_free (this->title);
  this->title = new_title;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_TITLE]);
}

/**
 * gtk_file_dialog_get_modal:
 * @this: a file dialog
 *
 * Returns whether the file chooser dialog blocks interaction
 * with the parent window while it is presented.
 *
 * Returns: true if the file chooser dialog is modal
 *
 * Since: 4.10
 */
gboolean
gtk_file_dialog_get_modal (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), TRUE);

  return this->modal;
}

/**
 * gtk_file_dialog_set_modal:
 * @this: a file dialog
 * @modal: the new value
 *
 * Sets whether the file chooser dialog blocks interaction
 * with the parent window while it is presented.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_modal (GtkFileDialog *this,
                           gboolean       modal)
{
  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  if (this->modal == modal)
    return;

  this->modal = modal;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODAL]);
}

/**
 * gtk_file_dialog_get_filters:
 * @this: a file dialog
 *
 * Gets the filters that will be offered to the user
 * in the file chooser dialog.
 *
 * Returns: (transfer none) (nullable): the filters,
 *   as a list model of [class@Gtk.FileFilter]
 *
 * Since: 4.10
 */
GListModel *
gtk_file_dialog_get_filters (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);

  return this->filters;
}

/**
 * gtk_file_dialog_set_filters:
 * @this: a file dialog
 * @filters: (nullable): a list model of [class@Gtk.FileFilter]
 *
 * Sets the filters that will be offered to the user
 * in the file chooser dialog.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_filters (GtkFileDialog *this,
                             GListModel    *filters)
{
  g_return_if_fail (GTK_IS_FILE_DIALOG (this));
  g_return_if_fail (filters == NULL || G_IS_LIST_MODEL (filters));

  if (!g_set_object (&this->filters, filters))
    return;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_FILTERS]);
}

/**
 * gtk_file_dialog_get_default_filter:
 * @this: a file dialog
 *
 * Gets the filter that will be selected by default
 * in the file chooser dialog.
 *
 * Returns: (transfer none) (nullable): the default filter
 *
 * Since: 4.10
 */
GtkFileFilter *
gtk_file_dialog_get_default_filter (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);

  return this->default_filter;
}

/**
 * gtk_file_dialog_set_default_filter:
 * @this: a file dialog
 * @filter: (nullable): the file filter
 *
 * Sets the filter that will be selected by default
 * in the file chooser dialog.
 *
 * If set to `NULL`, the first item in [property@Gtk.FileDialog:filters]
 * will be used as the default filter. If that list is empty, the dialog
 * will be unfiltered.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_default_filter (GtkFileDialog *this,
                                    GtkFileFilter *filter)
{
  g_return_if_fail (GTK_IS_FILE_DIALOG (this));
  g_return_if_fail (filter == NULL || GTK_IS_FILE_FILTER (filter));

  if (!g_set_object (&this->default_filter, filter))
    return;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_DEFAULT_FILTER]);
}

/**
 * gtk_file_dialog_get_initial_folder:
 * @this: a file dialog
 *
 * Gets the folder that will be set as the
 * initial folder in the file chooser dialog.
 *
 * Returns: (nullable) (transfer none): the folder
 *
 * Since: 4.10
 */
GFile *
gtk_file_dialog_get_initial_folder (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);

  return this->initial_folder;
}

/**
 * gtk_file_dialog_set_initial_folder:
 * @this: a file dialog
 * @folder: (nullable): a file
 *
 * Sets the folder that will be set as the
 * initial folder in the file chooser dialog.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_initial_folder (GtkFileDialog *this,
                                    GFile         *folder)
{
  g_return_if_fail (GTK_IS_FILE_DIALOG (this));
  g_return_if_fail (folder == NULL || G_IS_FILE (folder));

  if (!g_set_object (&this->initial_folder, folder))
    return;

  if (this->initial_name && this->initial_folder)
    {
      g_clear_object (&this->initial_file);
      this->initial_file = g_file_get_child_for_display_name (this->initial_folder,
                                                              this->initial_name,
                                                              NULL);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_FILE]);
    }
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_FOLDER]);
}

/**
 * gtk_file_dialog_get_initial_name:
 * @this: a file dialog
 *
 * Gets the filename that will be initially selected.
 *
 * Returns: (nullable) (transfer none): the name
 *
 * Since: 4.10
 */
const char *
gtk_file_dialog_get_initial_name (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);

  return this->initial_name;
}

/**
 * gtk_file_dialog_set_initial_name:
 * @this: a file dialog
 * @name: (nullable): a string
 *
 * Sets the filename that will be initially selected.
 *
 * For save dialogs, @name will usually be pre-entered into the
 * name field.
 *
 * If a file with this name already exists in the directory set
 * via [property@Gtk.FileDialog:initial-folder], the dialog will
 * preselect it.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_initial_name (GtkFileDialog *this,
                                  const char    *name)
{
  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  if (!g_set_str (&this->initial_name, name))
    return;

  if (this->initial_name && this->initial_folder)
    {
      g_clear_object (&this->initial_file);
      this->initial_file = g_file_get_child_for_display_name (this->initial_folder,
                                                              this->initial_name,
                                                              NULL);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_FILE]);
    }
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_NAME]);
}

/**
 * gtk_file_dialog_get_initial_file:
 * @this: a file dialog
 *
 * Gets the file that will be initially selected in
 * the file chooser dialog.
 *
 * Returns: (nullable) (transfer none): the file
 *
 * Since: 4.10
 */
GFile *
gtk_file_dialog_get_initial_file (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);

  return this->initial_file;
}

/**
 * gtk_file_dialog_set_initial_file:
 * @this: a file dialog
 * @file: (nullable): a file
 *
 * Sets the file that will be initially selected in
 * the file chooser dialog.
 *
 * This function is a shortcut for calling both
 * [method@Gtk.FileDialog.set_initial_folder] and
 * [method@Gtk.FileDialog.set_initial_name] with the
 * directory and name of @file, respectively.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_initial_file (GtkFileDialog *this,
                                  GFile         *file)
{
  g_return_if_fail (GTK_IS_FILE_DIALOG (this));
  g_return_if_fail (file == NULL || G_IS_FILE (file));

  g_object_freeze_notify (G_OBJECT (this));

  if (file != NULL)
    {
      GFile *folder;
      GFileInfo *info;

      if (this->initial_file && g_file_equal (this->initial_file, file))
        return;

      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_FILE]);

      folder = g_file_get_parent (file);
      if (folder == NULL)
        goto invalid_file;

      if (g_set_object (&this->initial_folder, folder))
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_FOLDER]);

      info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_EDIT_NAME, 0, NULL, NULL);
      if (info && g_file_info_get_edit_name (info) != NULL)
        {
          if (g_set_str (&this->initial_name, g_file_info_get_edit_name (info)))
            g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_NAME]);
        }
      else
        {
          char *relative, *name;

          relative = g_file_get_relative_path (folder, file);
          name = g_filename_display_name (relative);
          if (g_set_str (&this->initial_name, name))
            g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_NAME]);

          g_free (name);
          g_free (relative);
        }
      g_clear_object (&info);
      g_object_unref (folder);
    }
  else
    {
invalid_file:
      if (g_set_object (&this->initial_file, NULL))
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_FILE]);
      if (g_set_object (&this->initial_folder, NULL))
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_FOLDER]);
      if (g_set_str (&this->initial_name, NULL))
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INITIAL_NAME]);
    }

  g_object_thaw_notify (G_OBJECT (this));
}

/**
 * gtk_file_dialog_get_accept_label:
 * @this: a file dialog
 *
 * Retrieves the text used by the dialog on its accept button.
 *
 * Returns: (nullable): the label shown on the file chooser's accept button
 *
 * Since: 4.10
 */
const char *
gtk_file_dialog_get_accept_label (GtkFileDialog *this)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);

  return this->accept_label;
}

/**
 * gtk_file_dialog_set_accept_label:
 * @this: a file dialog
 * @accept_label: (nullable): the new accept label
 *
 * Sets the label shown on the file chooser's accept button.
 *
 * Leaving the accept label unset or setting it as `NULL` will
 * fall back to a default label, depending on what API is used
 * to launch the file dialog.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_set_accept_label (GtkFileDialog *this,
                                  const char    *accept_label)
{
  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  if (g_set_str (&this->accept_label, accept_label))
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ACCEPT_LABEL]);
}

/* }}} */
/* {{{ Async implementation */

static void response_cb (GTask *task,
                         int    response);

static void
cancelled_cb (GCancellable *cancellable,
              GTask        *task)
{
  response_cb (task, GTK_RESPONSE_CLOSE);
}

typedef struct {
  GListModel *files;
  char **choices;
} TaskResult;

static void
task_result_free (gpointer data)
{
  TaskResult *res = data;

  g_object_unref (res->files);
  g_strfreev (res->choices);
  g_free (res);
}

static void
response_cb (GTask *task,
             int    response)
{
  GCancellable *cancellable;

  cancellable = g_task_get_cancellable (task);

  if (cancellable)
    g_signal_handlers_disconnect_by_func (cancellable, cancelled_cb, task);

  if (response == GTK_RESPONSE_ACCEPT)
    {
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
      GtkFileChooser *chooser;
      TaskResult *res;
      char **choices;

      res = g_new0 (TaskResult, 1);

      chooser = GTK_FILE_CHOOSER (g_task_get_task_data (task));
      res->files = gtk_file_chooser_get_files (chooser);

      choices = (char **) g_object_get_data (G_OBJECT (chooser), "choices");
      if (choices)
        {
          res->choices = g_new0 (char *, g_strv_length (choices) + 1);
          for (guint i = 0; choices[i]; i++)
            res->choices[i] = g_strdup (gtk_file_chooser_get_choice (chooser, choices[i]));
        }

      g_task_return_pointer (task, res, task_result_free);
G_GNUC_END_IGNORE_DEPRECATIONS
    }
  else if (response == GTK_RESPONSE_CLOSE)
    g_task_return_new_error (task, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED, "Cancelled by application");
  else if (response == GTK_RESPONSE_CANCEL ||
           response == GTK_RESPONSE_DELETE_EVENT)
    g_task_return_new_error (task, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED, "Dismissed by user");
  else
    g_task_return_new_error (task, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_FAILED, "Unknown failure (%d)", response);

  gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (g_task_get_task_data (task)));

  g_object_unref (task);
}

static void
dialog_response (GtkDialog *dialog,
                 int        response,
                 GTask     *task)
{
  response_cb (task, response);
}

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
static GtkFileChooserNative *
create_file_chooser (GtkFileDialog        *this,
                     GtkWindow            *parent,
                     GtkFileChooserAction  action,
                     gboolean              select_multiple)
{
  GtkFileChooserNative *chooser;
  const char *default_accept_label, *accept;
  const char *default_title, *title;

  switch (action)
    {
    case GTK_FILE_CHOOSER_ACTION_OPEN:
      default_accept_label = _("_Open");
      default_title = select_multiple ? _("Pick Files") : _("Pick a File");
      break;

    case GTK_FILE_CHOOSER_ACTION_SAVE:
      default_accept_label = _("_Save");
      default_title = _("Save a File");
      break;

    case GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER:
      default_accept_label = _("_Select");
      default_title = select_multiple ? _("Select Folders") : _("Select a Folder");
      break;

    default:
      g_assert_not_reached ();
    }

  if (this->title)
    title = this->title;
  else
    title = default_title;

  if (this->accept_label)
    accept = this->accept_label;
  else
    accept = default_accept_label;

  chooser = gtk_file_chooser_native_new (title, parent, action, accept, _("_Cancel"));

  gtk_native_dialog_set_modal (GTK_NATIVE_DIALOG (chooser), this->modal);
  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (chooser), select_multiple);

  file_chooser_set_filters (GTK_FILE_CHOOSER (chooser), this->filters);
  if (this->default_filter)
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (chooser), this->default_filter);
  else if (this->filters)
    {
      GtkFileFilter *filter = g_list_model_get_item (this->filters, 0);
      if (filter)
        {
          gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (chooser), filter);
          g_object_unref (filter);
        }
    }

  if (this->initial_folder)
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooser), this->initial_folder, NULL);
  if (this->initial_name && action == GTK_FILE_CHOOSER_ACTION_SAVE)
    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (chooser), this->initial_name);

  return chooser;
}
G_GNUC_END_IGNORE_DEPRECATIONS

static GFile *
finish_file_op (GtkFileDialog   *this,
                GTask           *task,
                char          ***choices,
                GError         **error)
{
  TaskResult *res;

  res = g_task_propagate_pointer (task, error);
  if (res)
    {
      GFile *file = NULL;

      if (g_list_model_get_n_items (res->files) > 0)
        file = g_list_model_get_item (res->files, 0);
      else
        g_set_error_literal (error,
                             GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_FAILED,
                             "No file selected");

      if (choices)
        *choices = g_strdupv (res->choices);

      task_result_free (res);

      return file;
    }

  return NULL;
}

static GListModel *
finish_multiple_files_op (GtkFileDialog   *this,
                          GTask           *task,
                          char          ***choices,
                          GError         **error)
{
  TaskResult *res;

  res = g_task_propagate_pointer (task, error);

  if (res)
    {
      GListModel *files;

      files = G_LIST_MODEL (g_object_ref (res->files));

      if (choices)
        *choices = g_strdupv (res->choices);

      task_result_free (res);

      return files;
    }

  return NULL;
}

/* }}} */
/* {{{ Public API */

/**
 * gtk_file_dialog_open:
 * @this: a file dialog
 * @parent: (nullable): the parent window
 * @cancellable: (nullable): a cancellable to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Presents a file chooser dialog to the user.
 *
 * The file chooser dialog will be set up to select a single file.
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_open (GtkFileDialog       *this,
                      GtkWindow           *parent,
                      GCancellable        *cancellable,
                      GAsyncReadyCallback  callback,
                      gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_OPEN, FALSE);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_open);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_open_finish:
 * @this: a file dialog
 * @result: the result
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.open] call.
 *
 * Returns: (nullable) (transfer full): the file that was selected
 *
 * Since: 4.10
 */
GFile *
gtk_file_dialog_open_finish (GtkFileDialog   *this,
                             GAsyncResult    *result,
                             GError         **error)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_open, NULL);

  return finish_file_op (this, G_TASK (result), NULL, error);
}

/**
 * gtk_file_dialog_select_folder:
 * @this: a file dialog
 * @parent: (nullable): the parent window
 * @cancellable: (nullable): a cancellable to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Presents a file chooser dialog to the user.
 *
 * The file chooser dialog will be set up to select a single folder.
 *
 * If you pass @initial_folder, the file chooser dialog will initially
 * be opened in the parent directory of that folder, otherwise, it
 * will be in the directory [property@Gtk.FileDialog:initial-folder].
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_select_folder (GtkFileDialog       *this,
                               GtkWindow           *parent,
                               GCancellable        *cancellable,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, FALSE);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_select_folder);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_select_folder_finish:
 * @this: a file dialog
 * @result: the result
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.select_folder] call.
 *
 * Returns: (nullable) (transfer full): the folder that was selected
 *
 * Since: 4.10
 */
GFile *
gtk_file_dialog_select_folder_finish (GtkFileDialog  *this,
                                      GAsyncResult   *result,
                                      GError        **error)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_select_folder, NULL);

  return finish_file_op (this, G_TASK (result), NULL, error);
}

/**
 * gtk_file_dialog_save:
 * @this: a file dialog
 * @parent: (nullable): the parent window
 * @cancellable: (nullable): a cancellable to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Presents a file chooser dialog to the user.
 *
 * The file chooser dialog will be save mode.
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_save (GtkFileDialog       *this,
                      GtkWindow           *parent,
                      GCancellable        *cancellable,
                      GAsyncReadyCallback  callback,
                      gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_SAVE, FALSE);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_save);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_save_finish:
 * @this: a file dialog
 * @result: the result
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.save] call.
 *
 * Returns: (nullable) (transfer full): the file that was selected
 *
 * Since: 4.10
 */
GFile *
gtk_file_dialog_save_finish (GtkFileDialog   *this,
                             GAsyncResult    *result,
                             GError         **error)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_save, NULL);

  return finish_file_op (this, G_TASK (result), NULL, error);
}

/**
 * gtk_file_dialog_open_multiple:
 * @this: a file dialog
 * @parent: (nullable): the parent window
 * @cancellable: (nullable): a cancellable to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Presents a file chooser dialog to the user.
 *
 * The file chooser dialog will be set up to select multiple files.
 *
 * The file chooser dialog will initially be opened in the directory
 * [property@Gtk.FileDialog:initial-folder].
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_open_multiple (GtkFileDialog       *this,
                               GtkWindow           *parent,
                               GCancellable        *cancellable,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_OPEN, TRUE);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_open_multiple);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_open_multiple_finish:
 * @this: a file dialog
 * @result: the result
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.open] call.
 *
 * Returns: (nullable) (transfer full): the files that were selected,
 *   as a list model of [iface@Gio.File]
 *
 * Since: 4.10
 */
GListModel *
gtk_file_dialog_open_multiple_finish (GtkFileDialog   *this,
                                      GAsyncResult    *result,
                                      GError         **error)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_open_multiple, NULL);

  return finish_multiple_files_op (this, G_TASK (result), NULL, error);
}

/**
 * gtk_file_dialog_select_multiple_folders:
 * @this: a file dialog
 * @parent: (nullable): the parent window
 * @cancellable: (nullable): a cancellable to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Presents a file chooser dialog to the user.
 *
 * The file chooser dialog will be set up to allow selecting
 * multiple folders.
 *
 * The file chooser dialog will initially be opened in the
 * directory [property@Gtk.FileDialog:initial-folder].
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.10
 */
void
gtk_file_dialog_select_multiple_folders (GtkFileDialog       *this,
                                         GtkWindow           *parent,
                                         GCancellable        *cancellable,
                                         GAsyncReadyCallback  callback,
                                         gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, TRUE);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_select_multiple_folders);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_select_multiple_folders_finish:
 * @this: a file dialog
 * @result: the result
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.select_multiple_folders] call.
 *
 * Returns: (nullable) (transfer full): the folders that were selected,
 *   as a list model of [iface@Gio.File]
 *
 * Since: 4.10
 */
GListModel *
gtk_file_dialog_select_multiple_folders_finish (GtkFileDialog   *this,
                                                GAsyncResult    *result,
                                                GError         **error)
{
  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_select_multiple_folders, NULL);

  return finish_multiple_files_op (this, G_TASK (result), NULL, error);
}

/* }}} */
/* {{{ Text file variants */

/**
 * gtk_file_dialog_open_text_file:
 * @this: a `GtkFileDialog`
 * @parent: (nullable): the parent `GtkWindow`
 * @cancellable: (nullable): a `GCancellable` to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Initiates a file selection operation by presenting a file chooser
 * dialog to the user.
 *
 * In contrast to [method@Gtk.FileDialog.open], this function
 * lets the user select the text encoding for the file, if possible.
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.18
 */
void
gtk_file_dialog_open_text_file (GtkFileDialog       *this,
                                GtkWindow           *parent,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;
  char **names;
  char **labels;
  const char **choices;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_OPEN, FALSE);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  names = gtk_text_encoding_get_names ();
  labels = gtk_text_encoding_get_labels ();
  gtk_file_chooser_add_choice (GTK_FILE_CHOOSER (chooser),
                               "encoding", _("Encoding"),
                               (const char **) names,
                               (const char **) labels);
  gtk_file_chooser_set_choice (GTK_FILE_CHOOSER (chooser),
                               "encoding", "automatic");
  g_free (names);
  g_free (labels);
G_GNUC_END_IGNORE_DEPRECATIONS

  choices = g_new0 (const char *, 2);
  choices[0] = "encoding";
  g_object_set_data_full (G_OBJECT (chooser), "choices", choices, g_free);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_open_text_file);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_open_text_file_finish:
 * @this: a `GtkFileDialog`
 * @result: a `GAsyncResult`
 * @encoding: (out) (transfer none): return location for the text encoding to use
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.open_text_file] call
 * and returns the resulting file and text encoding.
 *
 * If the user has explicitly selected a text encoding to use
 * for the file, then @encoding will be set to a codeset name that
 * is suitable for passing to iconv_open(). Otherwise, it will
 * be `NULL`.
 *
 * Returns: (nullable) (transfer full): the file that was selected
 *
 * Since: 4.18
 */
GFile *
gtk_file_dialog_open_text_file_finish (GtkFileDialog  *this,
                                       GAsyncResult   *result,
                                       const char    **encoding,
                                       GError        **error)
{
  char **choices = NULL;
  GFile *file;

  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_open_text_file, NULL);

  file = finish_file_op (this, G_TASK (result), &choices, error);

  if (choices)
    {
      *encoding = gtk_text_encoding_from_name (choices[0]);
      g_strfreev (choices);
    }
  else
    {
      *encoding = NULL;
    }

  return file;
}

/**
 * gtk_file_dialog_open_multiple_text_files:
 * @this: a file dialog
 * @parent: (nullable): the parent window
 * @cancellable: (nullable): a cancellable to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Presents a file chooser dialog to the user.
 *
 * The file chooser dialog will be set up to select multiple files.
 *
 * The file chooser dialog will initially be opened in the directory
 * [property@Gtk.FileDialog:initial-folder].
 *
 * In contrast to [method@Gtk.FileDialog.open], this function
 * lets the user select the text encoding for the files, if possible.
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.18
 */
void
gtk_file_dialog_open_multiple_text_files (GtkFileDialog       *this,
                                          GtkWindow           *parent,
                                          GCancellable        *cancellable,
                                          GAsyncReadyCallback  callback,
                                          gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;
  char **names;
  char **labels;
  const char **choices;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_OPEN, TRUE);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  names = gtk_text_encoding_get_names ();
  labels = gtk_text_encoding_get_labels ();
  gtk_file_chooser_add_choice (GTK_FILE_CHOOSER (chooser),
                               "encoding", _("Encoding"),
                               (const char **) names,
                               (const char **) labels);
  gtk_file_chooser_set_choice (GTK_FILE_CHOOSER (chooser),
                               "encoding", "automatic");
  g_free (names);
  g_free (labels);
G_GNUC_END_IGNORE_DEPRECATIONS

  choices = g_new0 (const char *, 2);
  choices[0] = "encoding";
  g_object_set_data_full (G_OBJECT (chooser), "choices", choices, g_free);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_open_multiple_text_files);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_open_multiple_text_files_finish:
 * @this: a file dialog
 * @result: the result
 * @encoding: (out) (transfer none): return location for the text encoding to use
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.open] call.
 *
 * Returns: (nullable) (transfer full): the files that were selected,
 *   as a list model of [iface@Gio.File]
 *
 * Since: 4.18
 */
GListModel *
gtk_file_dialog_open_multiple_text_files_finish (GtkFileDialog   *this,
                                                 GAsyncResult    *result,
                                                 const char     **encoding,
                                                 GError         **error)
{
  char **choices = NULL;
  GListModel *files;

  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_open_multiple_text_files, NULL);

  files = finish_multiple_files_op (this, G_TASK (result), &choices, error);

  if (choices)
    {
      *encoding = gtk_text_encoding_from_name (choices[0]);
      g_strfreev (choices);
    }
  else
    {
      *encoding = NULL;
    }

  return files;
}

/**
 * gtk_file_dialog_save_text_file:
 * @this: a `GtkFileDialog`
 * @parent: (nullable): the parent `GtkWindow`
 * @cancellable: (nullable): a `GCancellable` to cancel the operation
 * @callback: (scope async) (closure user_data): a callback to call when the
 *   operation is complete
 * @user_data: data to pass to @callback
 *
 * Initiates a file save operation by presenting a file chooser
 * dialog to the user.
 *
 * In contrast to [method@Gtk.FileDialog.save], this function
 * lets the user select the text encoding and line endings for
 * the text file, if possible.
 *
 * The @callback will be called when the dialog is dismissed.
 *
 * Since: 4.18
 */
void
gtk_file_dialog_save_text_file (GtkFileDialog       *this,
                                GtkWindow           *parent,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  GtkFileChooserNative *chooser;
  GTask *task;
  char **names;
  char **labels;
  const char **choices;

  g_return_if_fail (GTK_IS_FILE_DIALOG (this));

  chooser = create_file_chooser (this, parent, GTK_FILE_CHOOSER_ACTION_SAVE, FALSE);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  names = gtk_text_encoding_get_names ();
  labels = gtk_text_encoding_get_labels ();
  gtk_file_chooser_add_choice (GTK_FILE_CHOOSER (chooser),
                               "encoding", _("Encoding"),
                               (const char **) names,
                               (const char **) labels);
  gtk_file_chooser_set_choice (GTK_FILE_CHOOSER (chooser),
                               "encoding", "automatic");
  g_free (names);
  g_free (labels);

  names = gtk_line_ending_get_names ();
  labels = gtk_line_ending_get_labels ();
  gtk_file_chooser_add_choice (GTK_FILE_CHOOSER (chooser),
                               "line_ending", _("Line Ending"),
                               (const char **) names,
                               (const char **) labels);
  gtk_file_chooser_set_choice (GTK_FILE_CHOOSER (chooser),
                               "line_ending", "as-is");

  g_free (names);
  g_free (labels);

G_GNUC_END_IGNORE_DEPRECATIONS

  choices = g_new0 (const char *, 3);
  choices[0] = "encoding";
  choices[1] = "line_ending";
  g_object_set_data_full (G_OBJECT (chooser), "choices", choices, g_free);

  task = g_task_new (this, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, gtk_file_dialog_save_text_file);
  g_task_set_task_data (task, chooser, g_object_unref);

  if (cancellable)
    g_signal_connect (cancellable, "cancelled", G_CALLBACK (cancelled_cb), task);

  g_signal_connect (chooser, "response", G_CALLBACK (dialog_response), task);

  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

/**
 * gtk_file_dialog_save_text_file_finish:
 * @this: a `GtkFileDialog`
 * @result: a `GAsyncResult`
 * @encoding: (out) (transfer none): return location for the text encoding to use
 * @line_ending: (out) (transfer none): return location for the line endings to use
 * @error: return location for a [enum@Gtk.DialogError] error
 *
 * Finishes the [method@Gtk.FileDialog.save_text_file] call
 * and returns the resulting file, text encoding and line endings.
 *
 * If the user has explicitly selected a text encoding to use
 * for the file, then @encoding will be set to a codeset name that
 * is suitable for passing to iconv_open(). Otherwise, it will
 * be `NULL`.
 *
 * The @line_ending will be set to one of "\n", "\r\n", "\r" or "",
 * where the latter means to preserve existing line endings.
 *
 * Returns: (nullable) (transfer full): the file that was selected.
 *   Otherwise, `NULL` is returned and @error is set
 *
 * Since: 4.18
 */
GFile *
gtk_file_dialog_save_text_file_finish (GtkFileDialog  *this,
                                       GAsyncResult   *result,
                                       const char    **encoding,
                                       const char    **line_ending,
                                       GError        **error)
{
  char **choices = NULL;
  GFile *file;

  g_return_val_if_fail (GTK_IS_FILE_DIALOG (this), NULL);
  g_return_val_if_fail (g_task_is_valid (result, this), NULL);
  g_return_val_if_fail (g_task_get_source_tag (G_TASK (result)) == gtk_file_dialog_save_text_file, NULL);

  file = finish_file_op (this, G_TASK (result), &choices, error);

  if (choices)
    {
      *encoding = gtk_text_encoding_from_name (choices[0]);
      *line_ending = gtk_line_ending_from_name (choices[1]);
      g_strfreev (choices);
    }
  else
    {
      *encoding = NULL;
      *line_ending = "\n";
    }

  return file;
}

/* }}} */

/* vim:set foldmethod=marker: */
