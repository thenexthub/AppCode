/* gdkgltexture.c
 *
 * Copyright 2016  Benjamin Otte
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

#include "config.h"

#include "gdkgltextureprivate.h"

#include "gdkcolorstateprivate.h"
#include "gdkdisplayprivate.h"
#include "gdkglcontextprivate.h"
#include "gdkmemoryformatprivate.h"
#include "gdkmemorytextureprivate.h"

#include <epoxy/gl.h>

/**
 * GdkGLTexture:
 *
 * A `GdkTexture` representing a GL texture object.
 */

struct _GdkGLTexture {
  GdkTexture parent_instance;

  GdkGLContext *context;
  guint id;
  gboolean has_mipmap;
  gpointer sync;

  GdkTexture *saved;

  GDestroyNotify destroy;
  gpointer data;
};

struct _GdkGLTextureClass {
  GdkTextureClass parent_class;
};

G_DEFINE_TYPE (GdkGLTexture, gdk_gl_texture, GDK_TYPE_TEXTURE)

static void
drop_gl_resources (GdkGLTexture *this)
{
  if (this->destroy)
    {
      this->destroy (this->data);
      this->destroy = NULL;
      this->data = NULL;
    }

  g_clear_object (&this->context);
  this->id = 0;
}

static void
gdk_gl_texture_dispose (GObject *object)
{
  GdkGLTexture *this = GDK_GL_TEXTURE (object);

  drop_gl_resources (this);

  g_clear_object (&this->saved);

  G_OBJECT_CLASS (gdk_gl_texture_parent_class)->dispose (object);
}

typedef void (* GLFunc) (GdkGLTexture *this,
                         GdkGLContext *context,
                         gpointer      data);

typedef struct _InvokeData
{
  GdkGLTexture *this;
  volatile int spinlock;
  GLFunc fn;
  gpointer data;
} InvokeData;

static gboolean
gdk_gl_texture_invoke_callback (gpointer data)
{
  InvokeData *invoke = data;
  GdkGLContext *context, *previous;

  context = gdk_display_get_gl_context (gdk_gl_context_get_display (invoke->this->context));

  previous = gdk_gl_context_get_current ();

  if (previous)
    g_object_ref (previous);

  gdk_gl_context_make_current (context);

  if (invoke->this->sync && context != invoke->this->context)
    glWaitSync (invoke->this->sync, 0, GL_TIMEOUT_IGNORED);

  glBindTexture (GL_TEXTURE_2D, invoke->this->id);

  invoke->fn (invoke->this, context, invoke->data);

  g_atomic_int_set (&invoke->spinlock, 1);

  if (previous)
    {
      gdk_gl_context_make_current (previous);
      g_object_unref (previous);
    }
  else
    {
      gdk_gl_context_clear_current ();
    }

  return FALSE;
}

static void
gdk_gl_texture_run (GdkGLTexture *this,
                    GLFunc        fn,
                    gpointer      data)
{
  InvokeData invoke = { this, 0, fn, data };

  g_main_context_invoke (NULL, gdk_gl_texture_invoke_callback, &invoke);

  while (g_atomic_int_get (&invoke.spinlock) == 0);
}

typedef struct _Download Download;

struct _Download
{
  guchar *data;
  GdkMemoryLayout layout;
  GdkColorState *color_state;
};

static void
gdk_gl_texture_do_download (GdkGLTexture *this,
                            GdkGLContext *context,
                            gpointer      download_)
{
  GdkTexture *texture = GDK_TEXTURE (this);
  Download *download = download_;

  gdk_gl_context_download (context,
                           gdk_gl_texture_get_id (this),
                           gdk_texture_get_format (texture),
                           gdk_texture_get_color_state (texture),
                           download->data,
                           &download->layout,
                           download->color_state);
}
                            
static void
gdk_gl_texture_download (GdkTexture            *texture,
                         guchar                *data,
                         const GdkMemoryLayout *layout,
                         GdkColorState         *color_state)
{
  GdkGLTexture *this = GDK_GL_TEXTURE (texture);
  Download download;

  if (this->saved)
    {
      gdk_texture_do_download (this->saved, data, layout, color_state);
      return;
    }

  download.data = data;
  download.layout = *layout;
  download.color_state = color_state;

  gdk_gl_texture_run (this, gdk_gl_texture_do_download, &download);
}

static void
gdk_gl_texture_class_init (GdkGLTextureClass *klass)
{
  GdkTextureClass *texture_class = GDK_TEXTURE_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  texture_class->download = gdk_gl_texture_download;

  gobject_class->dispose = gdk_gl_texture_dispose;
}

static void
gdk_gl_texture_init (GdkGLTexture *this)
{
}

GdkGLContext *
gdk_gl_texture_get_context (GdkGLTexture *this)
{
  return this->context;
}

guint
gdk_gl_texture_get_id (GdkGLTexture *this)
{
  return this->id;
}

gboolean
gdk_gl_texture_has_mipmap (GdkGLTexture *this)
{
  return this->has_mipmap;
}

gpointer
gdk_gl_texture_get_sync (GdkGLTexture *this)
{
  return this->sync;
}

/**
 * gdk_gl_texture_release:
 * @this: a `GdkTexture` wrapping a GL texture
 *
 * Releases the GL resources held by a `GdkGLTexture`.
 *
 * The texture contents are still available via the
 * [method@Gdk.Texture.download] function, after this
 * function has been called.
 */
void
gdk_gl_texture_release (GdkGLTexture *this)
{
  GdkTexture *texture;

  g_return_if_fail (GDK_IS_GL_TEXTURE (this));
  g_return_if_fail (this->saved == NULL);

  texture = GDK_TEXTURE (this);
  this->saved = GDK_TEXTURE (gdk_memory_texture_from_texture (texture));

  drop_gl_resources (this);
}

GdkTexture *
gdk_gl_texture_new_from_builder (GdkGLTextureBuilder *builder,
                                 GDestroyNotify       destroy,
                                 gpointer             data)
{
  GdkGLTexture *this;
  GdkTexture *update_texture;

  this = g_object_new (GDK_TYPE_GL_TEXTURE,
                       "width", gdk_gl_texture_builder_get_width (builder),
                       "height", gdk_gl_texture_builder_get_height (builder),
                       "color-state", gdk_gl_texture_builder_get_color_state (builder),
                       NULL);

  this->context = g_object_ref (gdk_gl_texture_builder_get_context (builder));
  this->id = gdk_gl_texture_builder_get_id (builder);
  GDK_TEXTURE (this)->format = gdk_gl_texture_builder_get_format (builder);
  this->has_mipmap = gdk_gl_texture_builder_get_has_mipmap (builder);
  this->sync = gdk_gl_texture_builder_get_sync (builder);
  this->destroy = destroy;
  this->data = data;

  update_texture = gdk_gl_texture_builder_get_update_texture (builder);
  if (update_texture)
    {
      cairo_region_t *update_region = gdk_gl_texture_builder_get_update_region (builder);
      if (update_region)
        {
          update_region = cairo_region_copy (update_region);
          cairo_region_intersect_rectangle (update_region,
                                            &(cairo_rectangle_int_t) {
                                              0, 0,
                                              update_texture->width, update_texture->height
                                            });
          gdk_texture_set_diff (GDK_TEXTURE (this), update_texture, update_region);
        }
    }

  return GDK_TEXTURE (this);
}

static void
gdk_gl_texture_determine_format (GdkGLTexture *this)
{
  GdkTexture *texture = GDK_TEXTURE (this);
  GdkGLContext *context;
  GLint active_texture;
  GLint internal_format;
  GLint width, height;

  /* Abort if somebody else is GL-ing here... */
  context = gdk_gl_context_get_current ();
  if (context == NULL ||
      !gdk_gl_context_is_shared (this->context, context) ||
      /* ... or glGetTexLevelParameter() isn't supported */
      !gdk_gl_context_check_version (context, NULL, "3.1"))
    {
      texture->format = GDK_MEMORY_DEFAULT;
      this->has_mipmap = FALSE;
      return;
    }

  /* We need to be careful about modifying the GL context, as this is not
   * expected during construction */
  glGetIntegerv (GL_TEXTURE_BINDING_2D, &active_texture);
  glBindTexture (GL_TEXTURE_2D, this->id);

  glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);

  switch (internal_format)
  {
    case GL_RGB8:
    case GL_RGB:
      texture->format = GDK_MEMORY_R8G8B8;
      break;

    case GL_RGBA8:
      texture->format = GDK_MEMORY_R8G8B8A8_PREMULTIPLIED;
      break;

    case GL_RGB16:
      texture->format = GDK_MEMORY_R16G16B16;
      break;

    case GL_RGBA16:
      texture->format = GDK_MEMORY_R16G16B16A16_PREMULTIPLIED;
      break;

    case GL_RGB16F:
      texture->format = GDK_MEMORY_R16G16B16_FLOAT;
      break;

    case GL_RGBA16F:
      texture->format = GDK_MEMORY_R16G16B16A16_FLOAT_PREMULTIPLIED;
      break;

    case GL_RGB32F:
      texture->format = GDK_MEMORY_R32G32B32_FLOAT;
      break;

    case GL_RGBA32F:
      texture->format = GDK_MEMORY_R32G32B32A32_FLOAT_PREMULTIPLIED;
      break;

    case GL_RGBA:
      {
        GLint red_size = 0;
        GLint green_size = 0;
        GLint blue_size = 0;
        GLint alpha_size = 0;
        GLint red_type = 0;
        GLint green_type = 0;
        GLint blue_type = 0;
        GLint alpha_type = 0;

        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_RED_TYPE, &red_type);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_TYPE, &green_type);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_TYPE, &blue_type);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_TYPE, &alpha_type);

        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &red_size);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &green_size);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &blue_size);
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha_size);

#define CHECK_RGBA(rt,gt,bt,at,rs,gs,bs,as) \
        (red_type == rt && green_type == gt && blue_type == bt && alpha_type == at && \
         red_size == rs && green_size == gs && blue_size == bs && alpha_size == as)

        if (CHECK_RGBA (GL_UNSIGNED_NORMALIZED, GL_UNSIGNED_NORMALIZED, GL_UNSIGNED_NORMALIZED, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8))
          {
            texture->format = GDK_MEMORY_R8G8B8A8_PREMULTIPLIED;
            break;
          }

#undef CHECK_RGBA
      }

      G_GNUC_FALLTHROUGH;

    default:
      g_warning ("Texture in unexpected format 0x%X (%d). File a bug about adding it to GTK", internal_format, internal_format);
      /* fallback to the dumbest possible format
       * so that even age old GLES can do it */
      texture->format = GDK_MEMORY_R8G8B8A8_PREMULTIPLIED;
      break;
  }

  /* Determine if the texture has a mipmap.
   * We do this here, since it requires binding the texture,
   * and we're already doing that here.
   * GL has no way to directly query 'mipmap completeness' of textures,
   * so we just check that level 1 has the expected size, and assume
   * that means somebody called glGenerateMipmap().
   */
  glGetTexLevelParameteriv (GL_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv (GL_TEXTURE_2D, 1, GL_TEXTURE_HEIGHT, &height);

  this->has_mipmap = width == texture->width / 2 &&
                     height == texture->height / 2;

  /* restore previous state */
  glBindTexture (GL_TEXTURE_2D, active_texture);
}

/**
 * gdk_gl_texture_new:
 * @context: a `GdkGLContext`
 * @id: the ID of a texture that was created with @context
 * @width: the nominal width of the texture
 * @height: the nominal height of the texture
 * @destroy: a destroy notify that will be called when the GL resources
 *   are released
 * @data: data that gets passed to @destroy
 *
 * Creates a new texture for an existing GL texture.
 *
 * Note that the GL texture must not be modified until @destroy is called,
 * which will happen when the GdkTexture object is finalized, or due to
 * an explicit call of [method@Gdk.GLTexture.release].
 *
 * Return value: (transfer full) (type GdkGLTexture): A newly-created
 *   `GdkTexture`
 *
 * Deprecated: 4.12: [class@Gdk.GLTextureBuilder] supersedes this function
 *   and provides extended functionality for creating GL textures.
 */
GdkTexture *
gdk_gl_texture_new (GdkGLContext   *context,
                    guint           id,
                    int             width,
                    int             height,
                    GDestroyNotify  destroy,
                    gpointer        data)
{
  GdkGLTexture *this;

  g_return_val_if_fail (GDK_IS_GL_CONTEXT (context), NULL);
  g_return_val_if_fail (id != 0, NULL);
  g_return_val_if_fail (width > 0, NULL);
  g_return_val_if_fail (height > 0, NULL);

  this = g_object_new (GDK_TYPE_GL_TEXTURE,
                       "width", width,
                       "height", height,
                       "color-state", GDK_COLOR_STATE_SRGB,
                       NULL);

  this->context = g_object_ref (context);
  this->id = id;
  this->destroy = destroy;
  this->data = data;

  gdk_gl_texture_determine_format (this);

  return GDK_TEXTURE (this);
}

