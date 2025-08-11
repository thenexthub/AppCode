#include "config.h"

#include "gskglbufferprivate.h"

/* {{{ GskGLBuffer */

struct _GskGLBuffer
{
  GskGpuBuffer parent_instance;

  GLenum target;
  GLuint buffer_id;
  guchar *data;
};

G_DEFINE_TYPE (GskGLBuffer, gsk_gl_buffer, GSK_TYPE_GPU_BUFFER)

static void
gsk_gl_buffer_finalize (GObject *object)
{
  GskGLBuffer *this = GSK_GL_BUFFER (object);

  glDeleteBuffers (1, &this->buffer_id);

  G_OBJECT_CLASS (gsk_gl_buffer_parent_class)->finalize (object);
}

static guchar *
gsk_gl_buffer_map (GskGpuBuffer *buffer)
{
  GskGLBuffer *this = GSK_GL_BUFFER (buffer);

  return this->data;
}

static void
gsk_gl_buffer_unmap (GskGpuBuffer *buffer,
                     gsize         used)
{
}

static void
gsk_gl_buffer_class_init (GskGLBufferClass *klass)
{
  GskGpuBufferClass *buffer_class = GSK_GPU_BUFFER_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  buffer_class->map = gsk_gl_buffer_map;
  buffer_class->unmap = gsk_gl_buffer_unmap;

  gobject_class->finalize = gsk_gl_buffer_finalize;
}

static void
gsk_gl_buffer_init (GskGLBuffer *this)
{
}

void
gsk_gl_buffer_bind (GskGLBuffer *this)
{
  glBindBuffer (this->target, this->buffer_id);
}

void
gsk_gl_buffer_bind_range (GskGLBuffer *this,
                          GLuint       index,
                          GLintptr     offset,
                          GLsizeiptr   size)
{
  glBindBufferRange (this->target,
                     index,
                     this->buffer_id,
                     offset,
                     size);
}

static void
gsk_gl_buffer_setup (GskGLBuffer *this,
                     GLenum       target,
                     gsize        size)
{
  gsk_gpu_buffer_setup (GSK_GPU_BUFFER (this), size);

  this->target = target;

  glGenBuffers (1, &this->buffer_id);
}

/* }}} */
/* {{{ GskGLMappedBuffer */

struct _GskGLMappedBuffer
{
  GskGLBuffer parent_instance;
};

G_DEFINE_TYPE (GskGLMappedBuffer, gsk_gl_mapped_buffer, GSK_TYPE_GL_BUFFER)

static void
gsk_gl_mapped_buffer_finalize (GObject *object)
{
  GskGLBuffer *this = GSK_GL_BUFFER (object);

  gsk_gl_buffer_bind (this);
  glUnmapBuffer (this->target);

  G_OBJECT_CLASS (gsk_gl_mapped_buffer_parent_class)->finalize (object);
}

static void
gsk_gl_mapped_buffer_class_init (GskGLMappedBufferClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = gsk_gl_mapped_buffer_finalize;
}

static void
gsk_gl_mapped_buffer_init (GskGLMappedBuffer *this)
{
}

GskGpuBuffer *
gsk_gl_mapped_buffer_new (GLenum target,
                          gsize  size)
{
  GskGLBuffer *this;

  this = g_object_new (GSK_TYPE_GL_MAPPED_BUFFER, NULL);

  gsk_gl_buffer_setup (this, target, size);
  gsk_gl_buffer_bind (this);

  glBufferStorage (target,
                   size,
                   NULL,
                   GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
  this->data = glMapBufferRange (target,
                                 0,
                                 size,
                                 GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

  return GSK_GPU_BUFFER (this);
}

/* }}} */
/* {{{ GskGLCopiedBuffer */

struct _GskGLCopiedBuffer
{
  GskGLBuffer parent_instance;
};

G_DEFINE_TYPE (GskGLCopiedBuffer, gsk_gl_copied_buffer, GSK_TYPE_GL_BUFFER)

static void
gsk_gl_copied_buffer_finalize (GObject *object)
{
  GskGLBuffer *this = GSK_GL_BUFFER (object);

  g_free (this->data);

  G_OBJECT_CLASS (gsk_gl_copied_buffer_parent_class)->finalize (object);
}

static void
gsk_gl_copied_buffer_unmap (GskGpuBuffer *buffer,
                            gsize         used)
{
  GskGLBuffer *this = GSK_GL_BUFFER (buffer);

  if (used == 0)
    return;

  gsk_gl_buffer_bind (this);

  glBufferSubData (this->target, 0, used, this->data);
}

static void
gsk_gl_copied_buffer_class_init (GskGLCopiedBufferClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GskGpuBufferClass *buffer_class = GSK_GPU_BUFFER_CLASS (klass);

  gobject_class->finalize = gsk_gl_copied_buffer_finalize;

  buffer_class->unmap = gsk_gl_copied_buffer_unmap;
}

static void
gsk_gl_copied_buffer_init (GskGLCopiedBuffer *this)
{
}

GskGpuBuffer *
gsk_gl_copied_buffer_new (GLenum target,
                          gsize  size)
{
  GskGLBuffer *this;

  this = g_object_new (GSK_TYPE_GL_COPIED_BUFFER, NULL);

  gsk_gl_buffer_setup (this, target, size);
  gsk_gl_buffer_bind (this);

  glBufferData (target, size, NULL, GL_STATIC_DRAW);
  this->data = malloc (size);

  return GSK_GPU_BUFFER (this);
}

/* }}} */
