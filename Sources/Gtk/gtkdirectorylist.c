/*
 * Copyright © 2019 Benjamin Otte
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

#include "gtkdirectorylist.h"

#include "gtkprivate.h"

/**
 * GtkDirectoryList:
 *
 * A list model that wraps [method@Gio.File.enumerate_children_async].
 *
 * It presents a `GListModel` and fills it asynchronously with the `GFileInfo`s
 * returned from that function.
 *
 * Enumeration will start automatically when the
 * [property@Gtk.DirectoryList:file] property is set.
 *
 * While the `GtkDirectoryList` is being filled, the
 * [property@Gtk.DirectoryList:loading] property will be set to %TRUE. You can
 * listen to that property if you want to show information like a `GtkSpinner`
 * or a "Loading..." text.
 *
 * If loading fails at any point, the [property@Gtk.DirectoryList:error]
 * property will be set to give more indication about the failure.
 *
 * The `GFileInfo`s returned from a `GtkDirectoryList` have the "standard::file"
 * attribute set to the `GFile` they refer to. This way you can get at the file
 * that is referred to in the same way you would via g_file_enumerator_get_child().
 * This means you do not need access to the `GtkDirectoryList`, but can access
 * the `GFile` directly from the `GFileInfo` when operating with a `GtkListView`
 * or similar.
 */

/* random number that everyone else seems to use, too */
#define FILES_PER_QUERY 100

enum {
  PROP_0,
  PROP_ATTRIBUTES,
  PROP_ERROR,
  PROP_FILE,
  PROP_IO_PRIORITY,
  PROP_ITEM_TYPE,
  PROP_LOADING,
  PROP_MONITORED,
  PROP_N_ITEMS,

  NUM_PROPERTIES
};

typedef struct _QueuedEvent QueuedEvent;
struct _QueuedEvent
{
  GtkDirectoryList *list;
  GFile *file;
  GFileInfo *info;
  GFileMonitorEvent event;
};

static void
free_queued_event (gpointer data)
{
  QueuedEvent *event = data;

  g_clear_object (&event->file);
  g_clear_object (&event->info);
  g_free (event);
}

struct _GtkDirectoryList
{
  GObject parent_instance;

  char *attributes;
  GFile *file;
  GFileMonitor *monitor;
  gboolean monitored;
  int io_priority;

  GCancellable *cancellable;
  GError *error; /* Error while loading */
  GSequence *items; /* Use GPtrArray or GListStore here? */
  GQueue events;
};

struct _GtkDirectoryListClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static GType
gtk_directory_list_get_item_type (GListModel *list)
{
  return G_TYPE_FILE_INFO;
}

static guint
gtk_directory_list_get_n_items (GListModel *list)
{
  GtkDirectoryList *this = GTK_DIRECTORY_LIST (list);

  return g_sequence_get_length (this->items);
}

static gpointer
gtk_directory_list_get_item (GListModel *list,
                             guint       position)
{
  GtkDirectoryList *this = GTK_DIRECTORY_LIST (list);
  GSequenceIter *iter;

  iter = g_sequence_get_iter_at_pos (this->items, position);

  if (g_sequence_iter_is_end (iter))
    return NULL;
  else
    return g_object_ref (g_sequence_get (iter));
}

static void
gtk_directory_list_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_directory_list_get_item_type;
  iface->get_n_items = gtk_directory_list_get_n_items;
  iface->get_item = gtk_directory_list_get_item;
}

G_DEFINE_TYPE_WITH_CODE (GtkDirectoryList, gtk_directory_list, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_directory_list_model_init))

static void
gtk_directory_list_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GtkDirectoryList *this = GTK_DIRECTORY_LIST (object);

  switch (prop_id)
    {
    case PROP_ATTRIBUTES:
      gtk_directory_list_set_attributes (this, g_value_get_string (value));
      break;
    case PROP_FILE:
      gtk_directory_list_set_file (this, g_value_get_object (value));
      break;

    case PROP_IO_PRIORITY:
      gtk_directory_list_set_io_priority (this, g_value_get_int (value));
      break;

    case PROP_MONITORED:
      gtk_directory_list_set_monitored (this, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_directory_list_get_property (GObject     *object,
                                 guint        prop_id,
                                 GValue      *value,
                                 GParamSpec  *pspec)
{
  GtkDirectoryList *this = GTK_DIRECTORY_LIST (object);

  switch (prop_id)
    {
    case PROP_ATTRIBUTES:
      g_value_set_string (value, this->attributes);
      break;

    case PROP_ERROR:
      g_value_set_boxed (value, this->error);
      break;

    case PROP_FILE:
      g_value_set_object (value, this->file);
      break;

    case PROP_IO_PRIORITY:
      g_value_set_int (value, this->io_priority);
      break;

    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, G_TYPE_FILE_INFO);
      break;

    case PROP_LOADING:
      g_value_set_boolean (value, gtk_directory_list_is_loading (this));
      break;

    case PROP_MONITORED:
      g_value_set_boolean (value, gtk_directory_list_get_monitored (this));
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, g_sequence_get_length (this->items));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static gboolean
gtk_directory_list_stop_loading (GtkDirectoryList *this)
{
  if (this->cancellable == NULL)
    return FALSE;

  g_cancellable_cancel (this->cancellable);
  g_clear_object (&this->cancellable);
  return TRUE;
}

static void directory_changed (GFileMonitor       *monitor,
                               GFile              *file,
                               GFile              *other_file,
                               GFileMonitorEvent   event,
                               gpointer            data);

static void
gtk_directory_list_stop_monitoring (GtkDirectoryList *this)
{
  if (this->monitor)
    g_signal_handlers_disconnect_by_func (this->monitor, directory_changed, this);
  g_clear_object (&this->monitor);
}

static void
gtk_directory_list_dispose (GObject *object)
{
  GtkDirectoryList *this = GTK_DIRECTORY_LIST (object);

  gtk_directory_list_stop_loading (this);
  gtk_directory_list_stop_monitoring (this);

  g_clear_object (&this->file);
  g_clear_pointer (&this->attributes, g_free);

  g_clear_error (&this->error);
  g_clear_pointer (&this->items, g_sequence_free);

  g_queue_foreach (&this->events, (GFunc) free_queued_event, NULL);
  g_queue_clear (&this->events);

  G_OBJECT_CLASS (gtk_directory_list_parent_class)->dispose (object);
}

static void
gtk_directory_list_class_init (GtkDirectoryListClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->set_property = gtk_directory_list_set_property;
  gobject_class->get_property = gtk_directory_list_get_property;
  gobject_class->dispose = gtk_directory_list_dispose;

  /**
   * GtkDirectoryList:attributes:
   *
   * The attributes to query.
   */
  properties[PROP_ATTRIBUTES] =
      g_param_spec_string ("attributes", NULL, NULL,
                           NULL,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkDirectoryList:error:
   *
   * Error encountered while loading files.
   */
  properties[PROP_ERROR] =
      g_param_spec_boxed ("error", NULL, NULL,
                          G_TYPE_ERROR,
                          GTK_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkDirectoryList:file:
   *
   * File to query.
   */
  properties[PROP_FILE] =
      g_param_spec_object ("file", NULL, NULL,
                           G_TYPE_FILE,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkDirectoryList:io-priority:
   *
   * Priority used when loading.
   */
  properties[PROP_IO_PRIORITY] =
      g_param_spec_int ("io-priority", NULL, NULL,
                        -G_MAXINT, G_MAXINT, G_PRIORITY_DEFAULT,
                        GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkDirectoryList:item-type:
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
   * GtkDirectoryList:loading: (getter is_loading)
   *
   * %TRUE if files are being loaded.
   */
  properties[PROP_LOADING] =
      g_param_spec_boolean ("loading", NULL, NULL,
                            FALSE,
                            GTK_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkDirectoryList:monitored:
   *
   * %TRUE if the directory is monitored for changed.
   */
  properties[PROP_MONITORED] =
      g_param_spec_boolean ("monitored", NULL, NULL,
                            TRUE,
                            GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkDirectoryList:n-items:
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
gtk_directory_list_init (GtkDirectoryList *this)
{
  this->items = g_sequence_new (g_object_unref);
  this->io_priority = G_PRIORITY_DEFAULT;
  this->monitored = TRUE;
  g_queue_init (&this->events);
}

/**
 * gtk_directory_list_new:
 * @file: (nullable): The file to query
 * @attributes: (nullable): The attributes to query with
 *
 * Creates a new `GtkDirectoryList`.
 *
 * The `GtkDirectoryList` is querying the given @file
 * with the given @attributes.
 *
 * Returns: a new `GtkDirectoryList`
 **/
GtkDirectoryList *
gtk_directory_list_new (const char *attributes,
                        GFile      *file)
{
  g_return_val_if_fail (file == NULL || G_IS_FILE (file), NULL);

  return g_object_new (GTK_TYPE_DIRECTORY_LIST,
                       "attributes", attributes,
                       "file", file,
                       NULL);
}

static void
gtk_directory_list_clear_items (GtkDirectoryList *this)
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

  if (this->error)
    {
      g_clear_error (&this->error);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ERROR]);
    }
}

static void
gtk_directory_list_enumerator_closed_cb (GObject      *source,
                                         GAsyncResult *res,
                                         gpointer      user_data)
{
  g_file_enumerator_close_finish (G_FILE_ENUMERATOR (source), res, NULL);
}

static void
gtk_directory_list_got_files_cb (GObject      *source,
                                 GAsyncResult *res,
                                 gpointer      user_data)
{
  GtkDirectoryList *this = user_data; /* invalid if cancelled */
  GFileEnumerator *enumerator = G_FILE_ENUMERATOR (source);
  GError *error = NULL;
  GList *l, *files;
  guint n;

  files = g_file_enumerator_next_files_finish (enumerator, res, &error);

  if (files == NULL)
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
          g_clear_error (&error);
          return;
        }

      g_file_enumerator_close_async (enumerator,
                                     this->io_priority,
                                     NULL,
                                     gtk_directory_list_enumerator_closed_cb,
                                     NULL);

      g_object_freeze_notify (G_OBJECT (this));

      g_clear_object (&this->cancellable);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);

      if (error)
        {
          this->error = error;
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ERROR]);
        }

      g_object_thaw_notify (G_OBJECT (this));
      return;
    }

  n = 0;
  for (l = files; l; l = l->next)
    {
      GFileInfo *info;
      GFile *file;

      info = l->data;
      file = g_file_enumerator_get_child (enumerator, info);
      g_file_info_set_attribute_object (info, "standard::file", G_OBJECT (file));
      g_object_unref (file);
      g_sequence_append (this->items, info);
      n++;
    }
  g_list_free (files);

  g_file_enumerator_next_files_async (enumerator,
                                      g_file_is_native (this->file) ? 50 * FILES_PER_QUERY : FILES_PER_QUERY,
                                      this->io_priority,
                                      this->cancellable,
                                      gtk_directory_list_got_files_cb,
                                      this);

  if (n > 0)
    {
      g_list_model_items_changed (G_LIST_MODEL (this), g_sequence_get_length (this->items) - n, 0, n);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
    }
}

static void
gtk_directory_list_got_enumerator_cb (GObject      *source,
                                      GAsyncResult *res,
                                      gpointer      user_data)
{
  GtkDirectoryList *this = user_data; /* invalid if cancelled */
  GFile *file = G_FILE (source);
  GFileEnumerator *enumerator;
  GError *error = NULL;

  enumerator = g_file_enumerate_children_finish (file, res, &error);
  if (enumerator == NULL)
    {
      if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
          g_clear_error (&error);
          return;
        }

      g_object_freeze_notify (G_OBJECT (this));
      this->error = error;
      g_clear_object (&this->cancellable);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ERROR]);
      g_object_thaw_notify (G_OBJECT (this));
      return;
    }

  g_file_enumerator_next_files_async (enumerator,
                                      g_file_is_native (file) ? 50 * FILES_PER_QUERY : FILES_PER_QUERY,
                                      this->io_priority,
                                      this->cancellable,
                                      gtk_directory_list_got_files_cb,
                                      this);
  g_object_unref (enumerator);
}

static void
gtk_directory_list_start_loading (GtkDirectoryList *this)
{
  gboolean was_loading;
  char *glib_apis_suck;

  was_loading = gtk_directory_list_stop_loading (this);
  gtk_directory_list_clear_items (this);

  if (this->file == NULL)
    {
      if (was_loading)
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
      return;
    }

  glib_apis_suck = g_strconcat ("standard::name,", this->attributes, NULL);
  this->cancellable = g_cancellable_new ();
  g_file_enumerate_children_async (this->file,
                                   glib_apis_suck,
                                   G_FILE_QUERY_INFO_NONE,
                                   this->io_priority,
                                   this->cancellable,
                                   gtk_directory_list_got_enumerator_cb,
                                   this);
  g_free (glib_apis_suck);

  if (!was_loading)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_LOADING]);
}

static GSequenceIter *
find_file (GSequence *sequence,
           GFile     *file)
{
  GSequenceIter *iter;

  for (iter = g_sequence_get_begin_iter (sequence);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    {
      GFileInfo *item = G_FILE_INFO (g_sequence_get (iter));
      GFile *f = G_FILE (g_file_info_get_attribute_object (item, "standard::file"));

      if (g_file_equal (f, file))
        return iter;
    }

  return NULL;
}

static gboolean
handle_event (QueuedEvent *event)
{
  GtkDirectoryList *this = event->list;
  GFile *file = event->file;
  GFileInfo *info = event->info;
  GSequenceIter *iter;
  unsigned int position;

  switch ((int)event->event)
    {
    case G_FILE_MONITOR_EVENT_MOVED_IN:
    case G_FILE_MONITOR_EVENT_CREATED:
      if (!info)
        return FALSE;

      g_file_info_set_attribute_object (info, "standard::file", G_OBJECT (file));

      iter = find_file (this->items, file);
      if (iter)
        {
          position = g_sequence_iter_get_position (iter);
          g_sequence_set (iter, g_object_ref (info));
          g_list_model_items_changed (G_LIST_MODEL (this), position, 1, 1);
        }
      else
        {
          position = g_sequence_get_length (this->items);
          g_sequence_append (this->items, g_object_ref (info));
          g_list_model_items_changed (G_LIST_MODEL (this), position, 0, 1);
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
        }
      break;

    case G_FILE_MONITOR_EVENT_MOVED_OUT:
    case G_FILE_MONITOR_EVENT_DELETED:
      iter = find_file (this->items, file);
      if (iter)
        {
          position = g_sequence_iter_get_position (iter);
          g_sequence_remove (iter);
          g_list_model_items_changed (G_LIST_MODEL (this), position, 1, 0);
          g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
        }
      break;

    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
      if (!info)
        return FALSE;

      g_file_info_set_attribute_object (info, "standard::file", G_OBJECT (file));

      iter = find_file (this->items, file);
      if (iter)
        {
          position = g_sequence_iter_get_position (iter);
          g_sequence_set (iter, g_object_ref (info));
          g_list_model_items_changed (G_LIST_MODEL (this), position, 1, 1);
        }
      break;

    default:
      g_assert_not_reached ();
    }

  return TRUE;
}

static void
handle_events (GtkDirectoryList *this)
{
  QueuedEvent *event;

  do
    {
      event = g_queue_peek_tail (&this->events);
      if (!event)
        return;

      if (!handle_event (event))
        return;

      event = g_queue_pop_tail (&this->events);
      free_queued_event (event);
    }
  while (TRUE);
}

static void
got_new_file_info_cb (GObject      *source,
                      GAsyncResult *res,
                      gpointer      data)
{
  QueuedEvent *event = data;
  GtkDirectoryList *this = event->list;
  GFile *file = event->file;

  event->info = g_file_query_info_finish (file, res, NULL);
  handle_events (this);
}

static void
got_existing_file_info_cb (GObject      *source,
                           GAsyncResult *res,
                           gpointer      data)
{
  QueuedEvent *event = data;
  GtkDirectoryList *this = event->list;
  GFile *file = event->file;

  event->info = g_file_query_info_finish (file, res, NULL);
  handle_events (this);
}

static void
directory_changed (GFileMonitor       *monitor,
                   GFile              *file,
                   GFile              *other_file,
                   GFileMonitorEvent   event,
                   gpointer            data)
{
  GtkDirectoryList *this = GTK_DIRECTORY_LIST (data);
  QueuedEvent *ev;

  switch (event)
    {
    case G_FILE_MONITOR_EVENT_MOVED_IN:
    case G_FILE_MONITOR_EVENT_CREATED:
      ev = g_new0 (QueuedEvent, 1);
      ev->list = this;
      ev->event = event;
      ev->file = g_object_ref (file);
      g_queue_push_head (&this->events, ev);

      g_file_query_info_async (file,
                               this->attributes,
                               G_FILE_QUERY_INFO_NONE,
                               this->io_priority,
                               this->cancellable,
                               got_new_file_info_cb,
                               ev);
      break;

    case G_FILE_MONITOR_EVENT_MOVED_OUT:
    case G_FILE_MONITOR_EVENT_DELETED:
      ev = g_new0 (QueuedEvent, 1);
      ev->list = this;
      ev->event = event;
      ev->file = g_object_ref (file);
      g_queue_push_head (&this->events, ev);

      handle_events (this);
      break;

    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
      ev = g_new0 (QueuedEvent, 1);
      ev->list = this;
      ev->event = event;
      ev->file = g_object_ref (file);
      g_queue_push_head (&this->events, ev);

      g_file_query_info_async (file,
                               this->attributes,
                               G_FILE_QUERY_INFO_NONE,
                               this->io_priority,
                               this->cancellable,
                               got_existing_file_info_cb,
                               ev);
      break;

    case G_FILE_MONITOR_EVENT_RENAMED:
      ev = g_new0 (QueuedEvent, 1);
      ev->list = this;
      ev->event = G_FILE_MONITOR_EVENT_DELETED;
      ev->file = g_object_ref (file);
      g_queue_push_head (&this->events, ev);

      ev = g_new0 (QueuedEvent, 1);
      ev->list = this;
      ev->event = G_FILE_MONITOR_EVENT_CREATED;
      ev->file = g_object_ref (other_file);
      g_queue_push_head (&this->events, ev);

      g_file_query_info_async (other_file,
                               this->attributes,
                               G_FILE_QUERY_INFO_NONE,
                               this->io_priority,
                               this->cancellable,
                               got_existing_file_info_cb,
                               ev);
      break;

    case G_FILE_MONITOR_EVENT_CHANGED:
    case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
    case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
    case G_FILE_MONITOR_EVENT_UNMOUNTED:
    case G_FILE_MONITOR_EVENT_MOVED:
    default:
      break;
    }
}

static void
gtk_directory_list_start_monitoring (GtkDirectoryList *this)
{
  g_assert (this->monitor == NULL);
  this->monitor = g_file_monitor_directory (this->file, G_FILE_MONITOR_WATCH_MOVES, NULL, NULL);
  if (this->monitor)
    g_signal_connect (this->monitor, "changed", G_CALLBACK (directory_changed), this);
}

static void
gtk_directory_list_update_monitoring (GtkDirectoryList *this)
{
  gtk_directory_list_stop_monitoring (this);
  if (this->file && this->monitored)
    gtk_directory_list_start_monitoring (this);
}

/**
 * gtk_directory_list_set_file:
 * @this: a `GtkDirectoryList`
 * @file: (nullable): the `GFile` to be enumerated
 *
 * Sets the @file to be enumerated and starts the enumeration.
 *
 * If @file is %NULL, the result will be an empty list.
 */
void
gtk_directory_list_set_file (GtkDirectoryList *this,
                             GFile            *file)
{
  g_return_if_fail (GTK_IS_DIRECTORY_LIST (this));
  g_return_if_fail (file == NULL || G_IS_FILE (file));

  if (this->file == file ||
      (this->file && file && g_file_equal (this->file, file)))
    return;

  g_object_freeze_notify (G_OBJECT (this));

  g_set_object (&this->file, file);

  gtk_directory_list_update_monitoring (this);
  gtk_directory_list_start_loading (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_FILE]);

  g_object_thaw_notify (G_OBJECT (this));
}

/**
 * gtk_directory_list_get_file:
 * @this: a `GtkDirectoryList`
 *
 * Gets the file whose children are currently enumerated.
 *
 * Returns: (nullable) (transfer none): The file whose children are enumerated
 **/
GFile *
gtk_directory_list_get_file (GtkDirectoryList *this)
{
  g_return_val_if_fail (GTK_IS_DIRECTORY_LIST (this), NULL);

  return this->file;
}

/**
 * gtk_directory_list_set_attributes:
 * @this: a `GtkDirectoryList`
 * @attributes: (nullable): the attributes to enumerate
 *
 * Sets the @attributes to be enumerated and starts the enumeration.
 *
 * If @attributes is %NULL, the list of file infos will still be created, it will just
 * not contain any extra attributes.
 */
void
gtk_directory_list_set_attributes (GtkDirectoryList *this,
                                   const char       *attributes)
{
  g_return_if_fail (GTK_IS_DIRECTORY_LIST (this));

  if (this->attributes == attributes)
    return;

  g_object_freeze_notify (G_OBJECT (this));

  g_free (this->attributes);
  this->attributes = g_strdup (attributes);

  gtk_directory_list_start_loading (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ATTRIBUTES]);

  g_object_thaw_notify (G_OBJECT (this));
}

/**
 * gtk_directory_list_get_attributes:
 * @this: a `GtkDirectoryList`
 *
 * Gets the attributes queried on the children.
 *
 * Returns: (nullable) (transfer none): The queried attributes
 */
const char *
gtk_directory_list_get_attributes (GtkDirectoryList *this)
{
  g_return_val_if_fail (GTK_IS_DIRECTORY_LIST (this), NULL);

  return this->attributes;
}

/**
 * gtk_directory_list_set_io_priority:
 * @this: a `GtkDirectoryList`
 * @io_priority: IO priority to use
 *
 * Sets the IO priority to use while loading directories.
 *
 * Setting the priority while @this is loading will reprioritize the
 * ongoing load as soon as possible.
 *
 * The default IO priority is %G_PRIORITY_DEFAULT, which is higher than
 * the GTK redraw priority. If you are loading a lot of directories in
 * parallel, lowering it to something like %G_PRIORITY_DEFAULT_IDLE
 * may increase responsiveness.
 */
void
gtk_directory_list_set_io_priority (GtkDirectoryList *this,
                                    int               io_priority)
{
  g_return_if_fail (GTK_IS_DIRECTORY_LIST (this));

  if (this->io_priority == io_priority)
    return;

  this->io_priority = io_priority;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_IO_PRIORITY]);
}

/**
 * gtk_directory_list_get_io_priority:
 * @this: a `GtkDirectoryList`
 *
 * Gets the IO priority set via gtk_directory_list_set_io_priority().
 *
 * Returns: The IO priority.
 */
int
gtk_directory_list_get_io_priority (GtkDirectoryList *this)
{
  g_return_val_if_fail (GTK_IS_DIRECTORY_LIST (this), G_PRIORITY_DEFAULT);

  return this->io_priority;
}

/**
 * gtk_directory_list_is_loading: (get-property loading)
 * @this: a `GtkDirectoryList`
 *
 * Returns %TRUE if the children enumeration is currently in
 * progress.
 *
 * Files will be added to @this from time to time while loading is
 * going on. The order in which are added is undefined and may change
 * in between runs.
 *
 * Returns: %TRUE if @this is loading
 */
gboolean
gtk_directory_list_is_loading (GtkDirectoryList *this)
{
  g_return_val_if_fail (GTK_IS_DIRECTORY_LIST (this), FALSE);

  return this->cancellable != NULL;
}

/**
 * gtk_directory_list_get_error:
 * @this: a `GtkDirectoryList`
 *
 * Gets the loading error, if any.
 *
 * If an error occurs during the loading process, the loading process
 * will finish and this property allows querying the error that happened.
 * This error will persist until a file is loaded again.
 *
 * An error being set does not mean that no files were loaded, and all
 * successfully queried files will remain in the list.
 *
 * Returns: (nullable) (transfer none): The loading error or %NULL if
 *   loading finished successfully
 */
const GError *
gtk_directory_list_get_error (GtkDirectoryList *this)
{
  g_return_val_if_fail (GTK_IS_DIRECTORY_LIST (this), FALSE);

  return this->error;
}

/**
 * gtk_directory_list_set_monitored:
 * @this: a `GtkDirectoryList`
 * @monitored: %TRUE to monitor the directory for changes
 *
 * Sets whether the directory list will monitor the directory
 * for changes.
 *
 * If monitoring is enabled, the ::items-changed signal will
 * be emitted when the directory contents change.
 *
 *
 * When monitoring is turned on after the initial creation
 * of the directory list, the directory is reloaded to avoid
 * missing files that appeared between the initial loading
 * and when monitoring was turned on.
 */
void
gtk_directory_list_set_monitored (GtkDirectoryList *this,
                                  gboolean          monitored)
{
  g_return_if_fail (GTK_IS_DIRECTORY_LIST (this));

  if (this->monitored == monitored)
    return;

  this->monitored = monitored;

  gtk_directory_list_update_monitoring (this);
  if (monitored)
    gtk_directory_list_start_loading (this);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MONITORED]);
}

/**
 * gtk_directory_list_get_monitored:
 * @this: a `GtkDirectoryList`
 *
 * Returns whether the directory list is monitoring
 * the directory for changes.
 *
 * Returns: %TRUE if the directory is monitored
 */
gboolean
gtk_directory_list_get_monitored (GtkDirectoryList *this)
{
  g_return_val_if_fail (GTK_IS_DIRECTORY_LIST (this), TRUE);

  return this->monitored;
}
