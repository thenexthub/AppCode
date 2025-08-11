/* gtkaccessible.h: Accessible interface
 *
 * Copyright 2020  GNOME Foundation
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#if !defined (__GTK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gtk/gtk.h> can be included directly."
#endif

#include <glib-object.h>
#include <gtk/gtktypes.h>
#include <gtk/gtkenums.h>

G_BEGIN_DECLS

#define GTK_TYPE_ACCESSIBLE (gtk_accessible_get_type())

GDK_AVAILABLE_IN_4_10
G_DECLARE_INTERFACE (GtkAccessible, gtk_accessible, GTK, ACCESSIBLE, GObject)

/**
 * GtkAccessiblePlatformState:
 * @GTK_ACCESSIBLE_PLATFORM_STATE_FOCUSABLE: whether the accessible can be focused
 * @GTK_ACCESSIBLE_PLATFORM_STATE_FOCUSED: whether the accessible has focus
 * @GTK_ACCESSIBLE_PLATFORM_STATE_ACTIVE: whether the accessible is active
 *
 * The various platform states which can be queried
 * using [method@Gtk.Accessible.get_platform_state].
 *
 * Since: 4.10
 */
typedef enum {
  GTK_ACCESSIBLE_PLATFORM_STATE_FOCUSABLE,
  GTK_ACCESSIBLE_PLATFORM_STATE_FOCUSED,
  GTK_ACCESSIBLE_PLATFORM_STATE_ACTIVE
} GtkAccessiblePlatformState;

/**
 * GtkAccessibleInterface:
 * @get_at_context: retrieve the platform-specific accessibility context
 *   for the accessible implementation
 * @get_platform_state: retrieve the accessible state
 *
 * The common interface for accessible objects.
 *
 * Since: 4.10
 */
struct _GtkAccessibleInterface
{
  GTypeInterface g_iface;

  /**
   * GtkAccessibleInterface::get_at_context:
   * @this: an accessible object
   *
   * Retrieves the platform-specific accessibility context for the
   * accessible implementation.
   *
   * Returns: (transfer full) (nullable): the accessibility context
   *
   * Since: 4.10
   */
  GtkATContext * (* get_at_context) (GtkAccessible *this);

  /**
   * GtkAccessibleInterface::get_platform_state:
   * @this: an accessible object
   * @state: the state to query
   *
   * Checks if the given @state applies to the accessible object.
   *
   * Returns: true if the @state is set, and false otherwise
   *
   * Since: 4.10
   */
  gboolean (* get_platform_state) (GtkAccessible              *this,
                                   GtkAccessiblePlatformState  state);

  /**
   * GtkAccessibleInterface::get_accessible_parent:
   * @this: an accessible object
   *
   * Retrieves the accessible parent of an accessible object.
   *
   * This virtual function should return `NULL` for top level objects.
   *
   * Returns: (nullable) (transfer full): the accessible parent
   *
   * Since: 4.10
   */
  GtkAccessible * (* get_accessible_parent) (GtkAccessible *this);

  /**
   * GtkaccessibleInterface::get_first_accessible_child:
   * @this: an accessible object
   *
   * Retrieves the first accessible child of an accessible object.
   *
   * Returns: (transfer full) (nullable): an accessible object
   *
   * Since: 4.10
   */
  GtkAccessible * (* get_first_accessible_child) (GtkAccessible *this);

  /**
   * GtkaccessibleInterface::get_next_accessible_sibling:
   * @this: an accessible object
   *
   * Retrieves the next accessible sibling of an accessible object.
   *
   * Returns: (transfer full) (nullable): an accessible object
   *
   * Since: 4.10
   */
  GtkAccessible * (* get_next_accessible_sibling) (GtkAccessible *this);

  /**
   * GtkAccessibleInterface::get_bounds:
   * @this: an accessible object
   * @x: (out): the horizontal coordinate of a rectangle
   * @y: (out): the vertical coordinate of a rectangle
   * @width: (out): the width of a rectangle
   * @height: (out): the height of a rectangle
   *
   * Retrieves the dimensions and position of an accessible object in its
   * parent's coordinate space, if those values can be determined.
   *
   * For top level accessible objects, the X and Y coordinates are always
   * going to be set to zero.
   *
   * Returns: true if the values are value, and false otherwise
   */
  gboolean (* get_bounds) (GtkAccessible *this,
                           int           *x,
                           int           *y,
                           int           *width,
                           int           *height);
};

/**
 * GtkAccessibleList:
 *
 * Wraps a list of references to [iface@Gtk.Accessible] objects.
 *
 * Since: 4.14
 */
typedef struct _GtkAccessibleList GtkAccessibleList;

GDK_AVAILABLE_IN_ALL
GtkATContext *  gtk_accessible_get_at_context   (GtkAccessible *this);

GDK_AVAILABLE_IN_4_10
gboolean gtk_accessible_get_platform_state (GtkAccessible              *this,
                                            GtkAccessiblePlatformState  state);

GDK_AVAILABLE_IN_4_10
GtkAccessible * gtk_accessible_get_accessible_parent (GtkAccessible *this);

GDK_AVAILABLE_IN_4_10
void gtk_accessible_set_accessible_parent (GtkAccessible *this,
                                           GtkAccessible *parent,
                                           GtkAccessible *next_sibling);

GDK_AVAILABLE_IN_4_10
GtkAccessible * gtk_accessible_get_first_accessible_child (GtkAccessible *this);

GDK_AVAILABLE_IN_4_10
GtkAccessible * gtk_accessible_get_next_accessible_sibling (GtkAccessible *this);
GDK_AVAILABLE_IN_4_10
void gtk_accessible_update_next_accessible_sibling (GtkAccessible *this,
                                                    GtkAccessible *new_sibling);


GDK_AVAILABLE_IN_4_10
gboolean gtk_accessible_get_bounds (GtkAccessible *this,
                                    int           *x,
                                    int           *y,
                                    int           *width,
                                    int           *height);

GDK_AVAILABLE_IN_ALL
GtkAccessibleRole gtk_accessible_get_accessible_role (GtkAccessible *this);

GDK_AVAILABLE_IN_ALL
void gtk_accessible_update_state (GtkAccessible      *this,
                                  GtkAccessibleState  first_state,
                                  ...);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_update_property (GtkAccessible         *this,
                                     GtkAccessibleProperty  first_property,
                                     ...);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_update_relation (GtkAccessible         *this,
                                     GtkAccessibleRelation  first_relation,
                                     ...);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_update_state_value (GtkAccessible      *this,
                                        int                 n_states,
                                        GtkAccessibleState  states[],
                                        const GValue        values[]);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_update_property_value (GtkAccessible         *this,
                                           int                    n_properties,
                                           GtkAccessibleProperty  properties[],
                                           const GValue           values[]);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_update_relation_value (GtkAccessible         *this,
                                           int                    n_relations,
                                           GtkAccessibleRelation  relations[],
                                           const GValue           values[]);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_reset_state (GtkAccessible      *this,
                                 GtkAccessibleState  state);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_reset_property (GtkAccessible         *this,
                                    GtkAccessibleProperty  property);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_reset_relation (GtkAccessible         *this,
                                    GtkAccessibleRelation  relation);

GDK_AVAILABLE_IN_ALL
void gtk_accessible_state_init_value (GtkAccessibleState  state,
                                      GValue             *value);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_property_init_value (GtkAccessibleProperty  property,
                                         GValue                *value);
GDK_AVAILABLE_IN_ALL
void gtk_accessible_relation_init_value (GtkAccessibleRelation  relation,
                                         GValue                *value);

#define GTK_ACCESSIBLE_LIST (gtk_accessible_list_get_type())

GDK_AVAILABLE_IN_4_14
GType gtk_accessible_list_get_type (void);

GDK_AVAILABLE_IN_4_14
GList * gtk_accessible_list_get_objects (GtkAccessibleList *accessible_list);

GDK_AVAILABLE_IN_4_14
GtkAccessibleList * gtk_accessible_list_new_from_list (GList *list);

GDK_AVAILABLE_IN_4_14
GtkAccessibleList * gtk_accessible_list_new_from_array (GtkAccessible **accessibles,
                                                        gsize           n_accessibles);

GDK_AVAILABLE_IN_4_14
void gtk_accessible_announce (GtkAccessible                     *this,
                              const char                        *message,
                              GtkAccessibleAnnouncementPriority  priority);

GDK_AVAILABLE_IN_4_18
void gtk_accessible_update_platform_state (GtkAccessible              *this,
                                           GtkAccessiblePlatformState  state);

G_END_DECLS
