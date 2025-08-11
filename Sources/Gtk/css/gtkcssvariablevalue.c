/*
 * Copyright (C) 2023 GNOME Foundation Inc.
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
 * Authors: Alice Mikhaylenko <alicem@gnome.org>
 */

#include "gtkcssvariablevalueprivate.h"

GtkCssVariableValue *
gtk_css_variable_value_new (GBytes                       *bytes,
                            gsize                         offset,
                            gsize                         end_offset,
                            gsize                         length,
                            GtkCssVariableValueReference *references,
                            gsize                         n_references)
{
  GtkCssVariableValue *this = g_new0 (GtkCssVariableValue, 1);

  this->ref_count = 1;

  this->bytes = g_bytes_ref (bytes);
  this->offset = offset;
  this->end_offset = end_offset;
  this->length = length;

  this->references = references;
  this->n_references = n_references;

  return this;
}

GtkCssVariableValue *
gtk_css_variable_value_new_initial (GBytes *bytes,
                                    gsize   offset,
                                    gsize   end_offset)
{
  GtkCssVariableValue *this = gtk_css_variable_value_new (bytes, offset, end_offset, 1, NULL, 0);

  this->is_invalid = TRUE;

  return this;
}

GtkCssVariableValue *
gtk_css_variable_value_ref (GtkCssVariableValue *this)
{
  this->ref_count++;

  return this;
}

void
gtk_css_variable_value_unref (GtkCssVariableValue *this)
{
  gsize i;

  this->ref_count--;
  if (this->ref_count > 0)
    return;

  g_bytes_unref (this->bytes);

  for (i = 0; i < this->n_references; i++)
    {
      GtkCssVariableValueReference *ref = &this->references[i];

      g_free (ref->name);
      if (ref->fallback)
        gtk_css_variable_value_unref (ref->fallback);
    }

  if (this->section)
    gtk_css_section_unref (this->section);

  g_free (this->references);
  g_free (this);
}

void
gtk_css_variable_value_print (GtkCssVariableValue *this,
                              GString             *string)
{
  gsize len = this->end_offset - this->offset;
  gconstpointer data = g_bytes_get_region (this->bytes, 1, this->offset, len);

  g_assert (data != NULL);

  g_string_append_len (string, (const char *) data, len);
}

char *
gtk_css_variable_value_to_string (GtkCssVariableValue *this)
{
  GString *string = g_string_new (NULL);
  gtk_css_variable_value_print (this, string);
  return g_string_free (string, FALSE);
}

gboolean
gtk_css_variable_value_equal (const GtkCssVariableValue *value1,
                              const GtkCssVariableValue *value2)
{
  if (value1 == value2)
    return TRUE;

  if (value1 == NULL || value2 == NULL)
    return FALSE;

  if (value1->bytes != value2->bytes)
    return FALSE;

  if (value1->offset != value2->offset)
    return FALSE;

  if (value1->end_offset != value2->end_offset)
    return FALSE;

  return TRUE;
}

GtkCssVariableValue *
gtk_css_variable_value_transition  (GtkCssVariableValue *start,
                                    GtkCssVariableValue *end,
                                    double               progress)
{
  GtkCssVariableValue *ret = progress < 0.5 ? start : end;

  if (ret == NULL)
    return NULL;

  return gtk_css_variable_value_ref (ret);
}

void
gtk_css_variable_value_set_section (GtkCssVariableValue *this,
                                    GtkCssSection       *section)
{
  this->section = gtk_css_section_ref (section);
}

void
gtk_css_variable_value_taint (GtkCssVariableValue *this)
{
  this->is_animation_tainted = TRUE;
}
