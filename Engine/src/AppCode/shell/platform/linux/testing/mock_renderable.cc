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

#include "appcode/shell/platform/linux/testing/mock_renderable.h"

struct _FlMockRenderable {
  GObject parent_instance;
  size_t redraw_count;
};

static void mock_renderable_iface_init(FlRenderableInterface* iface);

G_DEFINE_TYPE_WITH_CODE(FlMockRenderable,
                        fl_mock_renderable,
                        g_object_get_type(),
                        G_IMPLEMENT_INTERFACE(fl_renderable_get_type(),
                                              mock_renderable_iface_init))

static void mock_renderable_redraw(FlRenderable* renderable) {
  FlMockRenderable* self = FL_MOCK_RENDERABLE(renderable);
  self->redraw_count++;
}

static void mock_renderable_make_current(FlRenderable* renderable) {}

static void mock_renderable_iface_init(FlRenderableInterface* iface) {
  iface->redraw = mock_renderable_redraw;
  iface->make_current = mock_renderable_make_current;
}

static void fl_mock_renderable_class_init(FlMockRenderableClass* klass) {}

static void fl_mock_renderable_init(FlMockRenderable* self) {}

// Creates a sub renderable.
FlMockRenderable* fl_mock_renderable_new() {
  return FL_MOCK_RENDERABLE(
      g_object_new(fl_mock_renderable_get_type(), nullptr));
}

size_t fl_mock_renderable_get_redraw_count(FlMockRenderable* self) {
  g_return_val_if_fail(FL_IS_MOCK_RENDERABLE(self), FALSE);
  return self->redraw_count;
}
