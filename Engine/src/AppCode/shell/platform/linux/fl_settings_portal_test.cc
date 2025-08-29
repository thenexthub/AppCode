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

#include "appcode/shell/platform/linux/fl_settings_portal.h"
#include "appcode/shell/platform/linux/testing/fl_test.h"
#include "appcode/testing/testing.h"

#include <glib.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(FlSettingsPortalTest, ClockFormat) {
  g_autoptr(GVariantDict) settings = g_variant_dict_new(nullptr);

  g_autoptr(FlSettings) portal =
      FL_SETTINGS(fl_settings_portal_new_with_values(settings));
  EXPECT_EQ(fl_settings_get_clock_format(portal), FL_CLOCK_FORMAT_24H);

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::clock-format",
                              g_variant_new_string("24h"));
  EXPECT_EQ(fl_settings_get_clock_format(portal), FL_CLOCK_FORMAT_24H);

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::clock-format",
                              g_variant_new_string("12h"));
  EXPECT_EQ(fl_settings_get_clock_format(portal), FL_CLOCK_FORMAT_12H);

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::clock-format",
                              g_variant_new_string("unknown"));
  EXPECT_EQ(fl_settings_get_clock_format(portal), FL_CLOCK_FORMAT_24H);
}

TEST(FlSettingsPortalTest, ColorScheme) {
  g_autoptr(GVariantDict) settings = g_variant_dict_new(nullptr);

  g_autoptr(FlSettings) portal =
      FL_SETTINGS(fl_settings_portal_new_with_values(settings));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);

  g_variant_dict_insert_value(settings,
                              "org.freedesktop.appearance::color-scheme",
                              g_variant_new_uint32(1));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_DARK);

  g_variant_dict_insert_value(settings,
                              "org.freedesktop.appearance::color-scheme",
                              g_variant_new_uint32(2));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);

  g_variant_dict_insert_value(settings,
                              "org.freedesktop.appearance::color-scheme",
                              g_variant_new_uint32(123));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);

  // color-scheme takes precedence over gtk-theme
  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::gtk-theme",
                              g_variant_new_string("Yaru-dark"));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);
}

TEST(FlSettingsPortalTest, GtkTheme) {
  g_autoptr(GVariantDict) settings = g_variant_dict_new(nullptr);

  g_autoptr(FlSettings) portal =
      FL_SETTINGS(fl_settings_portal_new_with_values(settings));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::gtk-theme",
                              g_variant_new_string("Yaru-dark"));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_DARK);

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::gtk-theme",
                              g_variant_new_string("Yaru"));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::gtk-theme",
                              g_variant_new_string("Adwaita"));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::gtk-theme",
                              g_variant_new_string("Adwaita-dark"));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_DARK);

  // color-scheme takes precedence over gtk-theme
  g_variant_dict_insert_value(settings,
                              "org.freedesktop.appearance::color-scheme",
                              g_variant_new_uint32(2));
  EXPECT_EQ(fl_settings_get_color_scheme(portal), FL_COLOR_SCHEME_LIGHT);
}

TEST(FlSettingsPortalTest, EnableAnimations) {
  g_autoptr(GVariantDict) settings = g_variant_dict_new(nullptr);

  g_autoptr(FlSettings) portal =
      FL_SETTINGS(fl_settings_portal_new_with_values(settings));
  EXPECT_TRUE(fl_settings_get_enable_animations(portal));

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.interface::enable-animations",
                              g_variant_new_boolean(false));
  EXPECT_FALSE(fl_settings_get_enable_animations(portal));
}

TEST(FlSettingsPortalTest, HighContrast) {
  g_autoptr(GVariantDict) settings = g_variant_dict_new(nullptr);

  g_autoptr(FlSettings) portal =
      FL_SETTINGS(fl_settings_portal_new_with_values(settings));
  EXPECT_FALSE(fl_settings_get_high_contrast(portal));

  g_variant_dict_insert_value(settings,
                              "org.gnome.desktop.a11y.interface::high-contrast",
                              g_variant_new_boolean(true));
  EXPECT_TRUE(fl_settings_get_high_contrast(portal));
}

TEST(FlSettingsPortalTest, TextScalingFactor) {
  g_autoptr(GVariantDict) settings = g_variant_dict_new(nullptr);

  g_autoptr(FlSettings) portal =
      FL_SETTINGS(fl_settings_portal_new_with_values(settings));
  EXPECT_EQ(fl_settings_get_text_scaling_factor(portal), 1.0);

  g_variant_dict_insert_value(
      settings, "org.gnome.desktop.interface::text-scaling-factor",
      g_variant_new_double(1.5));
  EXPECT_EQ(fl_settings_get_text_scaling_factor(portal), 1.5);
}
