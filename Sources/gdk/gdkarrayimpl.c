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

#include <glib.h>

G_BEGIN_DECLS

#ifndef GDK_ARRAY_TYPE_NAME
#define GDK_ARRAY_TYPE_NAME GdkArray
#endif

#ifndef GDK_ARRAY_NAME
#define GDK_ARRAY_NAME gdk_array
#endif

#ifndef GDK_ARRAY_ELEMENT_TYPE
#define GDK_ARRAY_ELEMENT_TYPE gpointer
#endif

#ifdef GDK_ARRAY_PREALLOC
#if GDK_ARRAY_PREALLOC == 0
#undef GDK_ARRAY_PREALLOC
#endif
#endif

#ifdef GDK_ARRAY_NULL_TERMINATED
#define GDK_ARRAY_REAL_SIZE(_size) ((_size) + 1)
#define GDK_ARRAY_MAX_SIZE (G_MAXSIZE / sizeof (_T_) - 1)
#else
#define GDK_ARRAY_REAL_SIZE(_size) (_size)
#define GDK_ARRAY_MAX_SIZE (G_MAXSIZE / sizeof (_T_))
#endif

/* make this readable */
#define _T_ GDK_ARRAY_ELEMENT_TYPE
#define GdkArray GDK_ARRAY_TYPE_NAME
#define gdk_array_paste_more(GDK_ARRAY_NAME, func_name) GDK_ARRAY_NAME ## _ ## func_name
#define gdk_array_paste(GDK_ARRAY_NAME, func_name) gdk_array_paste_more (GDK_ARRAY_NAME, func_name)
#define gdk_array(func_name) gdk_array_paste (GDK_ARRAY_NAME, func_name)

typedef struct GdkArray GdkArray;

struct GdkArray
{
  _T_ *start;
  _T_ *end;
  _T_ *end_allocation;
#ifdef GDK_ARRAY_PREALLOC
  _T_ preallocated[GDK_ARRAY_REAL_SIZE(GDK_ARRAY_PREALLOC)];
#endif
};

/* no G_GNUC_UNUSED here, if you don't use an array type, remove it. */
static inline void
gdk_array(init) (GdkArray *this)
{
#ifdef GDK_ARRAY_PREALLOC
  this->start = this->preallocated;
  this->end = this->start;
  this->end_allocation = this->start + GDK_ARRAY_PREALLOC;
#ifdef GDK_ARRAY_NULL_TERMINATED
  *this->start = *(_T_[1]) { 0 };
#endif
#else
  this->start = NULL;
  this->end = NULL;
  this->end_allocation = NULL;
#endif
}

G_GNUC_UNUSED static inline gsize
gdk_array(get_capacity) (const GdkArray *this)
{
  return this->end_allocation - this->start;
}

G_GNUC_UNUSED static inline gsize
gdk_array(get_size) (const GdkArray *this)
{
  return this->end - this->start;
}

static inline void
gdk_array(free_elements) (_T_ *start,
                          _T_ *end)
{
#ifdef GDK_ARRAY_FREE_FUNC
  _T_ *e;
  for (e = start; e < end; e++)
#ifdef GDK_ARRAY_BY_VALUE
    GDK_ARRAY_FREE_FUNC (e);
#else
    GDK_ARRAY_FREE_FUNC (*e);
#endif
#endif
}

/* no G_GNUC_UNUSED here */
static inline void
gdk_array(clear) (GdkArray *this)
{
  gdk_array(free_elements) (this->start, this->end);

#ifdef GDK_ARRAY_PREALLOC
  if (this->start != this->preallocated)
#endif
    g_free (this->start);
  gdk_array(init) (this);
}

/*
 * gdk_array_steal:
 * @this: the array
 *
 * Steals all data in the array and clears the array.
 *
 * If you need to know the size of the data, you should query it
 * beforehand.
 *
 * Returns: The array's data
 **/
G_GNUC_UNUSED static inline _T_ *
gdk_array(steal) (GdkArray *this)
{
  _T_ *result;

#ifdef GDK_ARRAY_PREALLOC
  if (this->start == this->preallocated)
    {
      gsize size = GDK_ARRAY_REAL_SIZE (gdk_array(get_size) (this));
      result = g_new (_T_, size);
      memcpy (result, this->preallocated, sizeof (_T_) * size);
    }
  else
#endif
    result = this->start;

  gdk_array(init) (this);

  return result;
}

G_GNUC_UNUSED static inline _T_ *
gdk_array(get_data) (const GdkArray *this)
{
  return this->start;
}

G_GNUC_UNUSED static inline _T_ *
gdk_array(index) (const GdkArray *this,
                  gsize           pos)
{
  return this->start + pos;
}

G_GNUC_UNUSED static inline gboolean
gdk_array(is_empty) (const GdkArray *this)
{
  return this->end == this->start;
}

G_GNUC_UNUSED static inline void
gdk_array(reserve) (GdkArray *this,
                    gsize      n)
{
  gsize new_capacity, size, capacity;

  if (G_UNLIKELY (n > GDK_ARRAY_MAX_SIZE))
    g_error ("requesting array size of %zu, but maximum size is %zu", n, GDK_ARRAY_MAX_SIZE);

  capacity = gdk_array(get_capacity) (this);
  if (n <= capacity)
     return;

  size = gdk_array(get_size) (this);
  /* capacity * 2 can overflow, that's why we MAX() */
  new_capacity = MAX (GDK_ARRAY_REAL_SIZE (n), capacity * 2);

#ifdef GDK_ARRAY_PREALLOC
  if (this->start == this->preallocated)
    {
      this->start = g_new (_T_, new_capacity);
      memcpy (this->start, this->preallocated, sizeof (_T_) * GDK_ARRAY_REAL_SIZE (size));
    }
  else
#endif
#ifdef GDK_ARRAY_NULL_TERMINATED
  if (this->start == NULL)
    {
      this->start = g_new (_T_, new_capacity);
      *this->start = *(_T_[1]) { 0 };
    }
  else
#endif
    this->start = g_renew (_T_, this->start, new_capacity);

  this->end = this->start + size;
  this->end_allocation = this->start + new_capacity;
#ifdef GDK_ARRAY_NULL_TERMINATED
  this->end_allocation--;
#endif
}

G_GNUC_UNUSED static inline void
gdk_array(splice) (GdkArray *this,
                   gsize      pos,
                   gsize      removed,
                   gboolean   stolen,
#ifdef GDK_ARRAY_BY_VALUE
                   const _T_ *additions,
#else
                   _T_       *additions,
#endif
                   gsize      added)
{
  gsize size;
  gsize remaining;

  size = gdk_array(get_size) (this);
  g_assert (pos + removed <= size);
  remaining = size - pos - removed;

  if (!stolen)
    gdk_array(free_elements) (gdk_array(index) (this, pos),
                              gdk_array(index) (this, pos + removed));

  gdk_array(reserve) (this, size - removed + added);

  if (GDK_ARRAY_REAL_SIZE (remaining) && removed != added)
    memmove (gdk_array(index) (this, pos + added),
             gdk_array(index) (this, pos + removed),
             GDK_ARRAY_REAL_SIZE (remaining) * sizeof (_T_));

  if (added)
    {
      if (additions)
        memcpy (gdk_array(index) (this, pos),
                additions,
                added * sizeof (_T_));
#ifndef GDK_ARRAY_NO_MEMSET
      else
        memset (gdk_array(index) (this, pos), 0, added * sizeof (_T_));
#endif
    }


  /* might overflow, but does the right thing */
  this->end += added - removed;
}

G_GNUC_UNUSED static void
gdk_array(set_size) (GdkArray *this,
                     gsize     new_size)
{
  gsize old_size = gdk_array(get_size) (this);
  if (new_size > old_size)
    gdk_array(splice) (this, old_size, 0, FALSE, NULL, new_size - old_size);
  else
    gdk_array(splice) (this, new_size, old_size - new_size, FALSE, NULL, 0);
}

G_GNUC_UNUSED static void
gdk_array(append) (GdkArray *this,
#ifdef GDK_ARRAY_BY_VALUE
                   _T_       *value)
#else
                   _T_        value)
#endif
{
  gdk_array(splice) (this, 
                     gdk_array(get_size) (this),
                     0,
                     FALSE,
#ifdef GDK_ARRAY_BY_VALUE
                     value,
#else
                     &value,
#endif
                     1);
}

#ifdef GDK_ARRAY_BY_VALUE
G_GNUC_UNUSED static _T_ *
gdk_array(get) (const GdkArray *this,
                gsize           pos)
{
  return gdk_array(index) (this, pos);
}
#else
G_GNUC_UNUSED static _T_
gdk_array(get) (const GdkArray *this,
                gsize           pos)
 {
   return *gdk_array(index) (this, pos);
 }
#endif

#ifndef GDK_ARRAY_NO_UNDEF

#undef _T_
#undef GdkArray
#undef gdk_array_paste_more
#undef gdk_array_paste
#undef gdk_array
#undef GDK_ARRAY_REAL_SIZE
#undef GDK_ARRAY_MAX_SIZE

#undef GDK_ARRAY_BY_VALUE
#undef GDK_ARRAY_ELEMENT_TYPE
#undef GDK_ARRAY_FREE_FUNC
#undef GDK_ARRAY_NAME
#undef GDK_ARRAY_NULL_TERMINATED
#undef GDK_ARRAY_PREALLOC
#undef GDK_ARRAY_TYPE_NAME
#undef GDK_ARRAY_NO_MEMSET
#endif

G_END_DECLS
