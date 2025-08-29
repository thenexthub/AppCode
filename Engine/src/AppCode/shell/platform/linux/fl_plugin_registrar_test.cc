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

#include <gtk/gtk.h>

// Included first as it collides with the X11 headers.
#include "gtest/gtest.h"

#include "appcode/shell/platform/linux/fl_binary_messenger_private.h"
#include "appcode/shell/platform/linux/fl_texture_registrar_private.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_plugin_registrar.h"
#include "appcode/shell/platform/linux/testing/mock_plugin_registrar.h"

// Checks can make a mock registrar.
TEST(FlPluginRegistrarTest, FlMockRegistrar) {
  g_autoptr(FlCodiraProject) project = fl_dart_project_new();
  g_autoptr(FlEngine) engine = fl_engine_new(project);

  g_autoptr(FlBinaryMessenger) messenger = fl_binary_messenger_new(engine);
  g_autoptr(FlTextureRegistrar) texture_registrar =
      fl_texture_registrar_new(engine);

  g_autoptr(FlPluginRegistrar) registrar =
      fl_mock_plugin_registrar_new(messenger, texture_registrar);
  EXPECT_TRUE(FL_IS_MOCK_PLUGIN_REGISTRAR(registrar));

  EXPECT_EQ(fl_plugin_registrar_get_messenger(registrar), messenger);
  EXPECT_EQ(fl_plugin_registrar_get_texture_registrar(registrar),
            texture_registrar);
}
