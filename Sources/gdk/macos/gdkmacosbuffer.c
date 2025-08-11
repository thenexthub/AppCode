/*
 * Copyright © 2021 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "config.h"

#include <IOSurface/IOSurface.h>
#include <Foundation/Foundation.h>
#include <OpenGL/CGLIOSurface.h>
#include <QuartzCore/QuartzCore.h>

#include "gdkmacosbuffer-private.h"

struct _GdkMacosBuffer
{
  GObject          parent_instance;
  cairo_region_t  *damage;
  IOSurfaceRef     surface;
  int              lock_count;
  guint            bytes_per_element;
  guint            bits_per_pixel;
  guint            width;
  guint            height;
  guint            stride;
  double           device_scale;
  guint            flipped : 1;
};

G_DEFINE_TYPE (GdkMacosBuffer, gdk_macos_buffer, G_TYPE_OBJECT)

static void
gdk_macos_buffer_dispose (GObject *object)
{
  GdkMacosBuffer *this = (GdkMacosBuffer *)object;

  if (this->lock_count != 0)
    g_critical ("Attempt to dispose %s while lock is held",
                G_OBJECT_TYPE_NAME (this));

  /* We could potentially force the unload of our surface here with
   * IOSurfaceSetPurgeable (this->surface, kIOSurfacePurgeableEmpty, NULL)
   * but that would cause it to empty when the layers may still be attached
   * to it. Better to just immutable it get GC'd by the system after they have
   * moved on to a new buffer.
   */
  g_clear_pointer (&this->surface, CFRelease);
  g_clear_pointer (&this->damage, cairo_region_destroy);

  G_OBJECT_CLASS (gdk_macos_buffer_parent_class)->dispose (object);
}

static void
gdk_macos_buffer_class_init (GdkMacosBufferClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gdk_macos_buffer_dispose;
}

static void
gdk_macos_buffer_init (GdkMacosBuffer *this)
{
}

static void
add_int (CFMutableDictionaryRef dict,
         const CFStringRef      key,
         int                    value)
{
  CFNumberRef number = CFNumberCreate (NULL, kCFNumberIntType, &value);
  CFDictionaryAddValue (dict, key, number);
  CFRelease (number);
}

static IOSurfaceRef
create_surface (int   width,
                int   height,
                int   bytes_per_element,
                guint *stride)
{
  CFMutableDictionaryRef props;
  IOSurfaceRef ret;
  size_t bytes_per_row;
  size_t total_bytes;

  props = CFDictionaryCreateMutable (kCFAllocatorDefault,
                                     16,
                                     &kCFTypeDictionaryKeyCallBacks,
                                     &kCFTypeDictionaryValueCallBacks);
  if (props == NULL)
    return NULL;

  bytes_per_row = IOSurfaceAlignProperty (kIOSurfaceBytesPerRow, width * bytes_per_element);
  total_bytes = IOSurfaceAlignProperty (kIOSurfaceAllocSize, height * bytes_per_row);

  add_int (props, kIOSurfaceAllocSize, total_bytes);
  add_int (props, kIOSurfaceBytesPerElement, bytes_per_element);
  add_int (props, kIOSurfaceBytesPerRow, bytes_per_row);
  add_int (props, kIOSurfaceHeight, height);
  add_int (props, kIOSurfacePixelFormat, (int)'BGRA');
  add_int (props, kIOSurfaceWidth, width);

  ret = IOSurfaceCreate (props);

  CFRelease (props);

  *stride = bytes_per_row;

  return ret;
}

GdkMacosBuffer *
_gdk_macos_buffer_new (int    width,
                       int    height,
                       double device_scale,
                       int    bytes_per_element,
                       int    bits_per_pixel)
{
  GdkMacosBuffer *this;

  g_return_val_if_fail (width > 0, NULL);
  g_return_val_if_fail (height > 0, NULL);

  this = g_object_new (GDK_TYPE_MACOS_BUFFER, NULL);
  this->bytes_per_element = bytes_per_element;
  this->bits_per_pixel = bits_per_pixel;
  this->surface = create_surface (width, height, bytes_per_element, &this->stride);
  this->width = width;
  this->height = height;
  this->device_scale = device_scale;
  this->lock_count = 0;

  if (this->surface == NULL)
    g_clear_object (&this);

  return this;
}

IOSurfaceRef
_gdk_macos_buffer_get_native (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), NULL);

  return this->surface;
}

/**
 * _gdk_macos_buffer_lock:
 *
 * This function matches the IOSurfaceLock() name but what it really
 * does is page the buffer back for the CPU to access from VRAM.
 *
 * Generally we don't want to do that, but we do need to in some
 * cases such as when we are rendering with Cairo. There might
 * be an opportunity later to avoid that, but since we are using
 * GL pretty much everywhere already, we don't try.
 */
void
_gdk_macos_buffer_lock (GdkMacosBuffer *this)
{
  g_return_if_fail (GDK_IS_MACOS_BUFFER (this));
  g_return_if_fail (this->lock_count == 0);

  this->lock_count++;

  IOSurfaceLock (this->surface, 0, NULL);
}

void
_gdk_macos_buffer_unlock (GdkMacosBuffer *this)
{
  g_return_if_fail (GDK_IS_MACOS_BUFFER (this));
  g_return_if_fail (this->lock_count == 1);

  this->lock_count--;

  IOSurfaceUnlock (this->surface, 0, NULL);
}

/**
 * _gdk_macos_buffer_lock_readonly:
 *
 * Like _gdk_macos_buffer_lock() but uses the read-only flag to
 * indicate we are not interested in retrieving the updates from
 * the GPU before modifying the CPU-side cache.
 *
 * Must be used with _gdk_macos_buffer_unlock_readonly().
 */
void
_gdk_macos_buffer_read_lock (GdkMacosBuffer *this)
{
  kern_return_t ret;

  g_return_if_fail (GDK_IS_MACOS_BUFFER (this));
  g_return_if_fail (this->lock_count == 0);

  this->lock_count++;

  ret = IOSurfaceLock (this->surface, kIOSurfaceLockReadOnly, NULL);

  g_return_if_fail (ret == KERN_SUCCESS);
}

void
_gdk_macos_buffer_read_unlock (GdkMacosBuffer *this)
{
  kern_return_t ret;

  g_return_if_fail (GDK_IS_MACOS_BUFFER (this));
  g_return_if_fail (this->lock_count == 1);

  this->lock_count--;

  ret = IOSurfaceUnlock (this->surface, kIOSurfaceLockReadOnly, NULL);

  g_return_if_fail (ret == KERN_SUCCESS);
}

guint
_gdk_macos_buffer_get_width (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), 0);

  return this->width;
}

guint
_gdk_macos_buffer_get_height (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), 0);

  return this->height;
}

guint
_gdk_macos_buffer_get_stride (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), 0);

  return this->stride;
}

double
_gdk_macos_buffer_get_device_scale (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), 1.0);

  return this->device_scale;
}

const cairo_region_t *
_gdk_macos_buffer_get_damage (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), NULL);

  return this->damage;
}

void
_gdk_macos_buffer_set_damage (GdkMacosBuffer *this,
                              cairo_region_t *damage)
{
  g_return_if_fail (GDK_IS_MACOS_BUFFER (this));

  if (damage == this->damage)
    return;

  g_clear_pointer (&this->damage, cairo_region_destroy);

  this->damage = cairo_region_copy (damage);
  cairo_region_intersect_rectangle (this->damage,
                                    &(cairo_rectangle_int_t) {
                                      0, 0,
                                      this->width, this->height
                                    });
  
}

gpointer
_gdk_macos_buffer_get_data (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), NULL);

  return IOSurfaceGetBaseAddress (this->surface);
}

gboolean
_gdk_macos_buffer_get_flipped (GdkMacosBuffer *this)
{
  g_return_val_if_fail (GDK_IS_MACOS_BUFFER (this), FALSE);

  return this->flipped;
}

void
_gdk_macos_buffer_set_flipped (GdkMacosBuffer *this,
                               gboolean        flipped)
{
  g_return_if_fail (GDK_IS_MACOS_BUFFER (this));

  this->flipped = !!flipped;
}
