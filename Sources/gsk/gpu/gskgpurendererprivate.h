#pragma once

#include "gskgpurenderer.h"

#include "gskgputypesprivate.h"
#include "gskrendererprivate.h"

G_BEGIN_DECLS

#define GSK_GPU_RENDERER_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GSK_TYPE_GPU_RENDERER, GskGpuRendererClass))
#define GSK_IS_GPU_RENDERER_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GSK_TYPE_GPU_RENDERER))
#define GSK_GPU_RENDERER_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GSK_TYPE_GPU_RENDERER, GskGpuRendererClass))

typedef struct _GskGpuRendererClass           GskGpuRendererClass;

struct _GskGpuRenderer
{
  GskRenderer parent_instance;
};

struct _GskGpuRendererClass
{
  GskRendererClass parent_class;

  GType frame_type;
  GskGpuOptimizations optimizations; /* subclasses cannot override this */

  GskGpuDevice *        (* get_device)                                  (GdkDisplay             *display,
                                                                         GError                **error);
  GdkDrawContext *      (* create_context)                              (GskGpuRenderer         *this,
                                                                         GdkDisplay             *display,
                                                                         GdkSurface             *surface,
                                                                         GskGpuOptimizations    *supported,
                                                                         GError                **error);

  void                  (* make_current)                                (GskGpuRenderer         *this);
  gpointer              (* save_current)                                (GskGpuRenderer         *this);
  void                  (* restore_current)                             (GskGpuRenderer         *this,
                                                                         gpointer                current);
  GskGpuImage *         (* get_backbuffer)                              (GskGpuRenderer         *this);
};

GdkDrawContext *        gsk_gpu_renderer_get_context                    (GskGpuRenderer         *this);
GskGpuDevice *          gsk_gpu_renderer_get_device                     (GskGpuRenderer         *this);

G_END_DECLS

