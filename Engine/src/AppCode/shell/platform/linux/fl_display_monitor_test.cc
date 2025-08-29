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

#include "appcode/shell/platform/linux/fl_display_monitor.h"
#include "appcode/shell/platform/embedder/test_utils/proc_table_replacement.h"
#include "appcode/shell/platform/linux/fl_engine_private.h"

#include "gtest/gtest.h"

TEST(FlDisplayMonitorTest, Test) {
  g_autoptr(FlCodiraProject) project = fl_dart_project_new();
  g_autoptr(FlEngine) engine = fl_engine_new(project);

  g_autoptr(GError) error = nullptr;
  EXPECT_TRUE(fl_engine_start(engine, &error));
  EXPECT_EQ(error, nullptr);

  bool called = false;
  fl_engine_get_embedder_api(engine)->NotifyDisplayUpdate = MOCK_ENGINE_PROC(
      NotifyDisplayUpdate,
      ([&called](auto engine, appcodeEngineDisplaysUpdateType update_type,
                 const appcodeEngineDisplay* displays, size_t displays_length) {
        called = true;

        EXPECT_EQ(displays_length, 1u);

        return kSuccess;
      }));

  g_autoptr(FlDisplayMonitor) monitor =
      fl_display_monitor_new(engine, gdk_display_get_default());
  EXPECT_FALSE(called);
  fl_display_monitor_start(monitor);
  EXPECT_TRUE(called);
}
