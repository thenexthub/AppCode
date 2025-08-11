/* gtkpasswordentrybuffer.c: Entry buffer with secure allocation 
 *
   Copyright 2009  Stefan Walter
 * Copyright 2020  GNOME Foundation
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gtkpasswordentrybuffer.h"

#include "gtksecurememoryprivate.h"

#include <string.h>

/* Initial size of buffer, in bytes */
#define MIN_SIZE 16

/**
 * GtkPasswordEntryBuffer:
 *
 * A `GtkEntryBuffer` that locks the underlying memory to prevent it
 * from being swapped to disk.
 *
 * `GtkPasswordEntry` uses a `GtkPasswordEntryBuffer`.
 *
 * Since: 4.4
 */
struct _GtkPasswordEntryBuffer
{
  GtkEntryBuffer parent_instance;

  char *text;
  gsize text_size;
  gsize text_bytes;
  guint text_chars;
};

G_DEFINE_TYPE (GtkPasswordEntryBuffer, gtk_password_entry_buffer, GTK_TYPE_ENTRY_BUFFER)

static const char *
gtk_password_entry_buffer_real_get_text (GtkEntryBuffer *buffer,
                                       gsize *n_bytes)
{
  GtkPasswordEntryBuffer *this = GTK_PASSWORD_ENTRY_BUFFER (buffer);

  if (n_bytes != NULL)
    *n_bytes = this->text_bytes;

  if (!this->text)
    return "";

  return this->text;
}

static guint
gtk_password_entry_buffer_real_get_length (GtkEntryBuffer *buffer)
{
  GtkPasswordEntryBuffer *this = GTK_PASSWORD_ENTRY_BUFFER (buffer);
  return this->text_chars;
}

static guint
gtk_password_entry_buffer_real_insert_text (GtkEntryBuffer *buffer,
                                            guint           position,
                                            const char     *chars,
                                            guint           n_chars)
{
  GtkPasswordEntryBuffer *this = GTK_PASSWORD_ENTRY_BUFFER (buffer);

  gsize n_bytes = g_utf8_offset_to_pointer (chars, n_chars) - chars;

  /* Need more memory */
  if (n_bytes + this->text_bytes + 1 > this->text_size)
    {
      /* Calculate our new buffer size */
      while (n_bytes + this->text_bytes + 1 > this->text_size)
        {
          if (this->text_size == 0)
            {
              this->text_size = MIN_SIZE;
            }
          else
            {
              if (2 * this->text_size < GTK_ENTRY_BUFFER_MAX_SIZE)
                {
                  this->text_size *= 2;
                }
              else
                {
                  this->text_size = GTK_ENTRY_BUFFER_MAX_SIZE;
                  if (n_bytes > this->text_size - this->text_bytes - 1)
                    {
                      n_bytes = this->text_size - this->text_bytes - 1;
                      n_bytes = g_utf8_find_prev_char (chars, chars + n_bytes + 1) - chars;
                      n_chars = g_utf8_strlen (chars, n_bytes);
                    }
                  break;
                }
            }
        }

      this->text = gtk_secure_realloc (this->text, this->text_size);
    }

  /* Actual text insertion */
  gsize at = g_utf8_offset_to_pointer (this->text, position) - this->text;
  memmove (this->text + at + n_bytes, this->text + at, this->text_bytes - at);
  memcpy (this->text + at, chars, n_bytes);

  /* Book keeping */
  this->text_bytes += n_bytes;
  this->text_chars += n_chars;
  this->text[this->text_bytes] = '\0';

  gtk_entry_buffer_emit_inserted_text (buffer, position, chars, n_chars);

  return n_chars;
}

static void
gtk_password_entry_buffer_real_deleted_text (GtkEntryBuffer *buffer,
                                             guint           position,
                                             guint           n_chars)
{
  GtkPasswordEntryBuffer *this = GTK_PASSWORD_ENTRY_BUFFER (buffer);

  gsize start = g_utf8_offset_to_pointer (this->text, position) - this->text;
  gsize end = g_utf8_offset_to_pointer (this->text, position + n_chars) - this->text;

  memmove (this->text + start, this->text + end, this->text_bytes + 1 - end);
  this->text_chars -= n_chars;
  this->text_bytes -= (end - start);

  g_object_notify (G_OBJECT (buffer), "text");
  g_object_notify (G_OBJECT (buffer), "length");
}

static guint
gtk_password_entry_buffer_real_delete_text (GtkEntryBuffer *buffer,
                                            guint           position,
                                            guint           n_chars)
{
  GtkPasswordEntryBuffer *this = GTK_PASSWORD_ENTRY_BUFFER (buffer);

  if (position > this->text_chars)
    position = this->text_chars;
  if (position + n_chars > this->text_chars)
    n_chars = this->text_chars - position;

  if (n_chars > 0)
    gtk_entry_buffer_emit_deleted_text (buffer, position, n_chars);

  return n_chars;
}

static void
gtk_password_entry_buffer_finalize (GObject *gobject)
{
  GtkPasswordEntryBuffer *this = GTK_PASSWORD_ENTRY_BUFFER (gobject);

  g_clear_pointer (&this->text, gtk_secure_free);

  this->text_bytes = 0;
  this->text_size = 0;
  this->text_chars = 0;

  G_OBJECT_CLASS (gtk_password_entry_buffer_parent_class)->finalize (gobject);
}

static void
gtk_password_entry_buffer_class_init (GtkPasswordEntryBufferClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkEntryBufferClass *buffer_class = GTK_ENTRY_BUFFER_CLASS (klass);

  gobject_class->finalize = gtk_password_entry_buffer_finalize;

  buffer_class->get_text = gtk_password_entry_buffer_real_get_text;
  buffer_class->get_length = gtk_password_entry_buffer_real_get_length;
  buffer_class->insert_text = gtk_password_entry_buffer_real_insert_text;
  buffer_class->delete_text = gtk_password_entry_buffer_real_delete_text;
  buffer_class->deleted_text = gtk_password_entry_buffer_real_deleted_text;
}

static void
gtk_password_entry_buffer_init (GtkPasswordEntryBuffer *this)
{
}

/**
 * gtk_password_entry_buffer_new:
 *
 * Creates a new `GtkEntryBuffer` using secure memory allocations.
 *
 * Returns: (transfer full): the newly created instance
 */
GtkEntryBuffer *
gtk_password_entry_buffer_new (void)
{
  return g_object_new (GTK_TYPE_PASSWORD_ENTRY_BUFFER, NULL);
}
