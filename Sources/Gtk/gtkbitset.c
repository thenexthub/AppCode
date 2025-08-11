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
 *
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include "config.h"

#include "gtkbitset.h"

#include "roaring/roaring.c"

/**
 * GtkBitset: (ref-fn gtk_bitset_ref) (unref-fn gtk_bitset_unref)
 *
 * A set of unsigned integers.
 *
 * Another name for this data structure is “bitmap”.
 *
 * The current implementation is based on [roaring bitmaps](https://roaringbitmap.org/).
 *
 * A bitset allows adding a set of integers and provides support for set operations
 * like unions, intersections and checks for equality or if a value is contained
 * in the set. `GtkBitset` also contains various functions to query metadata about
 * the bitset, such as the minimum or maximum values or its size.
 *
 * The fastest way to iterate values in a bitset is [struct@Gtk.BitsetIter].
 *
 * The main use case for `GtkBitset` is implementing complex selections for
 * [iface@Gtk.SelectionModel].
 */

struct _GtkBitset
{
  int ref_count;
  roaring_bitmap_t roaring;
};


G_DEFINE_BOXED_TYPE (GtkBitset, gtk_bitset,
                     gtk_bitset_ref,
                     gtk_bitset_unref)

/**
 * gtk_bitset_ref:
 * @this: (not nullable): a `GtkBitset`
 *
 * Acquires a reference on the given `GtkBitset`.
 *
 * Returns: (transfer none): the `GtkBitset` with an additional reference
 */
GtkBitset *
gtk_bitset_ref (GtkBitset *this)
{
  g_return_val_if_fail (this != NULL, NULL);

  this->ref_count += 1;

  return this;
}

/**
 * gtk_bitset_unref:
 * @this: (not nullable) (transfer full): a `GtkBitset`
 *
 * Releases a reference on the given `GtkBitset`.
 *
 * If the reference was the last, the resources associated to the @this are
 * freed.
 */
void
gtk_bitset_unref (GtkBitset *this)
{
  g_return_if_fail (this != NULL);
  g_return_if_fail (this->ref_count > 0);

  this->ref_count -= 1;
  if (this->ref_count > 0)
    return;

  ra_clear (&this->roaring.high_low_container);
  g_free (this);
}

/**
 * gtk_bitset_contains:
 * @this: a `GtkBitset`
 * @value: the value to check
 *
 * Checks if the given @value has been added to @this
 *
 * Returns: %TRUE if @this contains @value
 **/
gboolean
gtk_bitset_contains (const GtkBitset *this,
                     guint            value)
{
  g_return_val_if_fail (this != NULL, FALSE);

  return roaring_bitmap_contains (&this->roaring, value);
}

/**
 * gtk_bitset_is_empty:
 * @this: a `GtkBitset`
 *
 * Check if no value is contained in bitset.
 *
 * Returns: %TRUE if @this is empty
 **/
gboolean
gtk_bitset_is_empty (const GtkBitset *this)
{
  g_return_val_if_fail (this != NULL, TRUE);

  return roaring_bitmap_is_empty (&this->roaring);
}

/**
 * gtk_bitset_equals:
 * @this: a `GtkBitset`
 * @other: another `GtkBitset`
 *
 * Returns %TRUE if @this and @other contain the same values.
 *
 * Returns: %TRUE if @this and @other contain the same values
 **/
gboolean
gtk_bitset_equals (const GtkBitset *this,
                   const GtkBitset *other)
{
  g_return_val_if_fail (this != NULL, other == NULL);
  g_return_val_if_fail (other != NULL, FALSE);

  if (this == other)
    return TRUE;

  return roaring_bitmap_equals (&this->roaring, &other->roaring);
}

/**
 * gtk_bitset_get_minimum:
 * @this: a `GtkBitset`
 *
 * Returns the smallest value in @this.
 *
 * If @this is empty, `G_MAXUINT` is returned.
 *
 * Returns: The smallest value in @this
 **/
guint
gtk_bitset_get_minimum (const GtkBitset *this)
{
  g_return_val_if_fail (this != NULL, G_MAXUINT);

  return roaring_bitmap_minimum (&this->roaring);
}

/**
 * gtk_bitset_get_maximum:
 * @this: a `GtkBitset`
 *
 * Returns the largest value in @this.
 *
 * If @this is empty, 0 is returned.
 *
 * Returns: The largest value in @this
 **/
guint
gtk_bitset_get_maximum (const GtkBitset *this)
{
  g_return_val_if_fail (this != NULL, 0);

  return roaring_bitmap_maximum (&this->roaring);
}

/**
 * gtk_bitset_get_size:
 * @this: a `GtkBitset`
 *
 * Gets the number of values that were added to the set.
 *
 * For example, if the set is empty, 0 is returned.
 *
 * Note that this function returns a `guint64`, because when all
 * values are set, the return value is `G_MAXUINT + 1`. Unless you
 * are sure this cannot happen (it can't with `GListModel`), be sure
 * to use a 64bit type.
 *
 * Returns: The number of values in the set.
 */
guint64
gtk_bitset_get_size (const GtkBitset *this)
{
  g_return_val_if_fail (this != NULL, 0);

  return roaring_bitmap_get_cardinality (&this->roaring);
}

/**
 * gtk_bitset_get_size_in_range:
 * @this: a `GtkBitset`
 * @first: the first element to include
 * @last: the last element to include
 *
 * Gets the number of values that are part of the set from @first to @last
 * (inclusive).
 *
 * Note that this function returns a `guint64`, because when all values are
 * set, the return value is `G_MAXUINT + 1`. Unless you are sure this cannot
 * happen (it can't with `GListModel`), be sure to use a 64bit type.
 *
 * Returns: The number of values in the set from @first to @last.
 */
guint64
gtk_bitset_get_size_in_range (const GtkBitset *this,
                              guint            first,
                              guint            last)
{
  g_return_val_if_fail (this != NULL, 0);
  g_return_val_if_fail (last >= first, 0);

  return roaring_bitmap_range_cardinality (&this->roaring, first, ((uint64_t) last) + 1);
}

/**
 * gtk_bitset_get_nth:
 * @this: a `GtkBitset`
 * @nth: index of the item to get
 *
 * Returns the value of the @nth item in this.
 *
 * If @nth is >= the size of @this, 0 is returned.
 *
 * Returns: the value of the @nth item in @this
 */
guint
gtk_bitset_get_nth (const GtkBitset *this,
                    guint            nth)
{
  uint32_t result;

  if (!roaring_bitmap_select (&this->roaring, nth, &result))
    return 0;

  return result;
}

/**
 * gtk_bitset_new_empty:
 *
 * Creates a new empty bitset.
 *
 * Returns: A new empty bitset
 */
GtkBitset *
gtk_bitset_new_empty (void)
{
  GtkBitset *this;

  this = g_new0 (GtkBitset, 1);

  this->ref_count = 1;

  ra_init (&this->roaring.high_low_container);

  return this;
}

/**
 * gtk_bitset_new_range:
 * @start: first value to add
 * @n_items: number of consecutive values to add
 *
 * Creates a bitset with the given range set.
 *
 * Returns: A new bitset
 **/
GtkBitset *
gtk_bitset_new_range (guint start,
                      guint n_items)
{
  GtkBitset *this;

  this = gtk_bitset_new_empty ();

  gtk_bitset_add_range (this, start, n_items);

  return this;
}

/**
 * gtk_bitset_copy:
 * @this: a `GtkBitset`
 *
 * Creates a copy of @this.
 *
 * Returns: (transfer full): A new bitset that contains the same
 *   values as @this
 */
GtkBitset *
gtk_bitset_copy (const GtkBitset *this)
{
  GtkBitset *copy;

  g_return_val_if_fail (this != NULL, NULL);

  copy = gtk_bitset_new_empty ();

  if (!gtk_bitset_is_empty (this))
    roaring_bitmap_overwrite (&copy->roaring, &this->roaring);

  return copy;
}

/**
 * gtk_bitset_remove_all:
 * @this: a `GtkBitset`
 *
 * Removes all values from the bitset so that it is empty again.
 */
void
gtk_bitset_remove_all (GtkBitset *this)
{
  g_return_if_fail (this != NULL);

  roaring_bitmap_clear (&this->roaring);
}

/**
 * gtk_bitset_add:
 * @this: a `GtkBitset`
 * @value: value to add
 *
 * Adds @value to @this if it wasn't part of it before.
 *
 * Returns: %TRUE if @value was not part of @this and @this
 *   was changed
 */
gboolean
gtk_bitset_add (GtkBitset *this,
                guint      value)
{
  g_return_val_if_fail (this != NULL, FALSE);

  return roaring_bitmap_add_checked (&this->roaring, value);
}

/**
 * gtk_bitset_remove:
 * @this: a `GtkBitset`
 * @value: value to remove
 *
 * Removes @value from @this if it was part of it before.
 *
 * Returns: %TRUE if @value was part of @this and @this
 *   was changed
 */
gboolean
gtk_bitset_remove (GtkBitset *this,
                   guint      value)
{
  g_return_val_if_fail (this != NULL, FALSE);

  return roaring_bitmap_remove_checked (&this->roaring, value);
}

/**
 * gtk_bitset_add_range:
 * @this: a `GtkBitset`
 * @start: first value to add
 * @n_items: number of consecutive values to add
 *
 * Adds all values from @start (inclusive) to @start + @n_items
 * (exclusive) in @this.
 */
void
gtk_bitset_add_range (GtkBitset *this,
                      guint      start,
                      guint      n_items)
{
  g_return_if_fail (this != NULL);

  if (n_items == 0)
    return;

  /* overflow check, the == 0 is to allow add_range(G_MAXUINT, 1); */
  g_return_if_fail (start + n_items == 0 || start + n_items > start);

  roaring_bitmap_add_range_closed (&this->roaring, start, start + n_items - 1);
}

/**
 * gtk_bitset_remove_range:
 * @this: a `GtkBitset`
 * @start: first value to remove
 * @n_items: number of consecutive values to remove
 *
 * Removes all values from @start (inclusive) to @start + @n_items (exclusive)
 * in @this.
 */
void
gtk_bitset_remove_range (GtkBitset *this,
                         guint      start,
                         guint      n_items)
{
  g_return_if_fail (this != NULL);

  if (n_items == 0)
    return;

  /* overflow check, the == 0 is to allow add_range(G_MAXUINT, 1); */
  g_return_if_fail (start + n_items == 0 || start + n_items > start);

  roaring_bitmap_remove_range_closed (&this->roaring, start, start + n_items - 1);
}

/**
 * gtk_bitset_add_range_closed:
 * @this: a `GtkBitset`
 * @first: first value to add
 * @last: last value to add
 *
 * Adds the closed range [@first, @last], so @first, @last and all
 * values in between. @first must be smaller than @last.
 */
void
gtk_bitset_add_range_closed (GtkBitset *this,
                             guint      first,
                             guint      last)
{
  g_return_if_fail (this != NULL);
  g_return_if_fail (first <= last);

  roaring_bitmap_add_range_closed (&this->roaring, first, last);
}

/**
 * gtk_bitset_remove_range_closed:
 * @this: a `GtkBitset`
 * @first: first value to remove
 * @last: last value to remove
 *
 * Removes the closed range [@first, @last], so @first, @last and all
 * values in between. @first must be smaller than @last.
 */
void
gtk_bitset_remove_range_closed (GtkBitset *this,
                                guint      first,
                                guint      last)
{
  g_return_if_fail (this != NULL);
  g_return_if_fail (first <= last);

  roaring_bitmap_remove_range_closed (&this->roaring, first, last);
}

/**
 * gtk_bitset_add_rectangle:
 * @this: a `GtkBitset`
 * @start: first value to add
 * @width: width of the rectangle
 * @height: height of the rectangle
 * @stride: row stride of the grid
 *
 * Interprets the values as a 2-dimensional boolean grid with the given @stride
 * and inside that grid, adds a rectangle with the given @width and @height.
 */
void
gtk_bitset_add_rectangle (GtkBitset *this,
                          guint      start,
                          guint      width,
                          guint      height,
                          guint      stride)
{
  guint i;

  g_return_if_fail (this != NULL);
  g_return_if_fail ((start % stride) + width <= stride);
  g_return_if_fail (G_MAXUINT - start >= height * stride);

  if (width == 0 || height == 0)
    return;

  for (i = 0; i < height; i++)
    gtk_bitset_add_range (this, i * stride + start, width);
}

/**
 * gtk_bitset_remove_rectangle:
 * @this: a `GtkBitset`
 * @start: first value to remove
 * @width: width of the rectangle
 * @height: height of the rectangle
 * @stride: row stride of the grid
 *
 * Interprets the values as a 2-dimensional boolean grid with the given @stride
 * and inside that grid, removes a rectangle with the given @width and @height.
 */
void
gtk_bitset_remove_rectangle (GtkBitset *this,
                             guint      start,
                             guint      width,
                             guint      height,
                             guint      stride)
{
  guint i;

  g_return_if_fail (this != NULL);
  g_return_if_fail (width <= stride);
  g_return_if_fail (G_MAXUINT - start >= height * stride);

  if (width == 0 || height == 0)
    return;

  for (i = 0; i < height; i++)
    gtk_bitset_remove_range (this, i * stride + start, width);
}

/**
 * gtk_bitset_union:
 * @this: a `GtkBitset`
 * @other: the `GtkBitset` to union with
 *
 * Sets @this to be the union of @this and @other.
 *
 * That is, add all values from @other into @this that weren't part of it.
 *
 * It is allowed for @this and @other to be the same bitset. Nothing will
 * happen in that case.
 */
void
gtk_bitset_union (GtkBitset       *this,
                  const GtkBitset *other)
{
  g_return_if_fail (this != NULL);
  g_return_if_fail (other != NULL);

  if (this == other)
    return;

  roaring_bitmap_or_inplace (&this->roaring, &other->roaring);
}

/**
 * gtk_bitset_intersect:
 * @this: a `GtkBitset`
 * @other: the `GtkBitset` to intersect with
 *
 * Sets @this to be the intersection of @this and @other.
 *
 * In other words, remove all values from @this that are not part of @other.
 *
 * It is allowed for @this and @other to be the same bitset. Nothing will
 * happen in that case.
 */
void
gtk_bitset_intersect (GtkBitset       *this,
                      const GtkBitset *other)
{
  g_return_if_fail (this != NULL);
  g_return_if_fail (other != NULL);

  if (this == other)
    return;

  roaring_bitmap_and_inplace (&this->roaring, &other->roaring);
}

/**
 * gtk_bitset_subtract:
 * @this: a `GtkBitset`
 * @other: the `GtkBitset` to subtract
 *
 * Sets @this to be the subtraction of @other from @this.
 *
 * In other words, remove all values from @this that are part of @other.
 *
 * It is allowed for @this and @other to be the same bitset. The bitset
 * will be emptied in that case.
 */
void
gtk_bitset_subtract (GtkBitset       *this,
                     const GtkBitset *other)
{
  g_return_if_fail (this != NULL);
  g_return_if_fail (other != NULL);

  if (this == other)
    {
      roaring_bitmap_clear (&this->roaring);
      return;
    }

  roaring_bitmap_andnot_inplace (&this->roaring, &other->roaring);
}

/**
 * gtk_bitset_difference:
 * @this: a `GtkBitset`
 * @other: the `GtkBitset` to compute the difference from
 *
 * Sets @this to be the symmetric difference of @this and @other.
 *
 * The symmetric difference is set @this to contain all values that
 * were either contained in @this or in @other, but not in both.
 * This operation is also called an XOR.
 *
 * It is allowed for @this and @other to be the same bitset. The bitset
 * will be emptied in that case.
 */
void
gtk_bitset_difference (GtkBitset       *this,
                       const GtkBitset *other)
{
  g_return_if_fail (this != NULL);
  g_return_if_fail (other != NULL);

  if (this == other)
    {
      roaring_bitmap_clear (&this->roaring);
      return;
    }

  roaring_bitmap_xor_inplace (&this->roaring, &other->roaring);
}

/**
 * gtk_bitset_shift_left:
 * @this: a `GtkBitset`
 * @amount: amount to shift all values to the left
 *
 * Shifts all values in @this to the left by @amount.
 *
 * Values smaller than @amount are discarded.
 */
void
gtk_bitset_shift_left (GtkBitset *this,
                       guint      amount)
{
  roaring_bitmap_t *other;

  g_return_if_fail (this != NULL);

  other = roaring_bitmap_add_offset (&this->roaring, - (int64_t) amount);
  roaring_bitmap_overwrite (&this->roaring, other);
  roaring_bitmap_free (other);
}

/**
 * gtk_bitset_shift_right:
 * @this: a `GtkBitset`
 * @amount: amount to shift all values to the right
 *
 * Shifts all values in @this to the right by @amount.
 *
 * Values that end up too large to be held in a #guint are discarded.
 */
void
gtk_bitset_shift_right (GtkBitset *this,
                        guint      amount)
{
  roaring_bitmap_t *other;

  other = roaring_bitmap_add_offset (&this->roaring, (int64_t) amount);
  roaring_bitmap_overwrite (&this->roaring, other);
  roaring_bitmap_free (other);
}

/**
 * gtk_bitset_splice:
 * @this: a `GtkBitset`
 * @position: position at which to slice
 * @removed: number of values to remove
 * @added: number of values to add
 *
 * This is a support function for `GListModel` handling, by mirroring
 * the `GlistModel::items-changed` signal.
 *
 * First, it "cuts" the values from @position to @removed from
 * the bitset. That is, it removes all those values and shifts
 * all larger values to the left by @removed places.
 *
 * Then, it "pastes" new room into the bitset by shifting all values
 * larger than @position by @added spaces to the right. This frees
 * up space that can then be filled.
 */
void
gtk_bitset_splice (GtkBitset *this,
                   guint      position,
                   guint      removed,
                   guint      added)
{
  g_return_if_fail (this != NULL);
  /* overflow */
  g_return_if_fail (position + removed >= position);
  g_return_if_fail (position + added >= position);

  gtk_bitset_remove_range (this, position, removed);

  if (removed != added)
    {
      GtkBitset *shift = gtk_bitset_copy (this);

      gtk_bitset_remove_range (shift, 0, position);
      gtk_bitset_remove_range_closed (this, position, G_MAXUINT);
      if (added > removed)
        gtk_bitset_shift_right (shift, added - removed);
      else
        gtk_bitset_shift_left (shift, removed - added);
      gtk_bitset_union (this, shift);
      gtk_bitset_unref (shift);
    }
}

G_STATIC_ASSERT (sizeof (GtkBitsetIter) >= sizeof (roaring_uint32_iterator_t));

static GtkBitsetIter *
gtk_bitset_iter_copy (GtkBitsetIter *iter)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  return (GtkBitsetIter *) roaring_uint32_iterator_copy (riter);
}

static void
gtk_bitset_iter_free (GtkBitsetIter *iter)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  roaring_uint32_iterator_free (riter);
}

G_DEFINE_BOXED_TYPE (GtkBitsetIter, gtk_bitset_iter, gtk_bitset_iter_copy, gtk_bitset_iter_free)

/**
 * gtk_bitset_iter_init_first:
 * @iter: (out): a pointer to an uninitialized `GtkBitsetIter`
 * @set: a `GtkBitset`
 * @value: (out) (optional): Set to the first value in @set
 *
 * Initializes an iterator for @set and points it to the first
 * value in @set.
 *
 * If @set is empty, %FALSE is returned and @value is set to %G_MAXUINT.
 *
 * Returns: %TRUE if @set isn't empty.
 */
gboolean
gtk_bitset_iter_init_first (GtkBitsetIter   *iter,
                            const GtkBitset *set,
                            guint           *value)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (set != NULL, FALSE);

  roaring_iterator_init (&set->roaring, riter);

  if (value)
    *value = riter->has_value ? riter->current_value : 0;

  return riter->has_value;
}

/**
 * gtk_bitset_iter_init_last:
 * @iter: (out): a pointer to an uninitialized `GtkBitsetIter`
 * @set: a `GtkBitset`
 * @value: (out) (optional): Set to the last value in @set
 *
 * Initializes an iterator for @set and points it to the last
 * value in @set.
 *
 * If @set is empty, %FALSE is returned.
 *
 * Returns: %TRUE if @set isn't empty.
 **/
gboolean
gtk_bitset_iter_init_last (GtkBitsetIter    *iter,
                            const GtkBitset *set,
                            guint           *value)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (set != NULL, FALSE);

  roaring_iterator_init_last (&set->roaring, riter);

  if (value)
    *value = riter->has_value ? riter->current_value : 0;

  return riter->has_value;
}

/**
 * gtk_bitset_iter_init_at:
 * @iter: (out): a pointer to an uninitialized `GtkBitsetIter`
 * @set: a `GtkBitset`
 * @target: target value to start iterating at
 * @value: (out) (optional): Set to the found value in @set
 *
 * Initializes @iter to point to @target.
 *
 * If @target is not found, finds the next value after it.
 * If no value >= @target exists in @set, this function returns %FALSE.
 *
 * Returns: %TRUE if a value was found.
 */
gboolean
gtk_bitset_iter_init_at (GtkBitsetIter   *iter,
                         const GtkBitset *set,
                         guint            target,
                         guint           *value)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (set != NULL, FALSE);

  roaring_iterator_init (&set->roaring, riter);
  if (!roaring_uint32_iterator_move_equalorlarger (riter, target))
    {
      if (value)
        *value = 0;
      return FALSE;
    }

  if (value)
    *value = riter->current_value;

  return TRUE;
}

/**
 * gtk_bitset_iter_next:
 * @iter: a pointer to a valid `GtkBitsetIter`
 * @value: (out) (optional): Set to the next value
 *
 * Moves @iter to the next value in the set.
 *
 * If it was already pointing to the last value in the set,
 * %FALSE is returned and @iter is invalidated.
 *
 * Returns: %TRUE if a next value existed
 */
gboolean
gtk_bitset_iter_next (GtkBitsetIter *iter,
                      guint         *value)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (riter->has_value, FALSE);

  if (!roaring_uint32_iterator_advance (riter))
    {
      if (value)
        *value = 0;
      return FALSE;
    }

  if (value)
    *value = riter->current_value;

  return TRUE;
}

/**
 * gtk_bitset_iter_previous:
 * @iter: a pointer to a valid `GtkBitsetIter`
 * @value: (out) (optional): Set to the previous value
 *
 * Moves @iter to the previous value in the set.
 *
 * If it was already pointing to the first value in the set,
 * %FALSE is returned and @iter is invalidated.
 *
 * Returns: %TRUE if a previous value existed
 */
gboolean
gtk_bitset_iter_previous (GtkBitsetIter *iter,
                          guint         *value)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (riter->has_value, FALSE);

  if (!roaring_uint32_iterator_previous (riter))
    {
      if (value)
        *value = 0;
      return FALSE;
    }

  if (value)
    *value = riter->current_value;

  return TRUE;
}

/**
 * gtk_bitset_iter_get_value:
 * @iter: a `GtkBitsetIter`
 *
 * Gets the current value that @iter points to.
 *
 * If @iter is not valid and [method@Gtk.BitsetIter.is_valid]
 * returns %FALSE, this function returns 0.
 *
 * Returns: The current value pointer to by @iter
 */
guint
gtk_bitset_iter_get_value (const GtkBitsetIter *iter)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  g_return_val_if_fail (iter != NULL, 0);

  if (!riter->has_value)
    return 0;

  return riter->current_value;
}

/**
 * gtk_bitset_iter_is_valid:
 * @iter: a `GtkBitsetIter`
 *
 * Checks if @iter points to a valid value.
 *
 * Returns: %TRUE if @iter points to a valid value
 */
gboolean
gtk_bitset_iter_is_valid (const GtkBitsetIter *iter)
{
  roaring_uint32_iterator_t *riter = (roaring_uint32_iterator_t *) iter;

  g_return_val_if_fail (iter != NULL, FALSE);

  return riter->has_value;
}
