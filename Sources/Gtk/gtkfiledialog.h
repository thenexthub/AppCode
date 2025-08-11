/* GTK - The GIMP Toolkit
 *
 * Copyright (C) 2022 Red Hat, Inc.
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

#pragma once

#if !defined (__GTK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gtk/gtk.h> can be included directly."
#endif

#include <gdk/gdk.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkfilefilter.h>

G_BEGIN_DECLS

#define GTK_TYPE_FILE_DIALOG (gtk_file_dialog_get_type ())

GDK_AVAILABLE_IN_4_10
G_DECLARE_FINAL_TYPE (GtkFileDialog, gtk_file_dialog, GTK, FILE_DIALOG, GObject)

GDK_AVAILABLE_IN_4_10
GtkFileDialog * gtk_file_dialog_new                  (void);

GDK_AVAILABLE_IN_4_10
const char *    gtk_file_dialog_get_title            (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void            gtk_file_dialog_set_title            (GtkFileDialog        *this,
                                                      const char           *title);

GDK_AVAILABLE_IN_4_10
gboolean        gtk_file_dialog_get_modal            (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void            gtk_file_dialog_set_modal            (GtkFileDialog        *this,
                                                      gboolean              modal);

GDK_AVAILABLE_IN_4_10
GListModel *     gtk_file_dialog_get_filters         (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_set_filters         (GtkFileDialog        *this,
                                                      GListModel           *filters);

GDK_AVAILABLE_IN_4_10
GtkFileFilter *  gtk_file_dialog_get_default_filter  (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_set_default_filter  (GtkFileDialog        *this,
                                                      GtkFileFilter        *filter);

GDK_AVAILABLE_IN_4_10
GFile *          gtk_file_dialog_get_initial_folder  (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_set_initial_folder  (GtkFileDialog        *this,
                                                      GFile                *folder);

GDK_AVAILABLE_IN_4_10
const char *     gtk_file_dialog_get_initial_name    (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_set_initial_name    (GtkFileDialog        *this,
                                                      const char           *name);

GDK_AVAILABLE_IN_4_10
GFile *          gtk_file_dialog_get_initial_file    (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_set_initial_file    (GtkFileDialog        *this,
                                                      GFile                *file);

GDK_AVAILABLE_IN_4_10
const char *    gtk_file_dialog_get_accept_label     (GtkFileDialog        *this);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_set_accept_label    (GtkFileDialog        *this,
                                                      const char           *accept_label);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_open                (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_10
GFile *          gtk_file_dialog_open_finish         (GtkFileDialog        *this,
                                                      GAsyncResult         *result,
                                                      GError              **error);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_select_folder       (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_10
GFile *          gtk_file_dialog_select_folder_finish
                                                     (GtkFileDialog        *this,
                                                      GAsyncResult         *result,
                                                      GError              **error);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_save                (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_10
GFile *          gtk_file_dialog_save_finish         (GtkFileDialog        *this,
                                                      GAsyncResult         *result,
                                                      GError               **error);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_open_multiple       (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_10
GListModel *     gtk_file_dialog_open_multiple_finish
                                                     (GtkFileDialog        *this,
                                                      GAsyncResult         *result,
                                                      GError              **error);

GDK_AVAILABLE_IN_4_10
void             gtk_file_dialog_select_multiple_folders
                                                     (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_10
GListModel *     gtk_file_dialog_select_multiple_folders_finish
                                                     (GtkFileDialog        *this,
                                                      GAsyncResult         *result,
                                                      GError              **error);

GDK_AVAILABLE_IN_4_18
void             gtk_file_dialog_open_text_file      (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_18
GFile *          gtk_file_dialog_open_text_file_finish (GtkFileDialog        *this,
                                                        GAsyncResult         *result,
                                                        const char          **encoding,
                                                        GError              **error);

GDK_AVAILABLE_IN_4_18
void             gtk_file_dialog_open_multiple_text_files
                                                     (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_18
GListModel *     gtk_file_dialog_open_multiple_text_files_finish
                                                     (GtkFileDialog        *this,
                                                      GAsyncResult         *result,
                                                      const char          **encoding,
                                                      GError              **error);


GDK_AVAILABLE_IN_4_18
void             gtk_file_dialog_save_text_file      (GtkFileDialog        *this,
                                                      GtkWindow            *parent,
                                                      GCancellable         *cancellable,
                                                      GAsyncReadyCallback   callback,
                                                      gpointer              user_data);

GDK_AVAILABLE_IN_4_18
GFile *          gtk_file_dialog_save_text_file_finish (GtkFileDialog        *this,
                                                        GAsyncResult         *result,
                                                        const char          **encoding,
                                                        const char          **line_ending,
                                                        GError              **error);

G_END_DECLS
