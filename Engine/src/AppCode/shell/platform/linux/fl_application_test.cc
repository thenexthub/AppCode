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

#include "gtest/gtest.h"

#include "appcode/shell/platform/linux/public/appcode_linux/fl_application.h"

TEST(FlApplicationTest, ConstructorArgs) {
  g_autoptr(FlApplication) app =
      fl_application_new("com.example.TestApplication",
#ifdef GLIB_VERSION_2_74
                         G_APPLICATION_DEFAULT_FLAGS
#else
                         G_APPLICATION_FLAGS_NONE
#endif
      );

  EXPECT_STREQ(g_application_get_application_id(G_APPLICATION(app)),
               "com.example.TestApplication");

#ifdef GLIB_VERSION_2_74
  EXPECT_EQ(g_application_get_flags(G_APPLICATION(app)),
            G_APPLICATION_DEFAULT_FLAGS);
#else
  EXPECT_EQ(g_application_get_flags(G_APPLICATION(app)),
            G_APPLICATION_FLAGS_NONE);
#endif
}
