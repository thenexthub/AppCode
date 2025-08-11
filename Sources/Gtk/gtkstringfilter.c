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

#include "gtkstringfilter.h"

#include "gtktypebuiltins.h"

/**
 * GtkStringFilter:
 *
 * Determines whether to include items by comparing strings to a fixed search term.
 *
 * The strings are obtained from the items by evaluating an expression
 * set with [method@Gtk.StringFilter.set_expression], and they are
 * compared against a search term set with [method@Gtk.StringFilter.set_search].
 *
 * `GtkStringFilter` has several different modes of comparison - it
 * can match the whole string, just a prefix, or any substring. Use
 * [method@Gtk.StringFilter.set_match_mode] choose a mode.
 *
 * It is also possible to make case-insensitive comparisons, with
 * [method@Gtk.StringFilter.set_ignore_case].
 */

struct _GtkStringFilter
{
  GtkFilter parent_instance;

  char *search;
  char *search_prepared;

  gboolean ignore_case;
  GtkStringFilterMatchMode match_mode;

  GtkExpression *expression;
};

enum {
  PROP_0,
  PROP_EXPRESSION,
  PROP_IGNORE_CASE,
  PROP_MATCH_MODE,
  PROP_SEARCH,
  NUM_PROPERTIES
};

G_DEFINE_TYPE (GtkStringFilter, gtk_string_filter, GTK_TYPE_FILTER)

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

static char *
gtk_string_filter_prepare (GtkStringFilter *this,
                           const char      *s)
{
  char *tmp;
  char *result;

  if (s == NULL || s[0] == '\0')
    return NULL;

  tmp = g_utf8_normalize (s, -1, G_NORMALIZE_ALL);

  if (!this->ignore_case)
    return tmp;

  result = g_utf8_casefold (tmp, -1);
  g_free (tmp);

  return result;
}

/* This is necessary because code just looks at this->search otherwise
 * and that can be the empty string...
 */
static gboolean
gtk_string_filter_has_search (GtkStringFilter *this)
{
  return this->search_prepared != NULL;
}

static gboolean
gtk_string_filter_match (GtkFilter *filter,
                         gpointer   item)
{
  GtkStringFilter *this = GTK_STRING_FILTER (filter);
  GValue value = G_VALUE_INIT;
  char *prepared;
  const char *s;
  gboolean result;

  if (!gtk_string_filter_has_search (this))
    return TRUE;

  if (this->expression == NULL ||
      !gtk_expression_evaluate (this->expression, item, &value))
    return FALSE;
  s = g_value_get_string (&value);
  prepared = gtk_string_filter_prepare (this, s);
  if (prepared == NULL)
    return FALSE;

  switch (this->match_mode)
    {
    case GTK_STRING_FILTER_MATCH_MODE_EXACT:
      result = strcmp (prepared, this->search_prepared) == 0;
      break;
    case GTK_STRING_FILTER_MATCH_MODE_SUBSTRING:
      result = strstr (prepared, this->search_prepared) != NULL;
      break;
    case GTK_STRING_FILTER_MATCH_MODE_PREFIX:
      result = g_str_has_prefix (prepared, this->search_prepared);
      break;
    default:
      g_assert_not_reached ();
    }

#if 0
  g_print ("%s (%s) %s %s (%s)\n", s, prepared, result ? "==" : "!=", this->search, this->search_prepared);
#endif

  g_free (prepared);
  g_value_unset (&value);

  return result;
}

static GtkFilterMatch
gtk_string_filter_get_strictness (GtkFilter *filter)
{
  GtkStringFilter *this = GTK_STRING_FILTER (filter);

  if (!gtk_string_filter_has_search (this))
    return GTK_FILTER_MATCH_ALL;

  if (this->expression == NULL)
    return GTK_FILTER_MATCH_NONE;

  return GTK_FILTER_MATCH_SOME;
}

static void
gtk_string_filter_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtkStringFilter *this = GTK_STRING_FILTER (object);

  switch (prop_id)
    {
    case PROP_EXPRESSION:
      gtk_string_filter_set_expression (this, gtk_value_get_expression (value));
      break;

    case PROP_IGNORE_CASE:
      gtk_string_filter_set_ignore_case (this, g_value_get_boolean (value));
      break;

    case PROP_MATCH_MODE:
      gtk_string_filter_set_match_mode (this, g_value_get_enum (value));
      break;

    case PROP_SEARCH:
      gtk_string_filter_set_search (this, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
gtk_string_filter_get_property (GObject     *object,
                                guint        prop_id,
                                GValue      *value,
                                GParamSpec  *pspec)
{
  GtkStringFilter *this = GTK_STRING_FILTER (object);

  switch (prop_id)
    {
    case PROP_EXPRESSION:
      gtk_value_set_expression (value, this->expression);
      break;

    case PROP_IGNORE_CASE:
      g_value_set_boolean (value, this->ignore_case);
      break;

    case PROP_MATCH_MODE:
      g_value_set_enum (value, this->match_mode);
      break;

    case PROP_SEARCH:
      g_value_set_string (value, this->search);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_string_filter_dispose (GObject *object)
{
  GtkStringFilter *this = GTK_STRING_FILTER (object);

  g_clear_pointer (&this->search, g_free);
  g_clear_pointer (&this->search_prepared, g_free);
  g_clear_pointer (&this->expression, gtk_expression_unref);

  G_OBJECT_CLASS (gtk_string_filter_parent_class)->dispose (object);
}

static void
gtk_string_filter_class_init (GtkStringFilterClass *class)
{
  GtkFilterClass *filter_class = GTK_FILTER_CLASS (class);
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  filter_class->match = gtk_string_filter_match;
  filter_class->get_strictness = gtk_string_filter_get_strictness;

  object_class->get_property = gtk_string_filter_get_property;
  object_class->set_property = gtk_string_filter_set_property;
  object_class->dispose = gtk_string_filter_dispose;

  /**
   * GtkStringFilter:expression: (type GtkExpression)
   *
   * The expression to evaluate on each item to get a
   * string to compare with.
   */
  properties[PROP_EXPRESSION] =
    gtk_param_spec_expression ("expression", NULL, NULL,
                               G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkStringFilter:ignore-case:
   *
   * If matching is case sensitive.
   */
  properties[PROP_IGNORE_CASE] =
      g_param_spec_boolean ("ignore-case", NULL, NULL,
                            TRUE,
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkStringFilter:match-mode:
   *
   * If exact matches are necessary or if substrings are allowed.
   */
  properties[PROP_MATCH_MODE] =
      g_param_spec_enum ("match-mode", NULL, NULL,
                         GTK_TYPE_STRING_FILTER_MATCH_MODE,
                         GTK_STRING_FILTER_MATCH_MODE_SUBSTRING,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkStringFilter:search:
   *
   * The search term.
   */
  properties[PROP_SEARCH] =
      g_param_spec_string ("search", NULL, NULL,
                           NULL,
                           G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, NUM_PROPERTIES, properties);

}

static void
gtk_string_filter_init (GtkStringFilter *this)
{
  this->ignore_case = TRUE;
  this->match_mode = GTK_STRING_FILTER_MATCH_MODE_SUBSTRING;
}

/**
 * gtk_string_filter_new:
 * @expression: (transfer full) (nullable): the expression to evaluate
 *
 * Creates a new string filter.
 *
 * You will want to set up the filter by providing a string to search for
 * and by providing a property to look up on the item.
 *
 * Returns: a new `GtkStringFilter`
 */
GtkStringFilter *
gtk_string_filter_new (GtkExpression *expression)
{
  GtkStringFilter *result;

  result = g_object_new (GTK_TYPE_STRING_FILTER,
                         "expression", expression,
                         NULL);

  g_clear_pointer (&expression, gtk_expression_unref);

  return result;
}

/**
 * gtk_string_filter_get_search:
 * @this: a string filter
 *
 * Gets the search term.
 *
 * Returns: (nullable) (transfer none): the search term
 **/
const char *
gtk_string_filter_get_search (GtkStringFilter *this)
{
  g_return_val_if_fail (GTK_IS_STRING_FILTER (this), NULL);

  return this->search;
}

/**
 * gtk_string_filter_set_search:
 * @this: a string filter
 * @search: (transfer none) (nullable): the string to search for
 *
 * Sets the string to search for.
 */
void
gtk_string_filter_set_search (GtkStringFilter *this,
                              const char      *search)
{
  GtkFilterChange change;

  g_return_if_fail (GTK_IS_STRING_FILTER (this));

  if (g_strcmp0 (this->search, search) == 0)
    return;

  if (search == NULL || search[0] == 0)
    change = GTK_FILTER_CHANGE_LESS_STRICT;
  else if (!gtk_string_filter_has_search (this))
    change = GTK_FILTER_CHANGE_MORE_STRICT;
  else if (g_str_has_prefix (search, this->search))
    change = GTK_FILTER_CHANGE_MORE_STRICT;
  else if (g_str_has_prefix (this->search, search))
    change = GTK_FILTER_CHANGE_LESS_STRICT;
  else
    change = GTK_FILTER_CHANGE_DIFFERENT;

  g_free (this->search);
  g_free (this->search_prepared);

  this->search = g_strdup (search);
  this->search_prepared = gtk_string_filter_prepare (this, search);

  gtk_filter_changed (GTK_FILTER (this), change);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_SEARCH]);
}

/**
 * gtk_string_filter_get_expression:
 * @this: a string filter
 *
 * Gets the expression that the string filter uses to
 * obtain strings from items.
 *
 * Returns: (transfer none) (nullable): the expression 
 */
GtkExpression *
gtk_string_filter_get_expression (GtkStringFilter *this)
{
  g_return_val_if_fail (GTK_IS_STRING_FILTER (this), NULL);

  return this->expression;
}

/**
 * gtk_string_filter_set_expression:
 * @this: a string filter
 * @expression: (nullable): the expression
 *
 * Sets the expression that the string filter uses to
 * obtain strings from items.
 *
 * The expression must have a value type of `G_TYPE_STRING`.
 */
void
gtk_string_filter_set_expression (GtkStringFilter *this,
                                  GtkExpression   *expression)
{
  g_return_if_fail (GTK_IS_STRING_FILTER (this));
  g_return_if_fail (expression == NULL || gtk_expression_get_value_type (expression) == G_TYPE_STRING);

  if (this->expression == expression)
    return;

  g_clear_pointer (&this->expression, gtk_expression_unref);
  this->expression = gtk_expression_ref (expression);

  if (gtk_string_filter_has_search (this))
    gtk_filter_changed (GTK_FILTER (this), GTK_FILTER_CHANGE_DIFFERENT);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_EXPRESSION]);
}

/**
 * gtk_string_filter_get_ignore_case:
 * @this: a string filter
 *
 * Returns whether the filter ignores case differences.
 *
 * Returns: true if the filter ignores case
 */
gboolean
gtk_string_filter_get_ignore_case (GtkStringFilter *this)
{
  g_return_val_if_fail (GTK_IS_STRING_FILTER (this), TRUE);

  return this->ignore_case;
}

/**
 * gtk_string_filter_set_ignore_case:
 * @this: a string filter
 * @ignore_case: true to ignore case
 *
 * Sets whether the filter ignores case differences.
 */
void
gtk_string_filter_set_ignore_case (GtkStringFilter *this,
                                   gboolean         ignore_case)
{
  g_return_if_fail (GTK_IS_STRING_FILTER (this));

  if (this->ignore_case == ignore_case)
    return;

  this->ignore_case = ignore_case;

  if (this->search)
    {
      g_free (this->search_prepared);
      this->search_prepared = gtk_string_filter_prepare (this, this->search);
      gtk_filter_changed (GTK_FILTER (this), ignore_case ? GTK_FILTER_CHANGE_LESS_STRICT : GTK_FILTER_CHANGE_MORE_STRICT);
    }

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_IGNORE_CASE]);
}

/**
 * gtk_string_filter_get_match_mode:
 * @this: a string filter
 *
 * Returns the match mode that the filter is using.
 *
 * Returns: the match mode of the filter
 */
GtkStringFilterMatchMode
gtk_string_filter_get_match_mode (GtkStringFilter *this)
{
  g_return_val_if_fail (GTK_IS_STRING_FILTER (this), GTK_STRING_FILTER_MATCH_MODE_EXACT);

  return this->match_mode;
}

/**
 * gtk_string_filter_set_match_mode:
 * @this: a string filter
 * @mode: the new match mode
 *
 * Sets the match mode for the filter.
 */
void
gtk_string_filter_set_match_mode (GtkStringFilter *this,
                                  GtkStringFilterMatchMode mode)
{
  GtkStringFilterMatchMode old_mode;

  g_return_if_fail (GTK_IS_STRING_FILTER (this));

  if (this->match_mode == mode)
    return;

  old_mode = this->match_mode;
  this->match_mode = mode;

  if (this->search_prepared && this->expression)
    {
      switch (old_mode)
        {
        case GTK_STRING_FILTER_MATCH_MODE_EXACT:
          gtk_filter_changed (GTK_FILTER (this), GTK_FILTER_CHANGE_LESS_STRICT);
          break;

        case GTK_STRING_FILTER_MATCH_MODE_SUBSTRING:
          gtk_filter_changed (GTK_FILTER (this), GTK_FILTER_CHANGE_MORE_STRICT);
          break;

        case GTK_STRING_FILTER_MATCH_MODE_PREFIX:
          if (mode == GTK_STRING_FILTER_MATCH_MODE_SUBSTRING)
            gtk_filter_changed (GTK_FILTER (this), GTK_FILTER_CHANGE_LESS_STRICT);
          else
            gtk_filter_changed (GTK_FILTER (this), GTK_FILTER_CHANGE_MORE_STRICT);
          break;

        default:
          g_assert_not_reached ();
          break;
        }
    }

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_MATCH_MODE]);
}
