/* GDK - The GIMP Drawing Kit
 *
 * Copyright (C) 2021 Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "gdkcolorprivate.h"

#include "gdkcolorstateprivate.h"


#define gdk_color_init(...) _gdk_color_init (__VA_ARGS__)
static inline void
_gdk_color_init (GdkColor        *this,
                 GdkColorState   *color_state,
                 const float      values[4])
{
  this->color_state = gdk_color_state_ref (color_state);
  memcpy (this->values, values, sizeof (float) * 4);
}

#define gdk_color_init_copy(this, color) _gdk_color_init_copy ((this), (color))
static inline void
_gdk_color_init_copy (GdkColor       *this,
                      const GdkColor *color)
{
  _gdk_color_init (this, color->color_state, color->values);
}

#define gdk_color_init_from_rgb(this, rgba) _gdk_color_init_from_rgba ((this), (rgba))
static inline void
_gdk_color_init_from_rgba (GdkColor      *this,
                           const GdkRGBA *rgba)
{
  _gdk_color_init (this, GDK_COLOR_STATE_SRGB, (const float *) rgba);
}

#define gdk_color_finish(this) _gdk_color_finish ((this))
static inline void
_gdk_color_finish (GdkColor *this)
{
  gdk_color_state_unref (this->color_state);
  this->color_state = NULL;
}

#define gdk_color_get_color_state(this) _gdk_color_get_color_state ((this))
static inline GdkColorState *
_gdk_color_get_color_state (const GdkColor *this)
{
  return this->color_state;
}

#define gdk_color_equal(this, other) _gdk_color_equal ((this), (other))
static inline gboolean
_gdk_color_equal (const GdkColor *this,
                  const GdkColor *other)
{
  return this->values[0] == other->values[0] &&
         this->values[1] == other->values[1] &&
         this->values[2] == other->values[2] &&
         this->values[3] == other->values[3] &&
         gdk_color_state_equal (this->color_state, other->color_state);
}

#define gdk_color_is_clear(this) _gdk_color_is_clear ((this))
static inline gboolean
_gdk_color_is_clear (const GdkColor *this)
{
  return this->alpha < (255.f / 65535.f);
}

#define gdk_color_is_opaque(this) _gdk_color_is_opaque ((this))
static inline gboolean
_gdk_color_is_opaque (const GdkColor *this)
{
  return this->alpha > (65280.f / 65535.f);
}

#define gdk_color_convert(this, cs, other) _gdk_color_convert ((this), (cs), (other))
static inline void
_gdk_color_convert (GdkColor        *this,
                    GdkColorState   *color_state,
                    const GdkColor  *other)
{
  if (gdk_color_state_equal (color_state, other->color_state))
    {
      gdk_color_init_copy (this, other);
      return;
    }

  this->color_state = gdk_color_state_ref (color_state);

  gdk_color_state_convert_color (other->color_state,
                                 other->values,
                                 this->color_state,
                                 this->values);
}

#define gdk_color_to_float(this, cs, values) _gdk_color_to_float ((this), (cs), (values))
static inline void
_gdk_color_to_float (const GdkColor  *this,
                     GdkColorState   *color_state,
                     float            values[4])
{
  if (gdk_color_state_equal (this->color_state, color_state))
    {
      values[0] = this->values[0];
      values[1] = this->values[1];
      values[2] = this->values[2];
      values[3] = this->values[3];
      return;
    }

  gdk_color_state_convert_color (this->color_state,
                                 this->values,
                                 color_state,
                                 values);
}

