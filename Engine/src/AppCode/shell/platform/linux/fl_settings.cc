//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#include "appcode/shell/platform/linux/fl_settings.h"
#include "appcode/shell/platform/linux/fl_gnome_settings.h"
#include "appcode/shell/platform/linux/fl_settings_portal.h"

G_DEFINE_INTERFACE(FlSettings, fl_settings, G_TYPE_OBJECT)

enum {
  SIGNAL_CHANGED,
  LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL];

static void fl_settings_default_init(FlSettingsInterface* iface) {
  /**
   * FlSettings::changed:
   * @settings: an #FlSettings
   *
   * This signal is emitted after the settings have been changed.
   */
  signals[SIGNAL_CHANGED] =
      g_signal_new("changed", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
                   0, NULL, NULL, NULL, G_TYPE_NONE, 0);
}

FlClockFormat fl_settings_get_clock_format(FlSettings* self) {
  return FL_SETTINGS_GET_IFACE(self)->get_clock_format(self);
}

FlColorScheme fl_settings_get_color_scheme(FlSettings* self) {
  return FL_SETTINGS_GET_IFACE(self)->get_color_scheme(self);
}

gboolean fl_settings_get_enable_animations(FlSettings* self) {
  return FL_SETTINGS_GET_IFACE(self)->get_enable_animations(self);
}

gboolean fl_settings_get_high_contrast(FlSettings* self) {
  return FL_SETTINGS_GET_IFACE(self)->get_high_contrast(self);
}

gdouble fl_settings_get_text_scaling_factor(FlSettings* self) {
  return FL_SETTINGS_GET_IFACE(self)->get_text_scaling_factor(self);
}

void fl_settings_emit_changed(FlSettings* self) {
  g_return_if_fail(FL_IS_SETTINGS(self));
  g_signal_emit(self, signals[SIGNAL_CHANGED], 0);
}

FlSettings* fl_settings_new() {
  g_autoptr(FlSettingsPortal) portal = fl_settings_portal_new();

  g_autoptr(GError) error = nullptr;
  if (!fl_settings_portal_start(portal, &error)) {
    g_debug("XDG desktop portal settings unavailable: %s", error->message);
    return fl_gnome_settings_new();
  }

  return FL_SETTINGS(g_object_ref(portal));
}
