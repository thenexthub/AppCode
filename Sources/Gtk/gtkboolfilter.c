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

#include "gtkboolfilter.h"

#include "gtktypebuiltins.h"

/**
 * GtkBoolFilter:
 *
 * Evaluates a boolean expression to determine whether to include items.
 */

struct _GtkBoolFilter
{
  GtkFilter parent_instance;

  gboolean invert;
  GtkExpression *expression;
};

enum {
  PROP_0,
  PROP_EXPRESSION,
  PROP_INVERT,
  NUM_PROPERTIES
};

G_DEFINE_TYPE (GtkBoolFilter, gtk_bool_filter, GTK_TYPE_FILTER)

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static gboolean
gtk_bool_filter_match (GtkFilter *filter,
                       gpointer   item)
{
  GtkBoolFilter *this = GTK_BOOL_FILTER (filter);
  GValue value = G_VALUE_INIT;
  gboolean result;

  if (this->expression == NULL ||
      !gtk_expression_evaluate (this->expression, item, &value))
    return FALSE;
  result = g_value_get_boolean (&value);

  g_value_unset (&value);

  if (this->invert)
    result = !result;

  return result;
}

static GtkFilterMatch
gtk_bool_filter_get_strictness (GtkFilter *filter)
{
  GtkBoolFilter *this = GTK_BOOL_FILTER (filter);

  if (this->expression == NULL)
    return GTK_FILTER_MATCH_NONE;

  return GTK_FILTER_MATCH_SOME;
}

static void
gtk_bool_filter_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GtkBoolFilter *this = GTK_BOOL_FILTER (object);

  switch (prop_id)
    {
    case PROP_EXPRESSION:
      gtk_bool_filter_set_expression (this, gtk_value_get_expression (value));
      break;

    case PROP_INVERT:
      gtk_bool_filter_set_invert (this, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
gtk_bool_filter_get_property (GObject     *object,
                              guint        prop_id,
                              GValue      *value,
                              GParamSpec  *pspec)
{
  GtkBoolFilter *this = GTK_BOOL_FILTER (object);

  switch (prop_id)
    {
    case PROP_EXPRESSION:
      gtk_value_set_expression (value, this->expression);
      break;

    case PROP_INVERT:
      g_value_set_boolean (value, this->invert);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_bool_filter_dispose (GObject *object)
{
  GtkBoolFilter *this = GTK_BOOL_FILTER (object);

  g_clear_pointer (&this->expression, gtk_expression_unref);

  G_OBJECT_CLASS (gtk_bool_filter_parent_class)->dispose (object);
}

static void
gtk_bool_filter_class_init (GtkBoolFilterClass *class)
{
  GtkFilterClass *filter_class = GTK_FILTER_CLASS (class);
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  filter_class->match = gtk_bool_filter_match;
  filter_class->get_strictness = gtk_bool_filter_get_strictness;

  object_class->get_property = gtk_bool_filter_get_property;
  object_class->set_property = gtk_bool_filter_set_property;
  object_class->dispose = gtk_bool_filter_dispose;

  /**
   * GtkBoolFilter:expression: (type GtkExpression)
   *
   * The boolean expression to evaluate on each item.
   */
  properties[PROP_EXPRESSION] =
    gtk_param_spec_expression ("expression", NULL, NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkBoolFilter:invert:
   *
   * If the expression result should be inverted.
   */
  properties[PROP_INVERT] =
      g_param_spec_boolean ("invert", NULL, NULL,
                            FALSE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, NUM_PROPERTIES, properties);
}

static void
gtk_bool_filter_init (GtkBoolFilter *this)
{
}

/**
 * gtk_bool_filter_new:
 * @expression: (transfer full) (nullable): the expression to evaluate
 *
 * Creates a new bool filter.
 *
 * Returns: a new `GtkBoolFilter`
 */
GtkBoolFilter *
gtk_bool_filter_new (GtkExpression *expression)
{
  GtkBoolFilter *result;

  result = g_object_new (GTK_TYPE_BOOL_FILTER,
                         "expression", expression,
                         NULL);

  g_clear_pointer (&expression, gtk_expression_unref);

  return result;
}

/**
 * gtk_bool_filter_get_expression:
 * @this: a bool filter
 *
 * Gets the expression that the filter evaluates for
 * each item.
 *
 * Returns: (transfer none) (nullable): the expression
 */
GtkExpression *
gtk_bool_filter_get_expression (GtkBoolFilter *this)
{
  g_return_val_if_fail (GTK_IS_BOOL_FILTER (this), NULL);

  return this->expression;
}

/**
 * gtk_bool_filter_set_expression:
 * @this: a bool filter
 * @expression: (nullable): the expression
 *
 * Sets the expression that the filter uses to check if items
 * should be filtered.
 *
 * The expression must have a value type of `G_TYPE_BOOLEAN`.
 */
void
gtk_bool_filter_set_expression (GtkBoolFilter *this,
                                GtkExpression *expression)
{
  g_return_if_fail (GTK_IS_BOOL_FILTER (this));
  g_return_if_fail (expression == NULL || gtk_expression_get_value_type (expression) == G_TYPE_BOOLEAN);

  if (this->expression == expression)
    return;

  g_clear_pointer (&this->expression, gtk_expression_unref);
  if (expression)
    this->expression = gtk_expression_ref (expression);

  gtk_filter_changed (GTK_FILTER (this), GTK_FILTER_CHANGE_DIFFERENT);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_EXPRESSION]);
}

/**
 * gtk_bool_filter_get_invert:
 * @this: a bool filter
 *
 * Returns whether the filter inverts the expression.
 *
 * Returns: true if the filter inverts
 */
gboolean
gtk_bool_filter_get_invert (GtkBoolFilter *this)
{
  g_return_val_if_fail (GTK_IS_BOOL_FILTER (this), TRUE);

  return this->invert;
}

/**
 * gtk_bool_filter_set_invert:
 * @this: a bool filter
 * @invert: true to invert
 *
 * Sets whether the filter should invert the expression.
 */
void
gtk_bool_filter_set_invert (GtkBoolFilter *this,
                            gboolean       invert)
{
  g_return_if_fail (GTK_IS_BOOL_FILTER (this));

  if (this->invert == invert)
    return;

  this->invert = invert;

  gtk_filter_changed (GTK_FILTER (this), GTK_FILTER_CHANGE_DIFFERENT);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_INVERT]);
}

