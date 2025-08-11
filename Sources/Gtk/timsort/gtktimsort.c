/* Lots of code for an adaptive, stable, natural mergesort.  There are many
 * pieces to this algorithm; read listsort.txt for overviews and details.
 */

#include "config.h"

#include "gtktimsortprivate.h"

/*
 * This is the minimum sized sequence that will be merged.  Shorter
 * sequences will be lengthened by calling binarySort.  If the entire
 * array is less than this length, no merges will be performed.
 *
 * This constant should be a power of two.  It was 64 in Tim Peter's C
 * implementation, but 32 was empirically determined to work better in
 * [Android's Java] implementation.  In the unlikely event that you set
 * this constant to be a number that's not a power of two, you'll need
 * to change the compute_min_run() computation.
 *
 * If you decrease this constant, you must change the
 * GTK_TIM_SORT_MAX_PENDING value, or you risk running out of space.
 * See Python's listsort.txt for a discussion of the minimum stack
 * length required as a function of the length of the array being sorted and
 * the minimum merge sequence length.
 */
#define MIN_MERGE 32

/*
 * When we get into galloping mode, we stay there until both runs win less
 * often than MIN_GALLOP consecutive times.
 */
#define MIN_GALLOP 7

/*
 * Returns the minimum acceptable run length for an array of the specified
 * length. Natural runs shorter than this will be extended with binary sort.
 *
 * Roughly speaking, the computation is:
 *
 *  If n < MIN_MERGE, return n (it's too small to bother with fancy stuff).
 *  Else if n is an exact power of 2, return MIN_MERGE/2.
 *  Else return an int k, MIN_MERGE/2 <= k <= MIN_MERGE, such that n/k
 *   is close to, but strictly less than, an exact power of 2.
 *
 * For the rationale, see listsort.txt.
 *
 * @param n the length of the array to be sorted
 * @return the length of the minimum run to be merged
 */
static gsize
compute_min_run (gsize n)
{
  gsize r = 0; // Becomes 1 if any 1 bits are shifted off

  while (n >= MIN_MERGE) {
    r |= (n & 1);
    n >>= 1;
  }
  return n + r;
}

void
gtk_tim_sort_init (GtkTimSort       *this,
                   gpointer          base,
                   gsize             size,
                   gsize             element_size,
                   GCompareDataFunc  compare_func,
                   gpointer          data)
{
  this->element_size = element_size;
  this->base = base;
  this->size = size;
  this->compare_func = compare_func;
  this->data = data;

  this->min_gallop = MIN_GALLOP;
  this->max_merge_size = G_MAXSIZE;
  this->min_run = compute_min_run (size);

  this->tmp = NULL;
  this->tmp_length = 0;
  this->pending_runs = 0;
}

void
gtk_tim_sort_finish (GtkTimSort *this)
{
  g_clear_pointer (&this->tmp, g_free);
}

void
gtk_tim_sort (gpointer         base,
              gsize            size,
              gsize            element_size,
              GCompareDataFunc compare_func,
              gpointer         user_data)
{
  GtkTimSort this;

  gtk_tim_sort_init (&this, base, size, element_size, compare_func, user_data);

  while (gtk_tim_sort_step (&this, NULL));

  gtk_tim_sort_finish (&this);
}

static inline int
gtk_tim_sort_compare (GtkTimSort *this,
                      gpointer    a,
                      gpointer    b)
{
  return this->compare_func (a, b, this->data);
}


/**
 * Pushes the specified run onto the pending-run stack.
 *
 * @param runBase index of the first element in the run
 * @param runLen  the number of elements in the run
 */
static void
gtk_tim_sort_push_run (GtkTimSort *this,
                       void       *base,
                       gsize       len)
{
  g_assert (this->pending_runs < GTK_TIM_SORT_MAX_PENDING);
  g_assert (len <= this->size);

  this->run[this->pending_runs].base = base;
  this->run[this->pending_runs].len = len;
  this->pending_runs++;

  /* Advance to find next run */
  this->base = ((char *) this->base) + len * this->element_size;
  this->size -= len;
}

/**
 * Ensures that the external array tmp has at least the specified
 * number of elements, increasing its size if necessary.  The size
 * increases exponentially to ensure amortized linear time complexity.
 *
 * @param min_capacity the minimum required capacity of the tmp array
 * @return tmp, whether or not it grew
 */
static gpointer
gtk_tim_sort_ensure_capacity (GtkTimSort *this,
                              gsize       min_capacity)
{
  if (this->tmp_length < min_capacity)
    {
      /* Compute smallest power of 2 > min_capacity */
      gsize new_size = min_capacity;
      new_size |= new_size >> 1;
      new_size |= new_size >> 2;
      new_size |= new_size >> 4;
      new_size |= new_size >> 8;
      new_size |= new_size >> 16;
#if GLIB_SIZEOF_SIZE_T > 4
      new_size |= new_size >> 32;
#endif

      new_size++;
      if (new_size == 0) /* (overflow) Not bloody likely! */
        new_size = min_capacity;

      g_free (this->tmp);
      this->tmp_length = new_size;
      this->tmp = g_malloc (this->tmp_length * this->element_size);
  }

  return this->tmp;
}

static void
gtk_tim_sort_set_change (GtkTimSortRun *out_change,
                         gpointer       base,
                         gsize          len)
{
  if (out_change)
    {
      out_change->base = base;
      out_change->len = len;
    }
}

/*<private>
 * gtk_tim_sort_get_runs:
 * @this: a GtkTimSort
 * @runs: (out) (caller-allocates): Place to store the 0-terminated list of
 *     runs
 *
 * Stores the already presorted list of runs - ranges of items that are
 * known to be sorted among themselves.
 *
 * This can be used with gtk_tim_sort_set_runs() when resuming a sort later.
 **/
void
gtk_tim_sort_get_runs (GtkTimSort *this,
                       gsize       runs[GTK_TIM_SORT_MAX_PENDING + 1])
{
  gsize i;

  g_return_if_fail (this);
  g_return_if_fail (runs);

  for (i = 0; i < this->pending_runs; i++)
    runs[i] = this->run[i].len;

  runs[this->pending_runs] = 0;
}

/*<private>
 * gtk_tim_sort_set_runs:
 * @this: a freshly initialized GtkTimSort
 * @runs: (array length=zero-terminated): a 0-terminated list of runs
 *
 * Sets the list of runs. A run is a range of items that are already
 * sorted correctly among themselves. Runs must appear at the beginning of
 * the array.
 *
 * Runs can only be set at the beginning of the sort operation.
 **/
void
gtk_tim_sort_set_runs (GtkTimSort *this,
                       gsize      *runs)
{
  gsize i;

  g_return_if_fail (this);
  g_return_if_fail (this->pending_runs == 0);

  for (i = 0; runs[i] != 0; i++)
    gtk_tim_sort_push_run (this, this->base, runs[i]);
}

/*
 * gtk_tim_sort_set_max_merge_size:
 * @this: a GtkTimSort
 * @max_merge_size: Maximum size of a merge step, 0 for unlimited
 *
 * Sets the maximum size of a merge step. Every time
 * gtk_tim_sort_step() is called and a merge operation has to be
 * done, the @max_merge_size will be used to limit the size of
 * the merge.
 *
 * The benefit is that merges happen faster, and if you're using
 * an incremental sorting algorithm in the main thread, this will
 * limit the runtime.
 *
 * The disadvantage is that setting up merges is expensive and that
 * various optimizations benefit from larger merges, so the total
 * runtime of the sorting will increase with the number of merges.
 *
 * A good estimate is to set a @max_merge_size to 1024 for around
 * 1ms runtimes, if your compare function is fast.
 *
 * By default, max_merge_size is set to unlimited.
 **/
void
gtk_tim_sort_set_max_merge_size (GtkTimSort *this,
                                 gsize       max_merge_size)
{
  g_return_if_fail (this != NULL);

  if (max_merge_size == 0)
    max_merge_size = G_MAXSIZE;
  this->max_merge_size = max_merge_size;
}

/**
 * gtk_tim_sort_get_progress:
 * @this: a GtkTimSort
 *
 * Does a progress estimate about sort progress, estimates relative
 * to the number of items to sort.
 *
 * Note that this is entirely a progress estimate and does not have
 * a relationship with items put in their correct place.  
 * It is also an estimate, so no guarantees are made about accuracy,
 * other than that it will only report 100% completion when it is
 * indeed done sorting.
 *
 * To get a percentage, you need to divide this number by the total
 * number of elements that are being sorted.
 *
 * Returns: Rough guess of sort progress
 **/
gsize
gtk_tim_sort_get_progress (GtkTimSort *this)
{
#define DEPTH 4
  gsize i;
  gsize last, progress;

  g_return_val_if_fail (this != NULL, 0);

  if (this->pending_runs == 0)
    return 0;

  last = this->run[0].len;
  progress = 0;

  for (i = 1; i < DEPTH + 1 && i < this->pending_runs; i++)
    {
      progress += (DEPTH + 1 - i) * MAX (last, this->run[i].len);
      last = MIN (last, this->run[i].len);
    }
  if (i < DEPTH + 1)
    progress += (DEPTH + 1 - i) * last;

  return progress / DEPTH;
#undef DEPTH
}

#if 1
#define WIDTH 4
#include "gtktimsort-impl.c"

#define WIDTH 8
#include "gtktimsort-impl.c"

#define WIDTH 16
#include "gtktimsort-impl.c"
#endif

#define NAME default
#define WIDTH (this->element_size)
#include "gtktimsort-impl.c"

/*
 * gtk_tim_sort_step:
 * @this: a GtkTimSort
 * @out_change: (optional): Return location for changed
 *     area. If a change did not cause any changes (for example,
 *     if an already sorted array gets sorted), out_change
 *     will be set to %NULL and 0.
 *
 * Performs another step in the sorting process. If a
 * step was performed, %TRUE is returned and @out_change is
 * set to the smallest area that contains all changes while
 * sorting.
 *
 * If the data is completely sorted, %FALSE will be
 * returned.
 *
 * Returns: %TRUE if an action was performed
 **/
gboolean
gtk_tim_sort_step (GtkTimSort    *this,
                   GtkTimSortRun *out_change)
{
  gboolean result;

  g_assert (this);

  switch (this->element_size)
  {
    case 4:
      result = gtk_tim_sort_step_4 (this, out_change);
      break;
    case 8:
      result = gtk_tim_sort_step_8 (this, out_change);
      break;
    case 16:
      result = gtk_tim_sort_step_16 (this, out_change);
      break;
    default:
      result = gtk_tim_sort_step_default (this, out_change);
      break;
  }

  return result;
}

