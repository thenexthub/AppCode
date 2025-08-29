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

#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture.h"
#include "appcode/shell/platform/linux/fl_texture_private.h"

#include <gmodule.h>
#include <cstdio>

G_DEFINE_INTERFACE(FlTexture, fl_texture, G_TYPE_OBJECT)

static void fl_texture_default_init(FlTextureInterface* self) {}

void fl_texture_set_id(FlTexture* self, int64_t id) {
  g_return_if_fail(FL_IS_TEXTURE(self));
  FL_TEXTURE_GET_IFACE(self)->set_id(self, id);
}

G_MODULE_EXPORT int64_t fl_texture_get_id(FlTexture* self) {
  g_return_val_if_fail(FL_IS_TEXTURE(self), -1);
  return FL_TEXTURE_GET_IFACE(self)->get_id(self);
}
