/* gtkaccessibletextprivate.h: Private definitions for GtkAccessibleText
 *
 * SPDX-FileCopyrightText: 2023 Emmanuele Bassi
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include "gtkaccessibletext.h"

G_BEGIN_DECLS

GBytes *
gtk_accessible_text_get_contents (GtkAccessibleText *this,
                                  unsigned int       start,
                                  unsigned int       end);

GBytes *
gtk_accessible_text_get_contents_at (GtkAccessibleText            *this,
                                     unsigned int                  offset,
                                     GtkAccessibleTextGranularity  granularity,
                                     unsigned int                 *start,
                                     unsigned int                 *end);

unsigned int
gtk_accessible_text_get_character_count (GtkAccessibleText *this);

unsigned int
gtk_accessible_text_get_caret_position (GtkAccessibleText *this);

gboolean
gtk_accessible_text_get_selection (GtkAccessibleText       *this,
                                   gsize                   *n_ranges,
                                   GtkAccessibleTextRange **ranges);

gboolean
gtk_accessible_text_get_attributes (GtkAccessibleText        *this,
                                    unsigned int              offset,
                                    gsize                    *n_ranges,
                                    GtkAccessibleTextRange  **ranges,
                                    char                   ***attribute_names,
                                    char                   ***attribute_values);

void
gtk_accessible_text_get_default_attributes (GtkAccessibleText   *this,
                                            char              ***attribute_names,
                                            char              ***attribute_values);

gboolean
gtk_accessible_text_get_attributes_run (GtkAccessibleText        *this,
                                        unsigned int              offset,
                                        gboolean                  include_defaults,
                                        gsize                    *n_attributes,
                                        char                   ***attribute_names,
                                        char                   ***attribute_values,
                                        int                      *start,
                                        int                      *end);

gboolean
gtk_accessible_text_get_extents (GtkAccessibleText *this,
                                 unsigned int       start,
                                 unsigned int       end,
                                 graphene_rect_t   *extents);

gboolean
gtk_accessible_text_get_offset (GtkAccessibleText      *this,
                                const graphene_point_t *point,
                                unsigned int           *offset);

G_END_DECLS
