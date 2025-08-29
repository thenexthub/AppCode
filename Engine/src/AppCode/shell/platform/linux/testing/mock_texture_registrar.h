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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_TEXTURE_REGISTRAR_H_
#define APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_TEXTURE_REGISTRAR_H_

#include "appcode/shell/platform/linux/fl_texture_registrar_private.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(FlMockTextureRegistrar,
                     fl_mock_texture_registrar,
                     FL,
                     MOCK_TEXTURE_REGISTRAR,
                     GObject)

FlMockTextureRegistrar* fl_mock_texture_registrar_new();

FlTexture* fl_mock_texture_registrar_get_texture(
    FlMockTextureRegistrar* registrar);

gboolean fl_mock_texture_registrar_get_frame_available(
    FlMockTextureRegistrar* registrar);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_TESTING_MOCK_TEXTURE_REGISTRAR_H_
