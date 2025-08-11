/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
#include "config.h"

#include "gtkcenterlayout.h"

#include "gtkcsspositionvalueprivate.h"
#include "gtklayoutchild.h"
#include "gtkprivate.h"
#include "gtksizerequest.h"
#include "gtkwidgetprivate.h"
#include "gtkcssnodeprivate.h"

/**
 * GtkCenterLayout:
 *
 * Manages up to three children.
 *
 * The start widget is allocated at the start of the layout (left in
 * left-to-right locales and right in right-to-left ones), and the end
 * widget at the end.
 *
 * The center widget is centered regarding the full width of the layout's.
 */
struct _GtkCenterLayout
{
  GtkLayoutManager parent_instance;

  GtkBaselinePosition baseline_pos;
  GtkOrientation orientation;
  gboolean shrink_center_last;

  union {
    struct {
      GtkWidget *start_widget;
      GtkWidget *center_widget;
      GtkWidget *end_widget;
    };
    GtkWidget *children[3];
  };
};

enum {
  PROP_0,
  PROP_SHRINK_CENTER_LAST,
  LAST_PROP
};

static GParamSpec *props[LAST_PROP] = { NULL, };

G_DEFINE_TYPE (GtkCenterLayout, gtk_center_layout, GTK_TYPE_LAYOUT_MANAGER)

static int
get_spacing (GtkCenterLayout *this,
             GtkCssNode      *node)
{
  GtkCssStyle *style = gtk_css_node_get_style (node);
  GtkCssValue *border_spacing;
  int css_spacing;

  border_spacing = style->size->border_spacing;
  if (this->orientation == GTK_ORIENTATION_HORIZONTAL)
    css_spacing = _gtk_css_position_value_get_x (border_spacing, 100);
  else
    css_spacing = _gtk_css_position_value_get_y (border_spacing, 100);

  return css_spacing;
}

static GtkSizeRequestMode
gtk_center_layout_get_request_mode (GtkLayoutManager *layout_manager,
                                    GtkWidget        *widget)
{
  GtkCenterLayout *this = GTK_CENTER_LAYOUT (layout_manager);
  int count[3] = { 0, 0, 0 };

  if (this->start_widget)
    count[gtk_widget_get_request_mode (this->start_widget)]++;

  if (this->center_widget)
    count[gtk_widget_get_request_mode (this->center_widget)]++;

  if (this->end_widget)
    count[gtk_widget_get_request_mode (this->end_widget)]++;

  if (!count[GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH] &&
      !count[GTK_SIZE_REQUEST_WIDTH_FOR_HEIGHT])
    return GTK_SIZE_REQUEST_CONSTANT_SIZE;
  else
    return count[GTK_SIZE_REQUEST_WIDTH_FOR_HEIGHT] > count[GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH]
           ? GTK_SIZE_REQUEST_WIDTH_FOR_HEIGHT
           : GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH;
}

static void
gtk_center_layout_distribute (GtkCenterLayout  *this,
                              int               for_size,
                              int               size,
                              int               spacing,
                              GtkRequestedSize *sizes)
{
  int center_size = 0;
  int start_size = 0;
  int end_size = 0;
  gboolean center_expand = FALSE;
  gboolean start_expand = FALSE;
  gboolean end_expand = FALSE;
  int avail;
  int i;
  int needed_spacing = 0;

  /* Usable space is really less... */
  for (i = 0; i < 3; i++)
    {
      if (this->children[i])
        needed_spacing += spacing;
    }
  needed_spacing -= spacing;

  sizes[0].minimum_size = sizes[0].natural_size = 0;
  sizes[1].minimum_size = sizes[1].natural_size = 0;
  sizes[2].minimum_size = sizes[2].natural_size = 0;

  for (i = 0; i < 3; i ++)
    {
      if (this->children[i])
        gtk_widget_measure (this->children[i], this->orientation, for_size,
                            &sizes[i].minimum_size, &sizes[i].natural_size,
                            NULL, NULL);
    }

  if (this->center_widget)
    {
      int natural_size;

      avail = size - needed_spacing - (sizes[0].minimum_size + sizes[2].minimum_size);

      if (this->shrink_center_last)
        natural_size = sizes[1].natural_size;
      else
        natural_size = CLAMP (size - needed_spacing - (sizes[0].natural_size + sizes[2].natural_size), sizes[1].minimum_size, sizes[1].natural_size);

      center_size = CLAMP (avail, sizes[1].minimum_size, natural_size);
      center_expand = gtk_widget_compute_expand (this->center_widget, this->orientation);
    }

  if (this->start_widget)
    {
      avail = size - needed_spacing - (center_size + sizes[2].minimum_size);
      start_size = CLAMP (avail, sizes[0].minimum_size, sizes[0].natural_size);
      start_expand = gtk_widget_compute_expand (this->start_widget, this->orientation);
    }

   if (this->end_widget)
    {
      avail = size - needed_spacing - (center_size + sizes[0].minimum_size);
      end_size = CLAMP (avail, sizes[2].minimum_size, sizes[2].natural_size);
      end_expand = gtk_widget_compute_expand (this->end_widget, this->orientation);
    }

  if (this->center_widget)
    {
      int center_pos;

      center_pos = (size / 2) - (center_size / 2);

      /* Push in from start/end */
      if (start_size > 0 && start_size + spacing > center_pos)
        center_pos = start_size + spacing;
      else if (end_size > 0 && size - end_size - spacing < center_pos + center_size)
        center_pos = size - center_size - end_size - spacing;
      else if (center_expand)
        {
          center_size = size - 2 * (MAX (start_size, end_size) + spacing);
          center_pos = (size / 2) - (center_size / 2) + spacing;
        }

      if (start_expand)
        start_size = center_pos - spacing;

      if (end_expand)
        end_size = size - (center_pos + center_size) - spacing;
    }
  else
    {
      avail = size - needed_spacing - (start_size + end_size);
      if (start_expand && end_expand)
        {
          start_size += avail / 2;
          end_size += avail / 2;
        }
      else if (start_expand)
        {
          start_size += avail;
        }
      else if (end_expand)
        {
          end_size += avail;
        }
    }

  sizes[0].minimum_size = start_size;
  sizes[1].minimum_size = center_size;
  sizes[2].minimum_size = end_size;
}

static void
gtk_center_layout_measure_orientation (GtkCenterLayout *this,
                                       GtkWidget       *widget,
                                       GtkOrientation   orientation,
                                       int              for_size,
                                       int             *minimum,
                                       int             *natural,
                                       int             *minimum_baseline,
                                       int             *natural_baseline)
{
  int min[3];
  int nat[3];
  int n_visible_children = 0;
  int spacing;
  int i;

  spacing = get_spacing (this, gtk_widget_get_css_node (widget));

  for (i = 0; i < 3; i ++)
    {
      GtkWidget *child = this->children[i];

      if (child)
        {
          gtk_widget_measure (child,
                              orientation,
                              for_size,
                              &min[i], &nat[i], NULL, NULL);

          if (_gtk_widget_get_visible (child))
            n_visible_children ++;
        }
      else
        {
          min[i] = 0;
          nat[i] = 0;
        }
    }

  *minimum = min[0] + min[1] + min[2];
  *natural = nat[1] + 2 * MAX (nat[0], nat[2]);

  if (n_visible_children > 0)
    {
      *minimum += (n_visible_children - 1) * spacing;
      *natural += (n_visible_children - 1) * spacing;
    }
}

static void
gtk_center_layout_measure_opposite (GtkCenterLayout *this,
                                    GtkOrientation   orientation,
                                    int              for_size,
                                    int             *minimum,
                                    int             *natural,
                                    int             *minimum_baseline,
                                    int             *natural_baseline)
{
  int child_min, child_nat;
  int child_min_baseline, child_nat_baseline;
  int total_min, above_min, below_min;
  int total_nat, above_nat, below_nat;
  GtkWidget *child[3];
  GtkRequestedSize sizes[3];
  gboolean have_baseline = FALSE;
  gboolean align_baseline = FALSE;
  int i;

  child[0] = this->start_widget;
  child[1] = this->center_widget;
  child[2] = this->end_widget;

  if (for_size >= 0)
    gtk_center_layout_distribute (this, -1, for_size, 0, sizes);

  above_min = below_min = above_nat = below_nat = -1;
  total_min = total_nat = 0;

  for (i = 0; i < 3; i++)
    {
      if (child[i] == NULL)
        continue;

      gtk_widget_measure (child[i],
                          orientation,
                          for_size >= 0 ? sizes[i].minimum_size : -1,
                          &child_min, &child_nat,
                          &child_min_baseline, &child_nat_baseline);

      total_min = MAX (total_min, child_min);
      total_nat = MAX (total_nat, child_nat);

      if (orientation == GTK_ORIENTATION_VERTICAL && child_min_baseline >= 0)
        {
          have_baseline = TRUE;
          if (gtk_widget_get_valign (child[i]) == GTK_ALIGN_BASELINE_FILL ||
              gtk_widget_get_valign (child[i]) == GTK_ALIGN_BASELINE_CENTER)
            align_baseline = TRUE;

          below_min = MAX (below_min, child_min - child_min_baseline);
          above_min = MAX (above_min, child_min_baseline);
          below_nat = MAX (below_nat, child_nat - child_nat_baseline);
          above_nat = MAX (above_nat, child_nat_baseline);
        }
   }

  if (have_baseline)
    {
      int min_baseline = -1;
      int nat_baseline = -1;

      if (align_baseline)
        {
          total_min = MAX (total_min, above_min + below_min);
          total_nat = MAX (total_nat, above_nat + below_nat);
        }

      switch (this->baseline_pos)
        {
        case GTK_BASELINE_POSITION_TOP:
          min_baseline = above_min;
          nat_baseline = above_nat;
          break;
        case GTK_BASELINE_POSITION_CENTER:
          min_baseline = above_min + (total_min - (above_min + below_min)) / 2;
          nat_baseline = above_nat + (total_nat - (above_nat + below_nat)) / 2;
          break;
        case GTK_BASELINE_POSITION_BOTTOM:
          min_baseline = total_min - below_min;
          nat_baseline = total_nat - below_nat;
          break;
        default:
          break;
        }

      if (minimum_baseline)
        *minimum_baseline = min_baseline;
      if (natural_baseline)
        *natural_baseline = nat_baseline;
    }

  *minimum = total_min;
  *natural = total_nat;
}



static void
gtk_center_layout_measure (GtkLayoutManager *layout_manager,
                           GtkWidget        *widget,
                           GtkOrientation    orientation,
                           int               for_size,
                           int              *minimum,
                           int              *natural,
                           int              *minimum_baseline,
                           int              *natural_baseline)
{
  GtkCenterLayout *this = GTK_CENTER_LAYOUT (layout_manager);

  if (this->orientation == orientation)
    gtk_center_layout_measure_orientation (this, widget, orientation, for_size,
                                           minimum, natural, minimum_baseline, natural_baseline);
  else
    gtk_center_layout_measure_opposite (this, orientation, for_size,
                                        minimum, natural, minimum_baseline, natural_baseline);
}

static void
gtk_center_layout_allocate (GtkLayoutManager *layout_manager,
                            GtkWidget        *widget,
                            int               width,
                            int               height,
                            int               baseline)
{
  GtkCenterLayout *this = GTK_CENTER_LAYOUT (layout_manager);
  GtkWidget *child[3];
  int child_size[3];
  int child_pos[3];
  GtkRequestedSize sizes[3];
  int size;
  int for_size;
  int i;
  int spacing;

  spacing = get_spacing (this, gtk_widget_get_css_node (widget));

  if (this->orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      size = width;
      for_size = height;
    }
  else
    {
      size = height;
      for_size = width;
      baseline = -1;
    }

  /* Allocate child sizes */

  gtk_center_layout_distribute (this, for_size, size, spacing, sizes);

  child[1] = this->center_widget;
  child_size[1] = sizes[1].minimum_size;

  if (this->orientation == GTK_ORIENTATION_HORIZONTAL &&
      gtk_widget_get_direction (widget) == GTK_TEXT_DIR_RTL)
    {
      child[0] = this->end_widget;
      child[2] = this->start_widget;
      child_size[0] = sizes[2].minimum_size;
      child_size[2] = sizes[0].minimum_size;
    }
  else
    {
      child[0] = this->start_widget;
      child[2] = this->end_widget;
      child_size[0] = sizes[0].minimum_size;
      child_size[2] = sizes[2].minimum_size;
    }

  /* Determine baseline */
  if (this->orientation == GTK_ORIENTATION_HORIZONTAL &&
      baseline == -1)
    {
      int min_above, nat_above;
      int min_below, nat_below;
      gboolean have_baseline;

      have_baseline = FALSE;
      min_above = nat_above = 0;
      min_below = nat_below = 0;

      for (i = 0; i < 3; i++)
        {
          if (child[i] &&
              (gtk_widget_get_valign (child[i]) == GTK_ALIGN_BASELINE_FILL ||
               gtk_widget_get_valign (child[i]) == GTK_ALIGN_BASELINE_CENTER))
            {
              int child_min_height, child_nat_height;
              int child_min_baseline, child_nat_baseline;

              child_min_baseline = child_nat_baseline = -1;

              gtk_widget_measure (child[i], GTK_ORIENTATION_VERTICAL,
                                  child_size[i],
                                  &child_min_height, &child_nat_height,
                                  &child_min_baseline, &child_nat_baseline);

              if (child_min_baseline >= 0)
                {
                  have_baseline = TRUE;
                  min_below = MAX (min_below, child_min_height - child_min_baseline);
                  nat_below = MAX (nat_below, child_nat_height - child_nat_baseline);
                  min_above = MAX (min_above, child_min_baseline);
                  nat_above = MAX (nat_above, child_nat_baseline);
                }
            }
        }

      if (have_baseline)
        {
          /* TODO: This is purely based on the minimum baseline.
           * When things fit we should use the natural one
           */
          switch (this->baseline_pos)
            {
            default:
            case GTK_BASELINE_POSITION_TOP:
              baseline = min_above;
              break;
            case GTK_BASELINE_POSITION_CENTER:
              baseline = min_above + (height - (min_above + min_below)) / 2;
              break;
            case GTK_BASELINE_POSITION_BOTTOM:
              baseline = height - min_below;
              break;
            }
        }
    }

  /* Allocate child positions */

  child_pos[0] = 0;
  child_pos[1] = (size / 2) - (child_size[1] / 2);
  child_pos[2] = size - child_size[2];

  if (child[1])
    {
      /* Push in from start/end */
      if (child_size[0] > 0 && child_size[0] + spacing > child_pos[1])
        child_pos[1] = child_size[0] + spacing;
      else if (child_size[2] > 0 && size - child_size[2] - spacing < child_pos[1] + child_size[1])
        child_pos[1] = size - child_size[1] - child_size[2] - spacing;
    }

  for (i = 0; i < 3; i++)
    {
      GtkAllocation child_allocation;

      if (child[i] == NULL)
        continue;

      if (this->orientation == GTK_ORIENTATION_HORIZONTAL)
        {
          child_allocation.x = child_pos[i];
          child_allocation.y = 0;
          child_allocation.width = child_size[i];
          child_allocation.height = height;
        }
      else
        {
          child_allocation.x = 0;
          child_allocation.y = child_pos[i];
          child_allocation.width = width;
          child_allocation.height = child_size[i];
        }

      gtk_widget_size_allocate (child[i], &child_allocation, baseline);
    }
}

static void
gtk_center_layout_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtkCenterLayout *this = GTK_CENTER_LAYOUT (object);

  switch (prop_id)
    {
    case PROP_SHRINK_CENTER_LAST:
      gtk_center_layout_set_shrink_center_last (this, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_center_layout_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtkCenterLayout *this = GTK_CENTER_LAYOUT (object);

  switch (prop_id)
    {
    case PROP_SHRINK_CENTER_LAST:
      g_value_set_boolean (value, this->shrink_center_last);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_center_layout_dispose (GObject *object)
{
  GtkCenterLayout *this = GTK_CENTER_LAYOUT (object);

  g_clear_object (&this->start_widget);
  g_clear_object (&this->center_widget);
  g_clear_object (&this->end_widget);

  G_OBJECT_CLASS (gtk_center_layout_parent_class)->dispose (object);
}

static void
gtk_center_layout_class_init (GtkCenterLayoutClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkLayoutManagerClass *layout_class = GTK_LAYOUT_MANAGER_CLASS (klass);

  object_class->get_property = gtk_center_layout_get_property;
  object_class->set_property = gtk_center_layout_set_property;
  object_class->dispose = gtk_center_layout_dispose;

  layout_class->get_request_mode = gtk_center_layout_get_request_mode;
  layout_class->measure = gtk_center_layout_measure;
  layout_class->allocate = gtk_center_layout_allocate;

  /**
   * GtkCenterLayout:shrink-center-last:
   *
   * Whether to shrink the center widget after other children.
   *
   * By default, when there's no space to give all three children their
   * natural widths, the start and end widgets start shrinking and the
   * center child keeps natural width until they reach minimum width.
   *
   * If set to `FALSE`, start and end widgets keep natural width and the
   * center widget starts shrinking instead.
   *
   * Since: 4.12
   */
  props[PROP_SHRINK_CENTER_LAST] =
      g_param_spec_boolean ("shrink-center-last", NULL, NULL,
                            TRUE,
                            GTK_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, LAST_PROP, props);
}

static void
gtk_center_layout_init (GtkCenterLayout *this)
{
  this->orientation = GTK_ORIENTATION_HORIZONTAL;
  this->baseline_pos = GTK_BASELINE_POSITION_CENTER;
  this->shrink_center_last = TRUE;
}

/**
 * gtk_center_layout_new:
 *
 * Creates a new `GtkCenterLayout`.
 *
 * Returns: the newly created `GtkCenterLayout`
 */
GtkLayoutManager *
gtk_center_layout_new (void)
{
  return g_object_new (GTK_TYPE_CENTER_LAYOUT, NULL);
}

/**
 * gtk_center_layout_set_orientation:
 * @this: a `GtkCenterLayout`
 * @orientation: the new orientation
 *
 * Sets the orientation of @this.
 */
void
gtk_center_layout_set_orientation (GtkCenterLayout *this,
                                   GtkOrientation   orientation)
{
  g_return_if_fail (GTK_IS_CENTER_LAYOUT (this));

  if (orientation != this->orientation)
    {
      this->orientation = orientation;
      gtk_layout_manager_layout_changed (GTK_LAYOUT_MANAGER (this));
    }
}

/**
 * gtk_center_layout_get_orientation:
 * @this: a `GtkCenterLayout`
 *
 * Gets the current orienration of the layout manager.
 *
 * Returns: The current orientation of @this
 */
GtkOrientation
gtk_center_layout_get_orientation (GtkCenterLayout *this)
{
  g_return_val_if_fail (GTK_IS_CENTER_LAYOUT (this), GTK_ORIENTATION_HORIZONTAL);

  return this->orientation;
}

/**
 * gtk_center_layout_set_baseline_position:
 * @this: a `GtkCenterLayout`
 * @baseline_position: the new baseline position
 *
 * Sets the new baseline position of @this
 */
void
gtk_center_layout_set_baseline_position (GtkCenterLayout     *this,
                                         GtkBaselinePosition  baseline_position)
{
  g_return_if_fail (GTK_IS_CENTER_LAYOUT (this));

  if (baseline_position != this->baseline_pos)
    {
      this->baseline_pos = baseline_position;
      gtk_layout_manager_layout_changed (GTK_LAYOUT_MANAGER (this));
    }
}

/**
 * gtk_center_layout_get_baseline_position:
 * @this: a `GtkCenterLayout`
 *
 * Returns the baseline position of the layout.
 *
 * Returns: The current baseline position of @this.
 */
GtkBaselinePosition
gtk_center_layout_get_baseline_position (GtkCenterLayout *this)
{
  g_return_val_if_fail (GTK_IS_CENTER_LAYOUT (this), GTK_BASELINE_POSITION_TOP);

  return this->baseline_pos;
}

/**
 * gtk_center_layout_set_start_widget:
 * @this: a `GtkCenterLayout`
 * @widget: (nullable): the new start widget
 *
 * Sets the new start widget of @this.
 *
 * To remove the existing start widget, pass %NULL.
 */
void
gtk_center_layout_set_start_widget (GtkCenterLayout *this,
                                    GtkWidget       *widget)
{
  g_return_if_fail (GTK_IS_CENTER_LAYOUT (this));
  g_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));

  if (g_set_object (&this->start_widget, widget))
    gtk_layout_manager_layout_changed (GTK_LAYOUT_MANAGER (this));
}

/**
 * gtk_center_layout_get_start_widget:
 * @this: a `GtkCenterLayout`
 *
 * Returns the start widget of the layout.
 *
 * Returns: (nullable) (transfer none): The current start widget of @this
 */
GtkWidget *
gtk_center_layout_get_start_widget (GtkCenterLayout *this)
{
  g_return_val_if_fail (GTK_IS_CENTER_LAYOUT (this), NULL);

  return this->start_widget;
}

/**
 * gtk_center_layout_set_center_widget:
 * @this: a `GtkCenterLayout`
 * @widget: (nullable): the new center widget
 *
 * Sets the new center widget of @this.
 *
 * To remove the existing center widget, pass %NULL.
 */
void
gtk_center_layout_set_center_widget (GtkCenterLayout *this,
                                     GtkWidget       *widget)
{
  g_return_if_fail (GTK_IS_CENTER_LAYOUT (this));
  g_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));

  if (g_set_object (&this->center_widget, widget))
    gtk_layout_manager_layout_changed (GTK_LAYOUT_MANAGER (this));
}

/**
 * gtk_center_layout_get_center_widget:
 * @this: a `GtkCenterLayout`
 *
 * Returns the center widget of the layout.
 *
 * Returns: (nullable) (transfer none): the current center widget of @this
 */
GtkWidget *
gtk_center_layout_get_center_widget (GtkCenterLayout *this)
{
  g_return_val_if_fail (GTK_IS_CENTER_LAYOUT (this), NULL);

  return this->center_widget;
}

/**
 * gtk_center_layout_set_end_widget:
 * @this: a `GtkCenterLayout`
 * @widget: (nullable): the new end widget
 *
 * Sets the new end widget of @this.
 *
 * To remove the existing center widget, pass %NULL.
 */
void
gtk_center_layout_set_end_widget (GtkCenterLayout *this,
                                  GtkWidget       *widget)
{
  g_return_if_fail (GTK_IS_CENTER_LAYOUT (this));
  g_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));

  if (g_set_object (&this->end_widget, widget))
    gtk_layout_manager_layout_changed (GTK_LAYOUT_MANAGER (this));
}

/**
 * gtk_center_layout_get_end_widget:
 * @this: a `GtkCenterLayout`
 *
 * Returns the end widget of the layout.
 *
 * Returns: (nullable) (transfer none): the current end widget of @this
 */
GtkWidget *
gtk_center_layout_get_end_widget (GtkCenterLayout *this)
{
  g_return_val_if_fail (GTK_IS_CENTER_LAYOUT (this), NULL);

  return this->end_widget;
}

/**
 * gtk_center_layout_set_shrink_center_last:
 * @this: a `GtkCenterLayout`
 * @shrink_center_last: whether to shrink the center widget after others
 *
 * Sets whether to shrink the center widget after other children.
 *
 * By default, when there's no space to give all three children their
 * natural widths, the start and end widgets start shrinking and the
 * center child keeps natural width until they reach minimum width.
 *
 * If set to `FALSE`, start and end widgets keep natural width and the
 * center widget starts shrinking instead.
 *
 * Since: 4.12
 */
void
gtk_center_layout_set_shrink_center_last (GtkCenterLayout *this,
                                          gboolean         shrink_center_last)
{
  g_return_if_fail (GTK_IS_CENTER_LAYOUT (this));

  shrink_center_last = !!shrink_center_last;

  if (shrink_center_last == this->shrink_center_last)
    return;

  this->shrink_center_last = shrink_center_last;

  gtk_layout_manager_layout_changed (GTK_LAYOUT_MANAGER (this));

  g_object_notify_by_pspec (G_OBJECT (this), props[PROP_SHRINK_CENTER_LAST]);
}

/**
 * gtk_center_layout_get_shrink_center_last:
 * @this: a `GtkCenterLayout`
 *
 * Gets whether @this shrinks the center widget after other children.
 *
 * Returns: whether to shrink the center widget after others
 *
 * Since: 4.12
 */
gboolean
gtk_center_layout_get_shrink_center_last (GtkCenterLayout *this)
{
  g_return_val_if_fail (GTK_IS_CENTER_LAYOUT (this), FALSE);

  return this->shrink_center_last;
}
