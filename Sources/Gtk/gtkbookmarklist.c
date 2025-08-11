/*
 * Copyright © 2020 Red Hat, Inc.
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
 * Authors: Matthias Clasen <mclasen@redhat.com>
 */

#include "config.h"

#include "gtkbookmarklist.h"

#include "gtksettings.h"
#include "gtkprivate.h"

/**
 * GtkBookmarkList:
 *
 * A list model that wraps `GBookmarkFile`.
 *
 * It presents a `GListModel` and fills it asynchronously with the
 * `GFileInfo`s returned from that function.
 *
 * The `GFileInfo`s in the list have some attributes in the recent
 * namespace added: `recent::private` (boolean) and `recent:applications`
 * (stringv).
 */

enum {
  PROP_0,
  PROP_FILENAME,
  PROP_ATTRIBUTES,
  PROP_IO_PRIORITY,
  PROP_ITEM_TYPE,
  PROP_LOADING,
  PROP_N_ITEMS,

  NUM_PROPERTIES
};

struct _GtkBookmarkList
{
  GObject parent_instance;

  char *attributes;
  char *filename;
  int io_priority;
  int loading;

  GCancellable *cancellable;
  GFileMonitor *monitor;
  GBookmarkFile *file;

  GSequence *items;
};

struct _GtkBookmarkListClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static GType
gtk_bookmark_list_get_item_type (GListModel *list)
{
  return G_TYPE_FILE_INFO;
}

static guint
gtk_bookmark_list_get_n_items (GListModel *list)
{
  GtkBookmarkList *this = GTK_BOOKMARK_LIST (list);

  return g_sequence_get_length (this->items);
}

static gpointer
gtk_bookmark_list_get_item (GListModel *list,
                                guint       position)
{
  GtkBookmarkList *this = GTK_BOOKMARK_LIST (list);
  GSequenceIter *iter;

  iter = g_sequence_get_iter_at_pos (this->items, position);

  if (g_sequence_iter_is_end (iter))
    return NULL;
  else
    return g_object_ref (g_sequence_get (iter));
}

static void
gtk_bookmark_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_bookmark_list_get_item_type;
  iface->get_n_items = gtk_bookmark_list_get_n_items;
  iface->get_item = gtk_bookmark_list_get_item;
}

static void     gtk_bookmark_list_start_loading (GtkBookmarkList *this);
static gboolean gtk_bookmark_list_stop_loading  (GtkBookmarkList *this);
static void     bookmark_file_changed (GFileMonitor       *monitor,
                                       GFile              *file,
                                       GFile              *other_file,
                                       GFileMonitorEvent   event,
                                       gpointer            data);
static void gtk_bookmark_list_set_filename (GtkBookmarkList *this,
                                                const char         *filename);

G_DEFINE_TYPE_WITH_CODE (GtkBookmarkList, gtk_bookmark_list, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_bookmark_list_model_init))

static void
gtk_bookmark_list_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GtkBookmarkList *this = GTK_BOOKMARK_LIST (object);

  switch (prop_id)
    {
    case PROP_ATTRIBUTES:
      gtk_bookmark_list_set_attributes (this, g_value_get_string (value));
      break;

    case PROP_IO_PRIORITY:
      gtk_bookmark_list_set_io_priority (this, g_value_get_int (value));
      break;

    case PROP_FILENAME:
      gtk_bookmark_list_set_filename (this, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_bookmark_list_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtkBookmarkList *this = GTK_BOOKMARK_LIST (object);

  switch (prop_id)
    {
    case PROP_ATTRIBUTES:
      g_value_set_string (value, this->attributes);
      break;

    case PROP_FILENAME:
      g_value_set_string (value, this->filename);
      break;

    case PROP_IO_PRIORITY:
      g_value_set_int (value, this->io_priority);
      break;

    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, G_TYPE_FILE_INFO);
      break;

    case PROP_LOADING:
      g_value_set_boolean (value, gtk_bookmark_list_is_loading (this));
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, g_sequence_get_length (this->items));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_bookmark_list_dispose (GObject *object)
{
  GtkBookmarkList *this = GTK_BOOKMARK_LIST (object);

  gtk_bookmark_list_stop_loading (this);

  g_clear_pointer (&this->attributes, g_free);
  g_clear_pointer (&this->filename, g_free);
  g_clear_pointer (&this->items, g_sequence_free);
  g_clear_pointer (&this->file, g_bookmark_file_free);

  g_signal_handlers_disconnect_by_func (this->monitor, G_CALLBACK (bookmark_file_changed), this);
  g_clear_object (&this->monitor);

  G_OBJECT_CLASS (gtk_bookmark_list_parent_class)->dispose (object);
}

static void
gtk_bookmark_list_class_init (GtkBookmarkListClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->set_property = gtk_bookmark_list_set_property;
  gobject_class->get_property = gtk_bookmark_list_get_property;
  gobject_class->dispose = gtk_bookmark_list_dispose;

  /**
   * GtkBookmarkList:filename:
   *
   * The bookmark file to load.
   */
  properties[PROP_FILENAME] =
      g_param_spec_string ("filename", NULL, NULL,
                           NULL,
                           GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_EXPLICIT_NOTIFY);
  /**
   * GtkBookmarkList:attributes:
   *
   * The attributes to query.
   */
  properties[PROP_ATTRIBUTES] =
      g_param_spec_string ("attributes", NULL, NULL,
                           NULL,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkBookmarkList:io-priority:
   *
   * Priority used when loading.
   */
  properties[PROP_IO_PRIORITY] =
      g_param_spec_int ("io-priority", NULL, NULL,
                        -G_MAXINT, G_MAXINT, G_PRIORITY_DEFAULT,
                        GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkBookmarkList:item-type:
   *
   * The type of items. See [method@Gio.ListModel.get_item_type].
   *
   * Since: 4.8
   **/
  properties[PROP_ITEM_TYPE] =
    g_param_spec_gtype ("item-type", NULL, NULL,
                        G_TYPE_FILE_INFO,
                        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkBookmarkList:loading: (getter is_loading)
   *
   * %TRUE if files are being loaded.
   */
  properties[PROP_LOADING] =
      g_param_spec_boolean ("loading", NULL, NULL,
                            FALSE,
                            GTK_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkBookmarkList:n-items:
   *
   * The number of items. See [method@Gio.ListModel.get_n_items].
   *
   * Since: 4.8
   **/
  properties[PROP_N_ITEMS] =
    g_param_spec_uint ("n-items", NULL, NULL,
                       0, G_MAXUINT, 0,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, NUM_PROPERTIES, properties);
}

static void
gtk_bookmark_list_init (GtkBookmarkList *this)
{
  this->items = g_sequence_new (g_object_unref);
  this->io_priority = G_PRIORITY_DEFAULT;
  this->file = g_bookmark_file_new ();
}

static gboolean
gtk_bookmark_list_stop_loading (GtkBookmarkList *this)
{
  if (this->cancellable == NULL)
    return FALSE;

  g_cancellable_cancel (this->cancellable);
  g_clear_object (&this->cancellable);

  this->loading = 0;

  return TRUE;
}

static void
got_file_info (GObject      *source,
               GAsyncResult *res,
               gpointer      user_data)
{
  GtkBookmarkList *this = user_data;
  GFile *file = G_FILE (source);
  GFileInfo *info;
  GError *error = NULL;

  info = g_file_query_info_finish (file, res, &error);
  if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    {
      g_error_free (error);
      return;
    }

  if (info)
    {
      char *uri;
      gboolean is_private;
      char **apps;

      uri = g_file_get_uri (file);
      is_private = g_bookmark_file_get_is_private (this->file, uri, NULL);
      apps = g_bookmark_file_get_applications (this->file, uri, NULL, NULL);

      g_file_info_set_attribute_object (info, "standard::file", G_OBJECT (file));
      g_file_info_set_attribute_boolean (info, "recent::private", is_private);
      g_file_info_set_attribute_stringv (info, "recent::applications", apps);

      g_strfreev (apps);

      g_sequence_append (this->items, info);
      g_list_model_items_changed (G_LIST_MODEL (this), g_sequence_get_length (this->items) - 1, 0, 1);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);

      g_free (uri);
    }

  this->loading--;

  if (this->loading == 0)
    {
      g_clear_object (&this->cancellable);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
    }
}

static void
gtk_bookmark_list_clear_items (GtkBookmarkList *this)
{
  guint n_items;

  n_items = g_sequence_get_length (this->items);
  if (n_items > 0)
    {
      g_sequence_remove_range (g_sequence_get_begin_iter (this->items),
                               g_sequence_get_end_iter (this->items));

      g_list_model_items_changed (G_LIST_MODEL (this), 0, n_items, 0);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }
}

static void
gtk_bookmark_list_start_loading (GtkBookmarkList *this)
{
  gboolean was_loading;
  GError *error = NULL;

  was_loading = gtk_bookmark_list_stop_loading (this);
  gtk_bookmark_list_clear_items (this);

  if (g_bookmark_file_load_from_file (this->file, this->filename, &error))
    {
      char **uris;
      gsize len;
      int i;

      uris = g_bookmark_file_get_uris (this->file, &len);
      if (len > 0)
        {
          this->cancellable = g_cancellable_new ();
          this->loading = len;
        }

      for (i = 0; i < len; i++)
        {
          const char *uri = uris[i];
          GFile *file;

          /* add this item */
          file = g_file_new_for_uri (uri);
          g_file_query_info_async (file,
                                   this->attributes,
                                   0,
                                   this->io_priority,
                                   this->cancellable,
                                   got_file_info,
                                   this);
          g_object_unref (file);
        }

      g_strfreev (uris);
    }
  else
    {
      if (!g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
        g_warning ("Failed to load %s: %s", this->filename, error->message);
      g_clear_error (&error);
    }

  if (was_loading != (this->cancellable != NULL))
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
}

static void
bookmark_file_changed (GFileMonitor      *monitor,
                       GFile             *file,
                       GFile             *other_file,
                       GFileMonitorEvent  event_type,
                       gpointer           data)
{
  GtkBookmarkList *this = data;

  switch (event_type)
    {
    case G_FILE_MONITOR_EVENT_CHANGED:
    case G_FILE_MONITOR_EVENT_CREATED:
    case G_FILE_MONITOR_EVENT_DELETED:
      gtk_bookmark_list_start_loading (this);
      break;

    case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
    case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
    case G_FILE_MONITOR_EVENT_UNMOUNTED:
    case G_FILE_MONITOR_EVENT_MOVED:
    case G_FILE_MONITOR_EVENT_RENAMED:
    case G_FILE_MONITOR_EVENT_MOVED_IN:
    case G_FILE_MONITOR_EVENT_MOVED_OUT:

    default:
      break;
    }
}

static void
gtk_bookmark_list_set_filename (GtkBookmarkList *this,
                                    const char         *filename)
{
  GFile *file;

  if (filename)
    this->filename = g_strdup (filename);
  else
    this->filename = g_build_filename (g_get_user_data_dir (), "recently-used.xbel", NULL);

  file = g_file_new_for_path (this->filename);
  this->monitor = g_file_monitor_file (file, G_FILE_MONITOR_NONE, NULL, NULL);
  g_signal_connect (this->monitor, "changed",
                    G_CALLBACK (bookmark_file_changed), this);
  g_object_unref (file);

  gtk_bookmark_list_start_loading (this);
}

/**
 * gtk_bookmark_list_get_filename:
 * @this: a `GtkBookmarkList`
 *
 * Returns the filename of the bookmark file that
 * this list is loading.
 *
 * Returns: (type filename): the filename of the .xbel file
 */
const char *
gtk_bookmark_list_get_filename (GtkBookmarkList *this)
{
  g_return_val_if_fail (GTK_IS_BOOKMARK_LIST (this), NULL);

  return this->filename;
}

/**
 * gtk_bookmark_list_new:
 * @filename: (type filename) (nullable): The bookmark file to load
 * @attributes: (nullable): The attributes to query
 *
 * Creates a new `GtkBookmarkList` with the given @attributes.
 *
 * Returns: a new `GtkBookmarkList`
 */
GtkBookmarkList *
gtk_bookmark_list_new (const char *filename,
                       const char *attributes)
{
  return g_object_new (GTK_TYPE_BOOKMARK_LIST,
                       "filename", filename,
                       "attributes", attributes,
                       NULL);
}

/**
 * gtk_bookmark_list_set_attributes:
 * @this: a `GtkBookmarkList`
 * @attributes: (nullable): the attributes to enumerate
 *
 * Sets the @attributes to be enumerated and starts the enumeration.
 *
 * If @attributes is %NULL, no attributes will be queried, but a list
 * of `GFileInfo`s will still be created.
 */
void
gtk_bookmark_list_set_attributes (GtkBookmarkList *this,
                                      const char         *attributes)
{
  g_return_if_fail (GTK_IS_BOOKMARK_LIST (this));

  if (g_strcmp0 (this->attributes, attributes) == 0)
    return;

  g_object_freeze_notify (G_OBJECT (this));

  g_free (this->attributes);
  this->attributes = g_strdup (attributes);

  gtk_bookmark_list_start_loading (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ATTRIBUTES]);

  g_object_thaw_notify (G_OBJECT (this));
}

/**
 * gtk_bookmark_list_get_attributes:
 * @this: a `GtkBookmarkList`
 *
 * Gets the attributes queried on the children.
 *
 * Returns: (nullable) (transfer none): The queried attributes
 */
const char *
gtk_bookmark_list_get_attributes (GtkBookmarkList *this)
{
  g_return_val_if_fail (GTK_IS_BOOKMARK_LIST (this), NULL);

  return this->attributes;
}

/**
 * gtk_bookmark_list_set_io_priority:
 * @this: a `GtkBookmarkList`
 * @io_priority: IO priority to use
 *
 * Sets the IO priority to use while loading files.
 *
 * The default IO priority is %G_PRIORITY_DEFAULT.
 */
void
gtk_bookmark_list_set_io_priority (GtkBookmarkList *this,
                                       int                 io_priority)
{
  g_return_if_fail (GTK_IS_BOOKMARK_LIST (this));

  if (this->io_priority == io_priority)
    return;

  this->io_priority = io_priority;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_IO_PRIORITY]);
}

/**
 * gtk_bookmark_list_get_io_priority:
 * @this: a `GtkBookmarkList`
 *
 * Gets the IO priority to use while loading file.
 *
 * Returns: The IO priority.
 */
int
gtk_bookmark_list_get_io_priority (GtkBookmarkList *this)
{
  g_return_val_if_fail (GTK_IS_BOOKMARK_LIST (this), G_PRIORITY_DEFAULT);

  return this->io_priority;
}

/**
 * gtk_bookmark_list_is_loading: (get-property loading)
 * @this: a `GtkBookmarkList`
 *
 * Returns %TRUE if the files are currently being loaded.
 *
 * Files will be added to @this from time to time while loading is
 * going on. The order in which are added is undefined and may change
 * in between runs.
 *
 * Returns: %TRUE if @this is loading
 */
gboolean
gtk_bookmark_list_is_loading (GtkBookmarkList *this)
{
  g_return_val_if_fail (GTK_IS_BOOKMARK_LIST (this), FALSE);

  return this->cancellable != NULL;
}
