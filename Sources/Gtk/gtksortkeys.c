/*
 * Copyright © 2020 Benjamin Otte
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
 */

#include "config.h"

#include "gtksortkeysprivate.h"

#include "gtkcssstyleprivate.h"
#include "gtkstyleproviderprivate.h"

GtkSortKeys *
gtk_sort_keys_alloc (const GtkSortKeysClass *klass,
                     gsize                   size,
                     gsize                   key_size,
                     gsize                   key_align)
{
  GtkSortKeys *this;

  g_return_val_if_fail (key_align > 0, NULL);

  this = g_malloc0 (size);

  this->klass = klass;
  this->ref_count = 1;

  this->key_size = key_size;
  this->key_align = key_align;

  return this;
}

GtkSortKeys *
gtk_sort_keys_ref (GtkSortKeys *this)
{
  this->ref_count += 1;

  return this;
}

void
gtk_sort_keys_unref (GtkSortKeys *this)
{
  this->ref_count -= 1;
  if (this->ref_count > 0)
    return;

  this->klass->free (this);
}

gsize
gtk_sort_keys_get_key_size (GtkSortKeys *this)
{
  return this->key_size;
}

gsize
gtk_sort_keys_get_key_align (GtkSortKeys *this)
{
  return this->key_align;
}

GCompareDataFunc
gtk_sort_keys_get_key_compare_func (GtkSortKeys *this)
{
  return this->klass->key_compare;
}

gboolean
gtk_sort_keys_is_compatible (GtkSortKeys *this,
                             GtkSortKeys *other)
{
  if (this == other)
    return TRUE;

  return this->klass->is_compatible (this, other);
}

gboolean
gtk_sort_keys_needs_clear_key (GtkSortKeys *this)
{
  return this->klass->clear_key != NULL;
}

static void
gtk_equal_sort_keys_free (GtkSortKeys *keys)
{
  g_free (keys);
}

static int
gtk_equal_sort_keys_compare (gconstpointer a,
                             gconstpointer b,
                             gpointer      unused)
{
  return GTK_ORDERING_EQUAL;
}

static gboolean
gtk_equal_sort_keys_is_compatible (GtkSortKeys *keys,
                                   GtkSortKeys *other)
{
  return keys->klass == other->klass;
}

static void
gtk_equal_sort_keys_init_key (GtkSortKeys *keys,
                               gpointer     item,
                               gpointer     key_memory)
{
}

static const GtkSortKeysClass GTK_EQUAL_SORT_KEYS_CLASS =
{
  gtk_equal_sort_keys_free,
  gtk_equal_sort_keys_compare,
  gtk_equal_sort_keys_is_compatible,
  gtk_equal_sort_keys_init_key,
  NULL
};

/*<private>
 * gtk_sort_keys_new_equal:
 *
 * Creates a new GtkSortKeys that compares every element as equal.
 * This is useful when sorters are in an invalid configuration.
 *
 * Returns: a new GtkSortKeys
 **/
GtkSortKeys *
gtk_sort_keys_new_equal (void)
{
  return gtk_sort_keys_new (GtkSortKeys,
                            &GTK_EQUAL_SORT_KEYS_CLASS,
                            0, 1);
}

