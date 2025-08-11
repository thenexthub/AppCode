/*
 * Copyright © 2024 Red Hat, Inc.
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
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include "config.h"

#include "gdkd3d12texturebuilder.h"

#include "gdkdebugprivate.h"
#include "gdkcolorstate.h"
#include "gdkd3d12textureprivate.h"
#include "gdkprivate-win32.h"

#include <cairo-gobject.h>

struct _GdkD3D12TextureBuilder
{
  GObject parent_instance;

  ID3D12Resource *resource;
  ID3D12Fence *fence;
  guint64 fence_wait;

  GdkColorState *color_state;
  gboolean premultiplied;

  GdkTexture *update_texture;
  cairo_region_t *update_region;
};

struct _GdkD3D12TextureBuilderClass
{
  GObjectClass parent_class;
};

/**
 * GdkD3D12TextureBuilder:
 *
 * `GdkD3D12TextureBuilder` is a builder used to construct [class@Gdk.Texture]
 * objects from [ID3D12Resources](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12resource).
 *
 * The operation of `GdkD3D12TextureBuilder` is quite simple: Create a texture builder,
 * set all the necessary properties, and then call [method@Gdk.D3d12TextureBuilder.build]
 * to create the new texture.
 *
 * Not all `D3D12Resources` can be used. You have to use a texture resource for a `GdkTexture`.
 * GDK will attempt to detect invalid resources and fail to create the texture in that case.

 * `GdkD3D12TextureBuilder` can be used for quick one-shot construction of
 * textures as well as kept around and reused to construct multiple textures.
 *
 * Since: 4.20
 */

enum
{
  PROP_0,
  PROP_COLOR_STATE,
  PROP_FENCE,
  PROP_FENCE_WAIT,
  PROP_RESOURCE,
  PROP_PREMULTIPLIED,
  PROP_UPDATE_REGION,
  PROP_UPDATE_TEXTURE,

  N_PROPS
};

G_DEFINE_TYPE (GdkD3D12TextureBuilder, gdk_d3d12_texture_builder, G_TYPE_OBJECT)

static GParamSpec *properties[N_PROPS] = { NULL, };

static void
gdk_d3d12_texture_builder_dispose (GObject *object)
{
  GdkD3D12TextureBuilder *this = GDK_D3D12_TEXTURE_BUILDER (object);

  g_clear_object (&this->update_texture);
  g_clear_pointer (&this->update_region, cairo_region_destroy);
  g_clear_pointer (&this->color_state, gdk_color_state_unref);

  gdk_win32_com_clear (&this->resource);
  gdk_win32_com_clear (&this->fence);

  G_OBJECT_CLASS (gdk_d3d12_texture_builder_parent_class)->dispose (object);
}

static void
gdk_d3d12_texture_builder_get_property (GObject    *object,
                                        guint       property_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  GdkD3D12TextureBuilder *this = GDK_D3D12_TEXTURE_BUILDER (object);

  switch (property_id)
    {
    case PROP_COLOR_STATE:
      g_value_set_boxed (value, this->color_state);
      break;

    case PROP_FENCE:
      g_value_set_pointer (value, this->fence);
      break;

    case PROP_FENCE_WAIT:
      g_value_set_uint64 (value, this->fence_wait);
      break;

    case PROP_PREMULTIPLIED:
      g_value_set_boolean (value, this->premultiplied);
      break;

    case PROP_RESOURCE:
      g_value_set_pointer (value, this->resource);
      break;

    case PROP_UPDATE_REGION:
      g_value_set_boxed (value, this->update_region);
      break;

    case PROP_UPDATE_TEXTURE:
      g_value_set_object (value, this->update_texture);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gdk_d3d12_texture_builder_set_property (GObject      *object,
                                        guint         property_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  GdkD3D12TextureBuilder *this = GDK_D3D12_TEXTURE_BUILDER (object);

  switch (property_id)
    {
    case PROP_COLOR_STATE:
      gdk_d3d12_texture_builder_set_color_state (this, g_value_get_boxed (value));
      break;

    case PROP_FENCE:
      gdk_d3d12_texture_builder_set_fence (this, g_value_get_pointer (value));
      break;

    case PROP_FENCE_WAIT:
      gdk_d3d12_texture_builder_set_fence_wait (this, g_value_get_uint64 (value));
      break;

    case PROP_PREMULTIPLIED:
      gdk_d3d12_texture_builder_set_premultiplied (this, g_value_get_boolean (value));
      break;

    case PROP_RESOURCE:
      gdk_d3d12_texture_builder_set_resource (this, g_value_get_pointer (value));
      break;

    case PROP_UPDATE_REGION:
      gdk_d3d12_texture_builder_set_update_region (this, g_value_get_boxed (value));
      break;

    case PROP_UPDATE_TEXTURE:
      gdk_d3d12_texture_builder_set_update_texture (this, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gdk_d3d12_texture_builder_class_init (GdkD3D12TextureBuilderClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = gdk_d3d12_texture_builder_dispose;
  gobject_class->get_property = gdk_d3d12_texture_builder_get_property;
  gobject_class->set_property = gdk_d3d12_texture_builder_set_property;

  /**
   * GdkD3D12TextureBuilder:color-state:
   *
   * The color state of the texture.
   *
   * Since: 4.20
   */
  properties[PROP_COLOR_STATE] =
    g_param_spec_boxed ("color-state", NULL, NULL,
                        GDK_TYPE_COLOR_STATE,
                        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GdkD3D12TextureBuilder:fence:
   *
   * The optional `ID3D12Fence` to wait on before using the resource.
   *
   * Since: 4.20
   */
  properties[PROP_FENCE] =
    g_param_spec_pointer ("fence", NULL, NULL,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GdkD3D12TextureBuilder:fence-wait:
   *
   * The value the fence should wait on
   *
   * Since: 4.20
   */
  properties[PROP_FENCE_WAIT] =
    g_param_spec_uint64 ("fence-wait", NULL, NULL,
                         0, G_MAXUINT64, 0,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GdkD3D12TextureBuilder:premultiplied:
   *
   * Whether the alpha channel is premultiplied into the others.
   *
   * Only relevant if the format has alpha.
   *
   * Since: 4.20
   */
  properties[PROP_PREMULTIPLIED] =
    g_param_spec_boolean ("premultiplied", NULL, NULL,
                          TRUE,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GdkD3D12TextureBuilder:resource:
   *
   * The `ID3D12Resource`
   *
   * Since: 4.20
   */
  properties[PROP_RESOURCE] =
    g_param_spec_pointer ("resource", NULL, NULL,
                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GdkD3D12TextureBuilder:update-region:
   *
   * The update region for [property@Gdk.D3d12TextureBuilder:update-texture].
   *
   * Since: 4.20
   */
  properties[PROP_UPDATE_REGION] =
    g_param_spec_boxed ("update-region", NULL, NULL,
                        CAIRO_GOBJECT_TYPE_REGION,
                        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GdkD3D12TextureBuilder:update-texture:
   *
   * The texture [property@Gdk.D3d12TextureBuilder:update-region] is an update for.
   *
   * Since: 4.20
   */
  properties[PROP_UPDATE_TEXTURE] =
    g_param_spec_object ("update-texture", NULL, NULL,
                         GDK_TYPE_TEXTURE,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);
}

static void
gdk_d3d12_texture_builder_init (GdkD3D12TextureBuilder *this)
{
  this->premultiplied = TRUE;
}

/**
 * gdk_d3d12_texture_builder_new: (constructor):
 *
 * Creates a new texture builder.
 *
 * Returns: the new `GdkTextureBuilder`
 *
 * Since: 4.20
 **/
GdkD3D12TextureBuilder *
gdk_d3d12_texture_builder_new (void)
{
  return (GdkD3D12TextureBuilder *) g_object_new (GDK_TYPE_D3D12_TEXTURE_BUILDER, NULL);
}

/**
 * gdk_d3d12_texture_builder_get_resource:
 * @this: a `GdkD3D12TextureBuilder`
 *
 * Returns the resource that this texture builder is
 * associated with.
 *
 * Returns: (nullable) (transfer none): the resource
 *
 * Since: 4.20
 */
ID3D12Resource *
gdk_d3d12_texture_builder_get_resource (GdkD3D12TextureBuilder *this)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), NULL);

  return this->resource;
}

/**
 * gdk_d3d12_texture_builder_set_resource:
 * @this: a `GdkD3D12TextureBuilder`
 * @resource: the resource
 *
 * Sets the resource that this texture builder is going to construct
 * a texture for.
 *
 * Since: 4.20
 */
void
gdk_d3d12_texture_builder_set_resource (GdkD3D12TextureBuilder *this,
                                        ID3D12Resource         *resource)
{
  g_return_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this));

  if (this->resource == resource)
    return;

  if (resource)
    ID3D12Resource_AddRef (resource);
  if (this->resource)
    ID3D12Resource_Release (this->resource);
  this->resource = resource;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_RESOURCE]);
}

/**
 * gdk_d3d12_texture_builder_get_fence:
 * @this: a `GdkD3D12TextureBuilder`
 *
 * Returns the fence that this texture builder is
 * associated with.
 *
 * Returns: (nullable) (transfer none): the fence
 *
 * Since: 4.20
 */
ID3D12Fence *
gdk_d3d12_texture_builder_get_fence (GdkD3D12TextureBuilder *this)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), NULL);

  return this->fence;
}

/**
 * gdk_d3d12_texture_builder_set_fence:
 * @this: a `GdkD3D12TextureBuilder`
 * @fence: the fence
 *
 * Sets the fence that this texture builder is going to construct
 * a texture for.
 *
 * Since: 4.20
 */
void
gdk_d3d12_texture_builder_set_fence (GdkD3D12TextureBuilder *this,
                                     ID3D12Fence            *fence)
{
  g_return_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this));

  if (this->fence == fence)
    return;

  if (fence)
    ID3D12Fence_AddRef (fence);
  gdk_win32_com_clear (&this->fence);
  this->fence = fence;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_FENCE]);
}

/**
 * gdk_d3d12_texture_builder_get_fence_wait:
 * @this: a `GdkD3D12TextureBuilder`
 *
 * Returns the value that GTK should wait for on the fence
 * before using the resource.
 *
 * Returns: the fence wait value
 *
 * Since: 4.20
 */
guint64
gdk_d3d12_texture_builder_get_fence_wait (GdkD3D12TextureBuilder *this)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), 0);

  return this->fence_wait;
}

/**
 * gdk_d3d12_texture_builder_set_fence_wait:
 * @this: a `GdkD3D12TextureBuilder`
 * @fence_wait: the value to wait on
 *
 * Sets the value that GTK should wait on on the given fence before using the
 * resource.
 *
 * When no fence is set, this value has no effect.
 *
 * Since: 4.20
 */
void
gdk_d3d12_texture_builder_set_fence_wait (GdkD3D12TextureBuilder *this,
                                          guint64                 fence_wait)
{
  g_return_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this));

  if (this->fence_wait == fence_wait)
    return;

  this->fence_wait = fence_wait;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_FENCE_WAIT]);
}

/**
 * gdk_d3d12_texture_builder_get_premultiplied:
 * @this: a `GdkD3D12TextureBuilder`
 *
 * Whether the data is premultiplied.
 *
 * Returns: whether the data is premultiplied
 *
 * Since: 4.20
 */
gboolean
gdk_d3d12_texture_builder_get_premultiplied (GdkD3D12TextureBuilder *this)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), TRUE);

  return this->premultiplied;
}

/**
 * gdk_d3d12_texture_builder_set_premultiplied:
 * @this: a `GdkD3D12TextureBuilder`
 * @premultiplied: whether the data is premultiplied
 *
 * Sets whether the data is premultiplied.
 *
 * Unless otherwise specified, all formats including alpha channels are assumed
 * to be premultiplied.
 *
 * Since: 4.20
 */
void
gdk_d3d12_texture_builder_set_premultiplied (GdkD3D12TextureBuilder *this,
                                             gboolean                premultiplied)
{
  g_return_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this));

  if (this->premultiplied == premultiplied)
    return;

  this->premultiplied = premultiplied;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_PREMULTIPLIED]);
}

/**
 * gdk_d3d12_texture_builder_get_color_state:
 * @this: a `GdkD3D12TextureBuilder`
 *
 * Gets the color state previously set via gdk_d3d12_texture_builder_set_color_state().
 *
 * Returns: (nullable) (transfer none): the color state
 *
 * Since: 4.20
 */
GdkColorState *
gdk_d3d12_texture_builder_get_color_state (GdkD3D12TextureBuilder *this)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), NULL);

  return this->color_state;
}

/**
 * gdk_d3d12_texture_builder_set_color_state:
 * @this: a `GdkD3D12TextureBuilder`
 * @color_state: (nullable): a `GdkColorState` or `NULL` to unset the colorstate.
 *
 * Sets the color state for the texture.
 *
 * By default, the colorstate is `NULL`. In that case, GTK will choose the
 * correct colorstate based on the format.
 * If you don't know what colorstates are, this is probably the right thing.
 *
 * Since: 4.20
 */
void
gdk_d3d12_texture_builder_set_color_state (GdkD3D12TextureBuilder *this,
                                           GdkColorState          *color_state)
{
  g_return_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this));

  if (this->color_state == color_state ||
      (this->color_state != NULL && color_state != NULL && gdk_color_state_equal (this->color_state, color_state)))
    return;

  g_clear_pointer (&this->color_state, gdk_color_state_unref);
  this->color_state = color_state;
  if (color_state)
    gdk_color_state_ref (color_state);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_COLOR_STATE]);
}

/**
 * gdk_d3d12_texture_builder_get_update_texture:
 * @this: a `GdkD3D12TextureBuilder`
 *
 * Gets the texture previously set via gdk_d3d12_texture_builder_set_update_texture() or
 * %NULL if none was set.
 *
 * Returns: (transfer none) (nullable): The texture
 *
 * Since: 4.20
 */
GdkTexture *
gdk_d3d12_texture_builder_get_update_texture (GdkD3D12TextureBuilder *this)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), NULL);

  return this->update_texture;
}

/**
 * gdk_d3d12_texture_builder_set_update_texture:
 * @this: a `GdkD3D12TextureBuilder`
 * @texture: (nullable): the texture to update
 *
 * Sets the texture to be updated by this texture. See
 * [method@Gdk.D3d12TextureBuilder.set_update_region] for an explanation.
 *
 * Since: 4.20
 */
void
gdk_d3d12_texture_builder_set_update_texture (GdkD3D12TextureBuilder *this,
                                              GdkTexture             *texture)
{
  g_return_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this));
  g_return_if_fail (texture == NULL || GDK_IS_TEXTURE (texture));

  if (!g_set_object (&this->update_texture, texture))
    return;

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_UPDATE_TEXTURE]);
}

/**
 * gdk_d3d12_texture_builder_get_update_region:
 * @this: a `GdkD3D12TextureBuilder`
 *
 * Gets the region previously set via gdk_d3d12_texture_builder_set_update_region() or
 * %NULL if none was set.
 *
 * Returns: (transfer none) (nullable): The region
 *
 * Since: 4.20
 */
cairo_region_t *
gdk_d3d12_texture_builder_get_update_region (GdkD3D12TextureBuilder *this)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), NULL);

  return this->update_region;
}

/**
 * gdk_d3d12_texture_builder_set_update_region:
 * @this: a `GdkD3D12TextureBuilder`
 * @region: (nullable): the region to update
 *
 * Sets the region to be updated by this texture. Together with
 * [property@Gdk.D3d12TextureBuilder:update-texture] this describes an
 * update of a previous texture.
 *
 * When rendering animations of large textures, it is possible that
 * consecutive textures are only updating contents in parts of the texture.
 * It is then possible to describe this update via these two properties,
 * so that GTK can avoid rerendering parts that did not change.
 *
 * An example would be a screen recording where only the mouse pointer moves.
 *
 * Since: 4.20
 */
void
gdk_d3d12_texture_builder_set_update_region (GdkD3D12TextureBuilder *this,
                                              cairo_region_t      *region)
{
  g_return_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this));

  if (this->update_region == region)
    return;

  g_clear_pointer (&this->update_region, cairo_region_destroy);

  if (region)
    this->update_region = cairo_region_reference (region);

  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_UPDATE_REGION]);
}

/**
 * gdk_d3d12_texture_builder_build:
 * @this: a `GdkD3D12TextureBuilder`
 * @destroy: (nullable): destroy function to be called when the texture is
 *   released
 * @data: user data to pass to the destroy function
 * @error: Return location for an error
 *
 * Builds a new `GdkTexture` with the values set up in the builder.
 *
 * It is a programming error to call this function if any mandatory property has not been set.
 *
 * Not all formats defined in the `drm_fourcc.h` header are supported. You can use
 * [method@Gdk.Display.get_d3d12_formats] to get a list of supported formats. If the
 * format is not supported by GTK, %NULL will be returned and @error will be set.
 *
 * The `destroy` function gets called when the returned texture gets released.
 *
 * It is the responsibility of the caller to keep the file descriptors for the planes
 * open until the created texture is no longer used, and close them afterwards (possibly
 * using the @destroy notify).
 *
 * It is possible to call this function multiple times to create multiple textures,
 * possibly with changing properties in between.
 *
 * Returns: (transfer full) (nullable): a newly built `GdkTexture` or `NULL`
 *   if the format is not supported
 *
 * Since: 4.20
 */
GdkTexture *
gdk_d3d12_texture_builder_build (GdkD3D12TextureBuilder *this,
                                  GDestroyNotify           destroy,
                                  gpointer                 data,
                                  GError                 **error)
{
  g_return_val_if_fail (GDK_IS_D3D12_TEXTURE_BUILDER (this), NULL);
  g_return_val_if_fail (destroy == NULL || data != NULL, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);
  g_return_val_if_fail (this->resource, NULL);

  return gdk_d3d12_texture_new_from_builder (this, destroy, data, error);
}
