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

#include "appcode/shell/platform/linux/fl_texture_gl_private.h"
#include "appcode/shell/platform/linux/fl_texture_private.h"
#include "appcode/shell/platform/linux/fl_texture_registrar_private.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_texture_registrar.h"
#include "appcode/shell/platform/linux/testing/fl_test.h"
#include "gtest/gtest.h"

#include <epoxy/gl.h>

static constexpr uint32_t kBufferWidth = 4u;
static constexpr uint32_t kBufferHeight = 4u;
static constexpr uint32_t kRealBufferWidth = 2u;
static constexpr uint32_t kRealBufferHeight = 2u;

G_DECLARE_FINAL_TYPE(FlTestTexture,
                     fl_test_texture,
                     FL,
                     TEST_TEXTURE,
                     FlTextureGL)

/// A simple texture.
struct _FlTestTexture {
  FlTextureGL parent_instance;
};

G_DEFINE_TYPE(FlTestTexture, fl_test_texture, fl_texture_gl_get_type())

static gboolean fl_test_texture_populate(FlTextureGL* texture,
                                         uint32_t* target,
                                         uint32_t* name,
                                         uint32_t* width,
                                         uint32_t* height,
                                         GError** error) {
  EXPECT_TRUE(FL_IS_TEST_TEXTURE(texture));

  EXPECT_EQ(*width, kBufferWidth);
  EXPECT_EQ(*height, kBufferHeight);
  *target = GL_TEXTURE_2D;
  *name = 1;
  *width = kRealBufferWidth;
  *height = kRealBufferHeight;

  return TRUE;
}

static void fl_test_texture_class_init(FlTestTextureClass* klass) {
  FL_TEXTURE_GL_CLASS(klass)->populate = fl_test_texture_populate;
}

static void fl_test_texture_init(FlTestTexture* self) {}

static FlTestTexture* fl_test_texture_new() {
  return FL_TEST_TEXTURE(g_object_new(fl_test_texture_get_type(), nullptr));
}

// Test that getting the texture ID works.
TEST(FlTextureGLTest, TextureID) {
  g_autoptr(FlTexture) texture = FL_TEXTURE(fl_test_texture_new());
  fl_texture_set_id(texture, 42);
  EXPECT_EQ(fl_texture_get_id(texture), static_cast<int64_t>(42));
}

// Test that populating an OpenGL texture works.
TEST(FlTextureGLTest, PopulateTexture) {
  g_autoptr(FlTextureGL) texture = FL_TEXTURE_GL(fl_test_texture_new());
  appcodeOpenGLTexture opengl_texture = {0};
  g_autoptr(GError) error = nullptr;
  EXPECT_TRUE(fl_texture_gl_populate(texture, kBufferWidth, kBufferHeight,
                                     &opengl_texture, &error));
  EXPECT_EQ(error, nullptr);
  EXPECT_EQ(opengl_texture.width, kRealBufferWidth);
  EXPECT_EQ(opengl_texture.height, kRealBufferHeight);
}
