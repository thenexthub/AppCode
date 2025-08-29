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

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PIXEL_BUFFER_TEXTURE_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PIXEL_BUFFER_TEXTURE_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <gmodule.h>

#include "fl_texture.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_DERIVABLE_TYPE(FlPixelBufferTexture,
                         fl_pixel_buffer_texture,
                         FL,
                         PIXEL_BUFFER_TEXTURE,
                         GObject)

/**
 * FlPixelBufferTexture:
 *
 * #FlPixelBufferTexture represents an OpenGL texture generated from a pixel
 * buffer.
 *
 * The following example shows how to implement an #FlPixelBufferTexture.
 * ![<!-- language="C" -->
 *   struct _MyTexture {
 *     FlPixelBufferTexture parent_instance;
 *
 *     uint8_t *buffer;  // your pixel buffer.
 *   }
 *
 *   G_DEFINE_TYPE(MyTexture,
 *                 my_texture,
 *                 fl_pixel_buffer_texture_get_type ())
 *
 *   static gboolean
 *   my_texture_copy_pixels (FlPixelBufferTexture* texture,
 *                           const uint8_t** out_buffer,
 *                           uint32_t* width,
 *                           uint32_t* height,
 *                           GError** error) {
 *     // This method is called on Render Thread. Be careful with your
 *     // cross-thread operation.
 *
 *     // @width and @height are initially stored the canvas size in Flutter.
 *
 *     // You must prepare your pixel buffer in RGBA format.
 *     // So you may do some format conversion first if your original pixel
 *     // buffer is not in RGBA format.
 *     manage_your_pixel_buffer_here ();
 *
 *     if (your_operations_are_successfully_finished) {
 *       // Directly return pointer to your pixel buffer here.
 *       // Flutter takes content of your pixel buffer after this function
 *       // is finished. So you must make the buffer live long enough until
 *       // next tick of Render Thread.
 *       // If it is hard to manage lifetime of your pixel buffer, you should
 *       // take look into #FlTextureGL.
 *
 *       *out_buffer = buffer;
 *       *width = real_width_of_buffer;
 *       *height = real_height_of_buffer;
 *       return TRUE;
 *     } else {
 *       // set @error to report failure.
 *       return FALSE;
 *     }
 *   }
 *
 *   static void my_texture_class_init(MyTextureClass* klass) {
 *     FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels =
 * my_texture_copy_pixels;
 *   }
 *
 *   static void my_texture_init(MyTexture* self) {}
 * ]|
 */

struct _FlPixelBufferTextureClass {
  GObjectClass parent_class;

  /**
   * FlPixelBufferTexture::copy_pixels:
   * @texture: an #FlPixelBufferTexture.
   * @buffer: (out): pixel data.
   * @width: (inout): width of the texture in pixels.
   * @height: (inout): height of the texture in pixels.
   * @error: (allow-none): #GError location to store the error occurring, or
   * %NULL to ignore.
   *
   * Retrieve pixel buffer in RGBA format.
   *
   * As this method is usually invoked from the render thread, you must
   * take care of proper synchronization. It also needs to be ensured that
   * the returned buffer is not released prior to unregistering this texture.
   *
   * Returns: %TRUE on success.
   */
  gboolean (*copy_pixels)(FlPixelBufferTexture* texture,
                          const uint8_t** buffer,
                          uint32_t* width,
                          uint32_t* height,
                          GError** error);
};

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_PIXEL_BUFFER_TEXTURE_H_
