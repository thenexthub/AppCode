/*
 * Copyright © 2018 Benjamin Otte
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

#include "gtksortlistmodel.h"

#include "gtkbitset.h"
#include "gtkmultisorter.h"
#include "gtkprivate.h"
#include "gtksectionmodel.h"
#include "gtksorterprivate.h"
#include "timsort/gtktimsortprivate.h"

/* The maximum amount of items to merge for a single merge step
 *
 * Making this smaller will result in more steps, which has more overhead and slows
 * down total sort time.
 * Making it larger will result in fewer steps, which increases the time taken for
 * a single step.
 *
 * As merges are the most expensive steps, this is essentially a tunable for the
 * longest time spent in gtk_tim_sort_step().
 *
 * Note that this should be reset to 0 when not doing incremental sorting to get
 * rid of all the overhead.
 */
#define GTK_SORT_MAX_MERGE_SIZE (1024)

/* Time we spend in the sort callback before returning to the main loop
 *
 * Increasing this number will make the callback take longer and potentially
 * reduce responsiveness of an application, but will increase the amount of
 * work done per step. And we emit an ::items-changed() signal after every
 * step, so if we can avoid that, we recuce the overhead in the list widget
 * and in turn reduce the total sort time.
 */
#define GTK_SORT_STEP_TIME_US (1000) /* 1 millisecond */

/**
 * GtkSortListModel:
 *
 * A list model that sorts the elements of another model.
 *
 * The elements are sorted according to a `GtkSorter`.
 *
 * The model is a stable sort. If two items compare equal according
 * to the sorter, the one that appears first in the original model will
 * also appear first after sorting.
 *
 * Note that if you change the sorter, the previous order will have no
 * influence on the new order. If you want that, consider using a
 * `GtkMultiSorter` and appending the previous sorter to it.
 *
 * The model can be set up to do incremental sorting, so that
 * sorting long lists doesn't block the UI. See
 * [method@Gtk.SortListModel.set_incremental] for details.
 *
 * `GtkSortListModel` is a generic model and because of that it
 * cannot take advantage of any external knowledge when sorting.
 * If you run into performance issues with `GtkSortListModel`,
 * it is strongly recommended that you write your own sorting list
 * model.
 *
 * `GtkSortListModel` allows sorting the items into sections. It
 * implements `GtkSectionModel` and when [property@Gtk.SortListModel:section-sorter]
 * is set, it will sort all items with that sorter and items comparing
 * equal with it will be put into the same section.
 * The [property@Gtk.SortListModel:sorter] will then be used to sort items
 * inside their sections.
 */

enum {
  PROP_0,
  PROP_INCREMENTAL,
  PROP_ITEM_TYPE,
  PROP_MODEL,
  PROP_N_ITEMS,
  PROP_PENDING,
  PROP_SECTION_SORTER,
  PROP_SORTER,
  NUM_PROPERTIES
};

struct _GtkSortListModel
{
  GObject parent_instance;

  GListModel *model;
  GtkSorter *sorter;
  GtkSorter *section_sorter;
  GtkSorter *real_sorter;
  gboolean incremental;

  GtkTimSort sort; /* ongoing sort operation */
  guint sort_cb; /* 0 or current ongoing sort callback */

  guint n_items;
  GtkSortKeys *sort_keys;
  GtkSortKeys *section_sort_keys; /* we assume they are compatible with the sort keys because they're the first element */
  gsize key_size;
  gpointer keys;
  GtkBitset *missing_keys;

  gpointer *positions;
};

struct _GtkSortListModelClass
{
  GObjectClass parent_class;
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static guint
pos_from_key (GtkSortListModel *this,
              gpointer          key)
{
  guint pos = ((char *) key - (char *) this->keys) / this->key_size;

  g_assert (pos < this->n_items);

  return pos;
}

static gpointer
key_from_pos (GtkSortListModel *this,
              guint             pos)
{
  return (char *) this->keys + this->key_size * pos;
}

static GType
gtk_sort_list_model_get_item_type (GListModel *list)
{
  return G_TYPE_OBJECT;
}

static guint
gtk_sort_list_model_get_n_items (GListModel *list)
{
  GtkSortListModel *this = GTK_SORT_LIST_MODEL (list);

  if (this->model == NULL)
    return 0;

  return g_list_model_get_n_items (this->model);
}

static gpointer
gtk_sort_list_model_get_item (GListModel *list,
                              guint       position)
{
  GtkSortListModel *this = GTK_SORT_LIST_MODEL (list);

  if (this->model == NULL)
    return NULL;

  if (position >= this->n_items)
    return NULL;

  if (this->positions)
    position = pos_from_key (this, this->positions[position]);

  return g_list_model_get_item (this->model, position);
}

static void
gtk_sort_list_model_model_init (GListModelInterface *iface)
{
  iface->get_item_type = gtk_sort_list_model_get_item_type;
  iface->get_n_items = gtk_sort_list_model_get_n_items;
  iface->get_item = gtk_sort_list_model_get_item;
}

static void
gtk_sort_list_model_ensure_key (GtkSortListModel *this,
                                guint             pos)
{
  gpointer item;

  if (!gtk_bitset_contains (this->missing_keys, pos))
    return;

 item = g_list_model_get_item (this->model, pos);
 gtk_sort_keys_init_key (this->sort_keys, item, key_from_pos (this, pos));
 g_object_unref (item);

  gtk_bitset_remove (this->missing_keys, pos);
}

static void
gtk_sort_list_model_get_section_unsorted (GtkSortListModel *this,
                                          guint             position,
                                          guint            *out_start,
                                          guint            *out_end)
{
  gpointer *pos, *start, *end;

  pos = &this->positions[position];
  gtk_sort_list_model_ensure_key (this, pos_from_key (this, *pos));

  for (start = pos;
       start > this->positions;
       start--)
    {
      gtk_sort_list_model_ensure_key (this, pos_from_key (this, start[-1]));
      if (gtk_sort_keys_compare (this->section_sort_keys, start[-1], *pos) != GTK_ORDERING_EQUAL)
        break;
    }

  for (end = pos + 1;
       end < &this->positions[this->n_items];
       end++)
    {
      gtk_sort_list_model_ensure_key (this, pos_from_key (this, *end));
      if (gtk_sort_keys_compare (this->section_sort_keys, *end, *pos) != GTK_ORDERING_EQUAL)
        break;
    }

  *out_start = start - this->positions;
  *out_end = end - this->positions;
}

static void
gtk_sort_list_model_get_section_sorted (GtkSortListModel *this,
                                        guint             position,
                                        guint            *out_start,
                                        guint            *out_end)
{
  gpointer *pos;
  guint step, min, max, mid;

  pos = &this->positions[position];
  
  max = position;
  step = 1;
  while (max > 0)
    {
      min = max - MIN (max, step);
      step *= 2;
      if (gtk_sort_keys_compare (this->section_sort_keys, this->positions[min], *pos) == GTK_ORDERING_EQUAL)
        {
          max = min;
          continue;
        }
      /* now min is different, max is equal, bsearch where that changes */
      while (max - min > 1)
        {
          mid = (max + min) / 2;
          if (gtk_sort_keys_compare (this->section_sort_keys, this->positions[mid], *pos) == GTK_ORDERING_EQUAL)
            max = mid;
          else
            min = mid;
        }
      break;
    }
  *out_start = max;

  min = position;
  step = 1;
  while (min < this->n_items - 1)
    {
      max = min + MIN (this->n_items - 1 - min, step);
      step *= 2;
      if (gtk_sort_keys_compare (this->section_sort_keys, this->positions[max], *pos) == GTK_ORDERING_EQUAL)
        {
          min = max;
          continue;
        }
      /* now min is equal, max is different, bsearch where that changes */
      while (max - min > 1)
        {
          mid = (max + min) / 2;
          if (gtk_sort_keys_compare (this->section_sort_keys, this->positions[mid], *pos) == GTK_ORDERING_EQUAL)
            min = mid;
          else
            max = mid;
        }
      break;
    }
  *out_end = min + 1;
}

static void
gtk_sort_list_model_get_section (GtkSectionModel *model,
                                 guint            position,
                                 guint           *out_start,
                                 guint           *out_end)
{
  GtkSortListModel *this = GTK_SORT_LIST_MODEL (model);

  if (position >= this->n_items)
    {
      *out_start = this->n_items;
      *out_end = G_MAXUINT;
      return;
    }

  if (this->section_sort_keys == NULL)
    {
      *out_start = 0;
      *out_end = this->n_items;
      return;
    }

  /* When the list is not sorted:
   * - keys may not exist yet
   * - equal items may not be adjacent
   * So add a slow path that can deal with that, but is O(N).
   * The fast path is O(log N) and will be used for I guess
   * 99% of cases.
   */
  if (this->sort_cb)
    gtk_sort_list_model_get_section_unsorted (this, position, out_start, out_end);
  else
    gtk_sort_list_model_get_section_sorted (this, position, out_start, out_end);
}

static void
gtk_sort_list_model_section_model_init (GtkSectionModelInterface *iface)
{
  iface->get_section = gtk_sort_list_model_get_section;
}

G_DEFINE_TYPE_WITH_CODE (GtkSortListModel, gtk_sort_list_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, gtk_sort_list_model_model_init)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_SECTION_MODEL, gtk_sort_list_model_section_model_init))

static gboolean
gtk_sort_list_model_is_sorting (GtkSortListModel *this)
{
  return this->sort_cb != 0;
}

static void
gtk_sort_list_model_stop_sorting (GtkSortListModel *this,
                                  gsize            *runs)
{
  if (this->sort_cb == 0)
    {
      if (runs)
        {
          runs[0] = this->n_items;
          runs[1] = 0;
        }
      return;
    }

  if (runs)
    gtk_tim_sort_get_runs (&this->sort, runs);
  gtk_tim_sort_finish (&this->sort);
  g_clear_handle_id (&this->sort_cb, g_source_remove);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_PENDING]);
}

static gboolean
gtk_sort_list_model_sort_step (GtkSortListModel *this,
                               gboolean          finish,
                               guint            *out_position,
                               guint            *out_n_items)
{
  gint64 end_time = g_get_monotonic_time ();
  gboolean result = FALSE;
  GtkTimSortRun change;
  gpointer *start_change, *end_change;

  end_time += GTK_SORT_STEP_TIME_US;

  if (!gtk_bitset_is_empty (this->missing_keys))
    {
      GtkBitsetIter iter;
      guint pos;

      for (gtk_bitset_iter_init_first (&iter, this->missing_keys, &pos);
           gtk_bitset_iter_is_valid (&iter);
           gtk_bitset_iter_next (&iter, &pos))
        {
          gpointer item = g_list_model_get_item (this->model, pos);
          gtk_sort_keys_init_key (this->sort_keys, item, key_from_pos (this, pos));
          g_object_unref (item);

          if (g_get_monotonic_time () >= end_time && !finish)
            {
              gtk_bitset_remove_range_closed (this->missing_keys, 0, pos);
              *out_position = 0;
              *out_n_items = 0;
              return TRUE;
            }
        }
      result = TRUE;
      gtk_bitset_remove_all (this->missing_keys);
    }

  end_change = this->positions;
  start_change = this->positions + this->n_items;

  while (gtk_tim_sort_step (&this->sort, &change))
    {
      result = TRUE;
      if (change.len)
        {
          start_change = MIN (start_change, (gpointer *) change.base);
          end_change = MAX (end_change, ((gpointer *) change.base) + change.len);
        }
     
      if (g_get_monotonic_time () >= end_time && !finish)
        break;
    }

  if (start_change < end_change)
    {
      *out_position = start_change - this->positions;
      *out_n_items = end_change - start_change;
    }
  else
    {
      *out_position = 0;
      *out_n_items = 0;
    }

  return result;
}

static gboolean
gtk_sort_list_model_sort_cb (gpointer data)
{
  GtkSortListModel *this = data;
  guint pos, n_items;

  if (gtk_sort_list_model_sort_step (this, FALSE, &pos, &n_items))
    {
      if (n_items)
        g_list_model_items_changed (G_LIST_MODEL (this), pos, n_items, n_items);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_PENDING]);
      return G_SOURCE_CONTINUE;
    }

  gtk_sort_list_model_stop_sorting (this, NULL);
  return G_SOURCE_REMOVE;
}

static int
sort_func (gconstpointer a,
           gconstpointer b,
           gpointer      data)
{
  gpointer *sa = (gpointer *) a;
  gpointer *sb = (gpointer *) b;
  int result;

  result = gtk_sort_keys_compare (data, *sa, *sb);
  if (result)
    return result;

  return *sa < *sb ? -1 : 1;
}

static gboolean
gtk_sort_list_model_start_sorting (GtkSortListModel *this,
                                   gsize            *runs)
{
  g_assert (this->sort_cb == 0);

  gtk_tim_sort_init (&this->sort,
                     this->positions,
                     this->n_items,
                     sizeof (gpointer),
                     sort_func,
                     this->sort_keys);
  if (runs)
    gtk_tim_sort_set_runs (&this->sort, runs);
  if (this->incremental)
    gtk_tim_sort_set_max_merge_size (&this->sort, GTK_SORT_MAX_MERGE_SIZE);

  if (!this->incremental)
    return FALSE;

  this->sort_cb = g_idle_add (gtk_sort_list_model_sort_cb, this);
  gdk_source_set_static_name_by_id (this->sort_cb, "[gtk] gtk_sort_list_model_sort_cb");
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_PENDING]);
  return TRUE;
}

static void
gtk_sort_list_model_finish_sorting (GtkSortListModel *this,
                                    guint            *pos,
                                    guint            *n_items)
{
  gtk_tim_sort_set_max_merge_size (&this->sort, 0);

  gtk_sort_list_model_sort_step (this, TRUE, pos, n_items);
  gtk_tim_sort_finish (&this->sort);

  gtk_sort_list_model_stop_sorting (this, NULL);
}

static void
gtk_sort_list_model_clear_sort_keys (GtkSortListModel *this,
                                     guint             position,
                                     guint             n_items)
{
  GtkBitsetIter iter;
  GtkBitset *clear;
  guint pos;

  if (!gtk_sort_keys_needs_clear_key (this->sort_keys))
    return;

  clear = gtk_bitset_new_range (position, n_items);
  gtk_bitset_subtract (clear, this->missing_keys);

  for (gtk_bitset_iter_init_first (&iter, clear, &pos);
       gtk_bitset_iter_is_valid (&iter);
       gtk_bitset_iter_next (&iter, &pos))
    {
      gtk_sort_keys_clear_key (this->sort_keys, key_from_pos (this, pos));
    }

  gtk_bitset_unref (clear);
}

static void
gtk_sort_list_model_clear_keys (GtkSortListModel *this)
{
  gtk_sort_list_model_clear_sort_keys (this, 0, this->n_items);

  g_clear_pointer (&this->missing_keys, gtk_bitset_unref);
  g_clear_pointer (&this->keys, g_free);
  g_clear_pointer (&this->sort_keys, gtk_sort_keys_unref);
  g_clear_pointer (&this->section_sort_keys, gtk_sort_keys_unref);
  this->key_size = 0;
}

static void
gtk_sort_list_model_clear_items (GtkSortListModel *this,
                                 guint            *pos,
                                 guint            *n_items)
{
  gtk_sort_list_model_stop_sorting (this, NULL);

  if (this->sort_keys == NULL)
    {
      if (pos || n_items)
        *pos = *n_items = 0;
      return;
    }

  if (pos || n_items)
    {
      guint start, end;

      for (start = 0; start < this->n_items; start++)
        {
          if (pos_from_key (this, this->positions[start]) != + start)
            break;
        }
      for (end = this->n_items; end > start; end--)
        {
          if (pos_from_key (this, this->positions[end - 1]) != end - 1)
            break;
        }

      *n_items = end - start;
      if (*n_items == 0)
        *pos = 0;
      else
        *pos = start;
    }

  g_clear_pointer (&this->positions, g_free);

  gtk_sort_list_model_clear_keys (this);
} 

static gboolean
gtk_sort_list_model_should_sort (GtkSortListModel *this)
{
  return this->real_sorter != NULL &&
         this->model != NULL &&
         gtk_sorter_get_order (this->real_sorter) != GTK_SORTER_ORDER_NONE;
}

static void
gtk_sort_list_model_create_keys (GtkSortListModel *this)
{
  g_assert (this->keys == NULL);
  g_assert (this->sort_keys == NULL);
  g_assert (this->section_sort_keys == NULL);
  g_assert (this->key_size == 0);

  this->sort_keys = gtk_sorter_get_keys (this->real_sorter);
  if (this->section_sorter)
    this->section_sort_keys = gtk_sorter_get_keys (this->section_sorter);
  this->key_size = GTK_SORT_KEYS_ALIGN (gtk_sort_keys_get_key_size (this->sort_keys),
                                        gtk_sort_keys_get_key_align (this->sort_keys));
  this->keys = g_malloc_n (this->n_items, this->key_size);
  this->missing_keys = gtk_bitset_new_range (0, this->n_items);
}

static void
gtk_sort_list_model_create_items (GtkSortListModel *this)
{
  guint i;

  if (!gtk_sort_list_model_should_sort (this))
    return;

  g_assert (this->sort_keys == NULL);

  this->positions = g_new (gpointer, this->n_items);

  gtk_sort_list_model_create_keys (this);

  for (i = 0; i < this->n_items; i++)
    this->positions[i] = key_from_pos (this, i);
}

/* This realloc()s the arrays but does not set the added values. */
static void
gtk_sort_list_model_update_items (GtkSortListModel *this,
                                  gsize             runs[GTK_TIM_SORT_MAX_PENDING + 1],
                                  guint             position,
                                  guint             removed,
                                  guint             added,
                                  guint            *unmodified_start,
                                  guint            *unmodified_end)
{
  guint i, n_items, valid;
  guint run_index, valid_run, valid_run_end, run_end;
  guint start, end;
  gpointer *old_keys;

  n_items = this->n_items;
  start = n_items;
  end = n_items;
  
  /* first, move the keys over */
  old_keys = this->keys;
  gtk_sort_list_model_clear_sort_keys (this, position, removed);

  if (removed > added)
    {
      memmove (key_from_pos (this, position + added),
               key_from_pos (this, position + removed),
               this->key_size * (n_items - position - removed));
      this->keys = g_realloc_n (this->keys, n_items - removed + added, this->key_size);
    }
  else if (removed < added)
    {
      this->keys = g_realloc_n (this->keys, n_items - removed + added, this->key_size);
      memmove (key_from_pos (this, position + added),
               key_from_pos (this, position + removed),
               this->key_size * (n_items - position - removed));
    }

  /* then, update the positions */
  valid = 0;
  valid_run = 0;
  valid_run_end = 0;
  run_index = 0;
  run_end = 0;
  for (i = 0; i < n_items;)
    {
      if (runs[run_index] == 0)
        {
          run_end = n_items;
          valid_run_end = G_MAXUINT;
        }
      else
        {
          run_end += runs[run_index++];
        }

      for (; i < run_end; i++)
        {
          guint pos = ((char *) this->positions[i] - (char *) old_keys) / this->key_size;

          if (pos >= position + removed)
            pos = pos - removed + added;
          else if (pos >= position)
            { 
              start = MIN (start, valid);
              end = n_items - i - 1;
              continue;
            }

          this->positions[valid] = key_from_pos (this, pos);
          valid++;
        }

      if (valid_run_end < valid)
        {
          runs[valid_run++] = valid - valid_run_end;
          valid_run_end = valid;
        }
    }
  g_assert (i == n_items);
  g_assert (valid == n_items - removed);
  runs[valid_run] = 0;

  this->positions = g_renew (gpointer, this->positions, n_items - removed + added);

  if (this->missing_keys)
    {
      gtk_bitset_splice (this->missing_keys, position, removed, added);
      gtk_bitset_add_range (this->missing_keys, position, added);
    }

  this->n_items = n_items - removed + added;

  for (i = 0; i < added; i++)
    {
      this->positions[this->n_items - added + i] = key_from_pos (this, position + i);
    }

  *unmodified_start = start;
  *unmodified_end = end;
}

static void
gtk_sort_list_model_items_changed_cb (GListModel       *model,
                                      guint             position,
                                      guint             removed,
                                      guint             added,
                                      GtkSortListModel *this)
{
  gsize runs[GTK_TIM_SORT_MAX_PENDING + 1];
  guint i, n_items, start, end;
  gboolean was_sorting;

  if (removed == 0 && added == 0)
    return;

  if (!gtk_sort_list_model_should_sort (this))
    {
      this->n_items = this->n_items - removed + added;
      g_list_model_items_changed (G_LIST_MODEL (this), position, removed, added);
      if (removed != added)
        g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
      return;
    }

  was_sorting = gtk_sort_list_model_is_sorting (this);
  gtk_sort_list_model_stop_sorting (this, runs);

  gtk_sort_list_model_update_items (this, runs, position, removed, added, &start, &end);

  if (added > 0)
    {
      if (gtk_sort_list_model_start_sorting (this, runs))
        {
          end = 0;
        }
      else
        {
          guint pos, n;
          gtk_sort_list_model_finish_sorting (this, &pos, &n);
          if (n)
            start = MIN (start, pos);
          /* find first item that was added */
          for (i = 0; i < end; i++)
            {
              pos = pos_from_key (this, this->positions[this->n_items - i - 1]);
              if (pos >= position && pos < position + added)
                {
                  end = i;
                  break;
                }
            }
        }
    }
  else
    {
      if (was_sorting)
        gtk_sort_list_model_start_sorting (this, runs);
    }

  n_items = this->n_items - start - end;
  g_list_model_items_changed (G_LIST_MODEL (this), start, n_items - added + removed, n_items);
  if (removed != added)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);
}

static void
gtk_sort_list_model_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GtkSortListModel *this = GTK_SORT_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_INCREMENTAL:
      gtk_sort_list_model_set_incremental (this, g_value_get_boolean (value));
      break;

    case PROP_MODEL:
      gtk_sort_list_model_set_model (this, g_value_get_object (value));
      break;

    case PROP_SECTION_SORTER:
      gtk_sort_list_model_set_section_sorter (this, g_value_get_object (value));
      break;

    case PROP_SORTER:
      gtk_sort_list_model_set_sorter (this, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
gtk_sort_list_model_get_property (GObject     *object,
                                  guint        prop_id,
                                  GValue      *value,
                                  GParamSpec  *pspec)
{
  GtkSortListModel *this = GTK_SORT_LIST_MODEL (object);

  switch (prop_id)
    {
    case PROP_INCREMENTAL:
      g_value_set_boolean (value, this->incremental);
      break;

    case PROP_ITEM_TYPE:
      g_value_set_gtype (value, gtk_sort_list_model_get_item_type (G_LIST_MODEL (this)));
      break;

    case PROP_MODEL:
      g_value_set_object (value, this->model);
      break;

    case PROP_N_ITEMS:
      g_value_set_uint (value, gtk_sort_list_model_get_n_items (G_LIST_MODEL (this)));
      break;

    case PROP_PENDING:
      g_value_set_uint (value, gtk_sort_list_model_get_pending (this));
      break;

    case PROP_SECTION_SORTER:
      g_value_set_object (value, this->section_sorter);
      break;

    case PROP_SORTER:
      g_value_set_object (value, this->sorter);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_sort_list_model_sorter_changed (GtkSorter        *sorter,
                                    int               change,
                                    GtkSortListModel *this,
                                    gboolean          sections_changed)
{
  guint pos, n_items;

  if (gtk_sort_list_model_should_sort (this))
    {
      gtk_sort_list_model_stop_sorting (this, NULL);

      if (this->sort_keys == NULL)
        {
          gtk_sort_list_model_create_items (this);
        }
      else
        {
          GtkSortKeys *new_keys = gtk_sorter_get_keys (sorter);

          if (!gtk_sort_keys_is_compatible (new_keys, this->sort_keys))
            {
              char *old_keys = this->keys;
              gsize old_key_size = this->key_size;
              guint i;

              gtk_sort_list_model_clear_keys (this);
              gtk_sort_list_model_create_keys (this);

              for (i = 0; i < this->n_items; i++)
                this->positions[i] = key_from_pos (this, ((char *) this->positions[i] - old_keys) / old_key_size);

              gtk_sort_keys_unref (new_keys);
            }
          else
            {
              gtk_sort_keys_unref (this->sort_keys);
              this->sort_keys = new_keys;
            }
        }

      if (this->section_sorter)
        {
          gtk_sort_keys_unref (this->section_sort_keys);
          this->section_sort_keys = gtk_sorter_get_keys (this->section_sorter);
        }

      if (gtk_sort_list_model_start_sorting (this, NULL))
        pos = n_items = 0;
      else
        gtk_sort_list_model_finish_sorting (this, &pos, &n_items);
    }
  else
    {
      gtk_sort_list_model_clear_items (this, &pos, &n_items);
    }

  if (sections_changed && this->n_items > 0)
    {
      if (n_items > 0)
        g_list_model_items_changed (G_LIST_MODEL (this), 0, this->n_items, this->n_items);
      else
        gtk_section_model_sections_changed (GTK_SECTION_MODEL (this), 0, this->n_items);
    }
  else if (n_items > 0)
    {
      g_list_model_items_changed (G_LIST_MODEL (this), pos, n_items, n_items);
    }
}

static void
gtk_sort_list_model_sorter_changed_cb (GtkSorter        *sorter,
                                       int               change,
                                       GtkSortListModel *this)
{
  gtk_sort_list_model_sorter_changed (sorter, change, this, FALSE);
}

static void
gtk_sort_list_model_clear_model (GtkSortListModel *this)
{
  if (this->model == NULL)
    return;

  g_signal_handlers_disconnect_by_func (this->model, gtk_sort_list_model_items_changed_cb, this);
  g_clear_object (&this->model);
  gtk_sort_list_model_clear_items (this, NULL, NULL);
  this->n_items = 0;
}

static void
gtk_sort_list_model_clear_real_sorter (GtkSortListModel *this)
{
  if (this->real_sorter == NULL)
    return;

  g_signal_handlers_disconnect_by_func (this->real_sorter, gtk_sort_list_model_sorter_changed_cb, this);
  g_clear_object (&this->real_sorter);
}

static void
gtk_sort_list_model_ensure_real_sorter (GtkSortListModel *this,
                                        gboolean          sections_changed)
{
  if (this->sorter)
    {
      if (this->section_sorter)
        {
          GtkMultiSorter *multi;

          multi = gtk_multi_sorter_new ();
          this->real_sorter = GTK_SORTER (multi);
          gtk_multi_sorter_append (multi, g_object_ref (this->section_sorter));
          gtk_multi_sorter_append (multi, g_object_ref (this->sorter));
        }
      else
        this->real_sorter = g_object_ref (this->sorter);
    }
  else
    {
      if (this->section_sorter)
        this->real_sorter = g_object_ref (this->section_sorter);
    }

  if (this->real_sorter)
    g_signal_connect (this->real_sorter, "changed", G_CALLBACK (gtk_sort_list_model_sorter_changed_cb), this);

  gtk_sort_list_model_sorter_changed (this->real_sorter, GTK_SORTER_CHANGE_DIFFERENT, this, sections_changed);
}

static void
gtk_sort_list_model_dispose (GObject *object)
{
  GtkSortListModel *this = GTK_SORT_LIST_MODEL (object);

  gtk_sort_list_model_clear_model (this);
  gtk_sort_list_model_clear_real_sorter (this);
  g_clear_object (&this->section_sorter);
  g_clear_object (&this->sorter);

  G_OBJECT_CLASS (gtk_sort_list_model_parent_class)->dispose (object);
};

static void
gtk_sort_list_model_class_init (GtkSortListModelClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);

  gobject_class->set_property = gtk_sort_list_model_set_property;
  gobject_class->get_property = gtk_sort_list_model_get_property;
  gobject_class->dispose = gtk_sort_list_model_dispose;

  /**
   * GtkSortListModel:incremental:
   *
   * If the model should sort items incrementally.
   */
  properties[PROP_INCREMENTAL] =
      g_param_spec_boolean ("incremental", NULL, NULL,
                            FALSE,
                            GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSortListModel:item-type:
   *
   * The type of items. See [method@Gio.ListModel.get_item_type].
   *
   * Since: 4.8
   **/
  properties[PROP_ITEM_TYPE] =
    g_param_spec_gtype ("item-type", NULL, NULL,
                        G_TYPE_OBJECT,
                        G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkSortListModel:model:
   *
   * The model being sorted.
   */
  properties[PROP_MODEL] =
      g_param_spec_object ("model", NULL, NULL,
                           G_TYPE_LIST_MODEL,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSortListModel:n-items:
   *
   * The number of items. See [method@Gio.ListModel.get_n_items].
   *
   * Since: 4.8
   **/
  properties[PROP_N_ITEMS] =
    g_param_spec_uint ("n-items", NULL, NULL,
                       0, G_MAXUINT, 0,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkSortListModel:pending:
   *
   * Estimate of unsorted items remaining.
   */
  properties[PROP_PENDING] =
      g_param_spec_uint ("pending", NULL, NULL,
                         0, G_MAXUINT, 0,
                         GTK_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSortListModel:section-sorter:
   *
   * The section sorter for this model, if one is set.
   *
   * Since: 4.12
   */
  properties[PROP_SECTION_SORTER] =
      g_param_spec_object ("section-sorter", NULL, NULL,
                           GTK_TYPE_SORTER,
                           GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSortListModel:sorter:
   *
   * The sorter for this model.
   */
  properties[PROP_SORTER] =
      g_param_spec_object ("sorter", NULL, NULL,
                            GTK_TYPE_SORTER,
                            GTK_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (gobject_class, NUM_PROPERTIES, properties);
}

static void
gtk_sort_list_model_init (GtkSortListModel *this)
{
}

/**
 * gtk_sort_list_model_new:
 * @model: (nullable) (transfer full): the model to sort
 * @sorter: (nullable) (transfer full): the `GtkSorter` to sort @model with,
 *
 * Creates a new sort list model that uses the @sorter to sort @model.
 *
 * Returns: a new `GtkSortListModel`
 */
GtkSortListModel *
gtk_sort_list_model_new (GListModel *model,
                         GtkSorter  *sorter)
{
  GtkSortListModel *result;

  g_return_val_if_fail (model == NULL || G_IS_LIST_MODEL (model), NULL);
  g_return_val_if_fail (sorter == NULL || GTK_IS_SORTER (sorter), NULL);

  result = g_object_new (GTK_TYPE_SORT_LIST_MODEL,
                         "model", model,
                         "sorter", sorter,
                         NULL);

  /* consume the references */
  g_clear_object (&model);
  g_clear_object (&sorter);

  return result;
}

/**
 * gtk_sort_list_model_set_model:
 * @this: a `GtkSortListModel`
 * @model: (nullable): The model to be sorted
 *
 * Sets the model to be sorted.
 *
 * The @model's item type must conform to the item type of @this.
 */
void
gtk_sort_list_model_set_model (GtkSortListModel *this,
                               GListModel       *model)
{
  guint removed;

  g_return_if_fail (GTK_IS_SORT_LIST_MODEL (this));
  g_return_if_fail (model == NULL || G_IS_LIST_MODEL (model));

  if (this->model == model)
    return;

  removed = g_list_model_get_n_items (G_LIST_MODEL (this));
  gtk_sort_list_model_clear_model (this);

  if (model)
    {
      guint ignore1, ignore2;

      this->model = g_object_ref (model);
      this->n_items = g_list_model_get_n_items (model);
      g_signal_connect (model, "items-changed", G_CALLBACK (gtk_sort_list_model_items_changed_cb), this);

      if (gtk_sort_list_model_should_sort (this))
        {
          gtk_sort_list_model_create_items (this);
          if (!gtk_sort_list_model_start_sorting (this, NULL))
            gtk_sort_list_model_finish_sorting (this, &ignore1, &ignore2);
        }
    }
  
  if (removed > 0 || this->n_items > 0)
    g_list_model_items_changed (G_LIST_MODEL (this), 0, removed, this->n_items);
  if (removed != this->n_items)
    g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_N_ITEMS]);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MODEL]);
}

/**
 * gtk_sort_list_model_get_model:
 * @this: a `GtkSortListModel`
 *
 * Gets the model currently sorted or %NULL if none.
 *
 * Returns: (nullable) (transfer none): The model that gets sorted
 */
GListModel *
gtk_sort_list_model_get_model (GtkSortListModel *this)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (this), NULL);

  return this->model;
}

/**
 * gtk_sort_list_model_set_sorter:
 * @this: a `GtkSortListModel`
 * @sorter: (nullable): the `GtkSorter` to sort @model with
 *
 * Sets a new sorter on @this.
 */
void
gtk_sort_list_model_set_sorter (GtkSortListModel *this,
                                GtkSorter        *sorter)
{
  g_return_if_fail (GTK_IS_SORT_LIST_MODEL (this));
  g_return_if_fail (sorter == NULL || GTK_IS_SORTER (sorter));

  if (this->sorter == sorter)
    return;

  gtk_sort_list_model_clear_real_sorter (this);
  g_set_object (&this->sorter, sorter);
  gtk_sort_list_model_ensure_real_sorter (this, FALSE);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SORTER]);
}

/**
 * gtk_sort_list_model_get_sorter:
 * @this: a `GtkSortListModel`
 *
 * Gets the sorter that is used to sort @this.
 *
 * Returns: (nullable) (transfer none): the sorter of #this
 */
GtkSorter *
gtk_sort_list_model_get_sorter (GtkSortListModel *this)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (this), NULL);

  return this->sorter;
}

/**
 * gtk_sort_list_model_set_section_sorter:
 * @this: a `GtkSortListModel`
 * @sorter: (nullable): the `GtkSorter` to sort @model with
 *
 * Sets a new section sorter on @this.
 *
 * Since: 4.12
 */
void
gtk_sort_list_model_set_section_sorter (GtkSortListModel *this,
                                        GtkSorter        *sorter)
{
  g_return_if_fail (GTK_IS_SORT_LIST_MODEL (this));
  g_return_if_fail (sorter == NULL || GTK_IS_SORTER (sorter));

  if (this->section_sorter == sorter)
    return;

  gtk_sort_list_model_clear_real_sorter (this);
  g_set_object (&this->section_sorter, sorter);
  gtk_sort_list_model_ensure_real_sorter (this, TRUE);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SECTION_SORTER]);
}

/**
 * gtk_sort_list_model_get_section_sorter:
 * @this: a `GtkSortListModel`
 *
 * Gets the section sorter that is used to sort items of @this into
 * sections.
 *
 * Returns: (nullable) (transfer none): the sorter of #this
 *
 * Since: 4.12
 */
GtkSorter *
gtk_sort_list_model_get_section_sorter (GtkSortListModel *this)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (this), NULL);

  return this->section_sorter;
}

/**
 * gtk_sort_list_model_set_incremental:
 * @this: a `GtkSortListModel`
 * @incremental: %TRUE to sort incrementally
 *
 * Sets the sort model to do an incremental sort.
 *
 * When incremental sorting is enabled, the `GtkSortListModel` will not do
 * a complete sort immediately, but will instead queue an idle handler that
 * incrementally sorts the items towards their correct position. This of
 * course means that items do not instantly appear in the right place. It
 * also means that the total sorting time is a lot slower.
 *
 * When your filter blocks the UI while sorting, you might consider
 * turning this on. Depending on your model and sorters, this may become
 * interesting around 10,000 to 100,000 items.
 *
 * By default, incremental sorting is disabled.
 *
 * See [method@Gtk.SortListModel.get_pending] for progress information
 * about an ongoing incremental sorting operation.
 */
void
gtk_sort_list_model_set_incremental (GtkSortListModel *this,
                                     gboolean          incremental)
{
  g_return_if_fail (GTK_IS_SORT_LIST_MODEL (this));

  if (this->incremental == incremental)
    return;

  this->incremental = incremental;

  if (!incremental && gtk_sort_list_model_is_sorting (this))
    {
      guint pos, n_items;

      gtk_sort_list_model_finish_sorting (this, &pos, &n_items);
      if (n_items)
        g_list_model_items_changed (G_LIST_MODEL (this), pos, n_items, n_items);
    }

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INCREMENTAL]);
}

/**
 * gtk_sort_list_model_get_incremental:
 * @this: a `GtkSortListModel`
 *
 * Returns whether incremental sorting is enabled.
 *
 * See [method@Gtk.SortListModel.set_incremental].
 *
 * Returns: %TRUE if incremental sorting is enabled
 */
gboolean
gtk_sort_list_model_get_incremental (GtkSortListModel *this)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (this), FALSE);

  return this->incremental;
}

/**
 * gtk_sort_list_model_get_pending:
 * @this: a `GtkSortListModel`
 *
 * Estimates progress of an ongoing sorting operation.
 *
 * The estimate is the number of items that would still need to be
 * sorted to finish the sorting operation if this was a linear
 * algorithm. So this number is not related to how many items are
 * already correctly sorted.
 *
 * If you want to estimate the progress, you can use code like this:
 * ```c
 * pending = gtk_sort_list_model_get_pending (this);
 * model = gtk_sort_list_model_get_model (this);
 * progress = 1.0 - pending / (double) MAX (1, g_list_model_get_n_items (model));
 * ```
 *
 * If no sort operation is ongoing - in particular when
 * [property@Gtk.SortListModel:incremental] is %FALSE - this
 * function returns 0.
 *
 * Returns: a progress estimate of remaining items to sort
 */
guint
gtk_sort_list_model_get_pending (GtkSortListModel *this)
{
  g_return_val_if_fail (GTK_IS_SORT_LIST_MODEL (this), FALSE);

  if (this->sort_cb == 0)
    return 0;

  /* We do a random guess that 50% of time is spent generating keys
   * and the other 50% is spent actually sorting.
   *
   * This is of course massively wrong, but it depends on the sorter
   * in use, and estimating this correctly is hard, so this will have
   * to be good enough.
   */
  if (!gtk_bitset_is_empty (this->missing_keys))
    {
      return (this->n_items + gtk_bitset_get_size (this->missing_keys)) / 2;
    }
  else
    {
      return (this->n_items - gtk_tim_sort_get_progress (&this->sort)) / 2;
    }
}

