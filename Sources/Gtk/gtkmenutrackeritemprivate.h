/*
 * Copyright © 2011, 2013 Canonical Limited
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Ryan Lortie <desrt@desrt.ca>
 */

#pragma once

#include "gtkactionobservableprivate.h"

#define GTK_TYPE_MENU_TRACKER_ITEM                          (gtk_menu_tracker_item_get_type ())
#define GTK_MENU_TRACKER_ITEM(inst)                         (G_TYPE_CHECK_INSTANCE_CAST ((inst), \
                                                             GTK_TYPE_MENU_TRACKER_ITEM, GtkMenuTrackerItem))
#define GTK_IS_MENU_TRACKER_ITEM(inst)                      (G_TYPE_CHECK_INSTANCE_TYPE ((inst), \
                                                             GTK_TYPE_MENU_TRACKER_ITEM))

typedef struct _GtkMenuTrackerItem GtkMenuTrackerItem;

#define GTK_TYPE_MENU_TRACKER_ITEM_ROLE                     (gtk_menu_tracker_item_role_get_type ())

typedef enum  {
  GTK_MENU_TRACKER_ITEM_ROLE_NORMAL,
  GTK_MENU_TRACKER_ITEM_ROLE_CHECK,
  GTK_MENU_TRACKER_ITEM_ROLE_RADIO,
} GtkMenuTrackerItemRole;

GType                   gtk_menu_tracker_item_get_type                  (void) G_GNUC_CONST;

GType                   gtk_menu_tracker_item_role_get_type             (void) G_GNUC_CONST;

GtkMenuTrackerItem *   _gtk_menu_tracker_item_new                       (GtkActionObservable *observable,
                                                                         GMenuModel          *model,
                                                                         int                  item_index,
                                                                         gboolean             mac_os_mode,
                                                                         const char          *action_namespace,
                                                                         gboolean             is_separator);

const char *           gtk_menu_tracker_item_get_action_name           (GtkMenuTrackerItem *this);

const char *           gtk_menu_tracker_item_get_special               (GtkMenuTrackerItem *this);

const char *           gtk_menu_tracker_item_get_custom                (GtkMenuTrackerItem *this);

const char *           gtk_menu_tracker_item_get_display_hint          (GtkMenuTrackerItem *this);

const char *           gtk_menu_tracker_item_get_text_direction        (GtkMenuTrackerItem *this);

GtkActionObservable *  _gtk_menu_tracker_item_get_observable            (GtkMenuTrackerItem *this);

gboolean                gtk_menu_tracker_item_get_is_separator          (GtkMenuTrackerItem *this);

gboolean                gtk_menu_tracker_item_get_has_link              (GtkMenuTrackerItem *this,
                                                                         const char         *link_name);

const char *           gtk_menu_tracker_item_get_label                 (GtkMenuTrackerItem *this);

gboolean               gtk_menu_tracker_item_get_use_markup            (GtkMenuTrackerItem *this);

GIcon *                 gtk_menu_tracker_item_get_icon                  (GtkMenuTrackerItem *this);

GIcon *                 gtk_menu_tracker_item_get_verb_icon             (GtkMenuTrackerItem *this);

gboolean                gtk_menu_tracker_item_get_sensitive             (GtkMenuTrackerItem *this);

GtkMenuTrackerItemRole  gtk_menu_tracker_item_get_role                  (GtkMenuTrackerItem *this);

gboolean                gtk_menu_tracker_item_get_toggled               (GtkMenuTrackerItem *this);

const char *           gtk_menu_tracker_item_get_accel                 (GtkMenuTrackerItem *this);

GMenuModel *           _gtk_menu_tracker_item_get_link                  (GtkMenuTrackerItem *this,
                                                                         const char         *link_name);

char *                _gtk_menu_tracker_item_get_link_namespace        (GtkMenuTrackerItem *this);

gboolean                gtk_menu_tracker_item_may_disappear             (GtkMenuTrackerItem *this);

gboolean                gtk_menu_tracker_item_get_is_visible            (GtkMenuTrackerItem *this);

gboolean                gtk_menu_tracker_item_get_should_request_show   (GtkMenuTrackerItem *this);

void                    gtk_menu_tracker_item_activated                 (GtkMenuTrackerItem *this);

void                    gtk_menu_tracker_item_request_submenu_shown     (GtkMenuTrackerItem *this,
                                                                         gboolean            shown);

gboolean                gtk_menu_tracker_item_get_submenu_shown         (GtkMenuTrackerItem *this);

