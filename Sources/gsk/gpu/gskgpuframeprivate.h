#pragma once

#include "gskgpurenderer.h"
#include "gskgputypesprivate.h"

G_BEGIN_DECLS

#define GSK_TYPE_GPU_FRAME         (gsk_gpu_frame_get_type ())
#define GSK_GPU_FRAME(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GSK_TYPE_GPU_FRAME, GskGpuFrame))
#define GSK_GPU_FRAME_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), GSK_TYPE_GPU_FRAME, GskGpuFrameClass))
#define GSK_IS_GPU_FRAME(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GSK_TYPE_GPU_FRAME))
#define GSK_IS_GPU_FRAME_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GSK_TYPE_GPU_FRAME))
#define GSK_GPU_FRAME_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GSK_TYPE_GPU_FRAME, GskGpuFrameClass))

typedef struct _GskGpuFrameClass GskGpuFrameClass;

struct _GskGpuFrame
{
  GObject parent_instance;
};

struct _GskGpuFrameClass
{
  GObjectClass parent_class;

  gboolean              (* is_busy)                                     (GskGpuFrame            *this);
  void                  (* wait)                                        (GskGpuFrame            *this);
  void                  (* setup)                                       (GskGpuFrame            *this);
  void                  (* cleanup)                                     (GskGpuFrame            *this);
  void                  (* begin)                                       (GskGpuFrame            *this,
                                                                         GdkDrawContext         *context,
                                                                         GdkMemoryDepth          depth,
                                                                         const cairo_region_t   *region,
                                                                         const graphene_rect_t  *opaque);
  void                  (* end)                                         (GskGpuFrame            *this,
                                                                         GdkDrawContext         *context);
  void                  (* sync)                                        (GskGpuFrame            *this);
  GskGpuImage *         (* upload_texture)                              (GskGpuFrame            *this,
                                                                         gboolean                with_mipmap,
                                                                         GdkTexture             *texture);
  GskGpuBuffer *        (* create_vertex_buffer)                        (GskGpuFrame            *this,
                                                                         gsize                   size);
  GskGpuBuffer *        (* create_globals_buffer)                       (GskGpuFrame            *this,
                                                                         gsize                   size);
  GskGpuBuffer *        (* create_storage_buffer)                       (GskGpuFrame            *this,
                                                                         gsize                   size);
  void                  (* write_texture_vertex_data)                   (GskGpuFrame            *this,
                                                                         guchar                 *data,
                                                                         GskGpuImage           **images,
                                                                         GskGpuSampler          *samplers,
                                                                         gsize                   n_images);
  void                  (* submit)                                      (GskGpuFrame            *this,
                                                                         GskRenderPassType       pass_type,
                                                                         GskGpuBuffer           *vertex_buffer,
                                                                         GskGpuBuffer           *globals_buffer,
                                                                         GskGpuOp               *op);
};

GType                   gsk_gpu_frame_get_type                          (void) G_GNUC_CONST;


void                    gsk_gpu_frame_setup                             (GskGpuFrame            *this,
                                                                         GskGpuRenderer         *renderer,
                                                                         GskGpuDevice           *device,
                                                                         GskGpuOptimizations     optimizations);
void                    gsk_gpu_frame_set_texture_vertex_size           (GskGpuFrame            *this,
                                                                         gsize                   texture_vertex_size);
gsize                   gsk_gpu_frame_get_texture_vertex_size           (GskGpuFrame            *this,
                                                                         gsize                   n_textures);

void                    gsk_gpu_frame_begin                             (GskGpuFrame            *this,
                                                                         GdkDrawContext         *context,
                                                                         GdkMemoryDepth          depth,
                                                                         const cairo_region_t   *region,
                                                                         const graphene_rect_t  *opaque);
void                    gsk_gpu_frame_end                               (GskGpuFrame            *this,
                                                                         GdkDrawContext         *context);
void                    gsk_gpu_frame_sync                              (GskGpuFrame            *this);

GdkDrawContext *        gsk_gpu_frame_get_context                       (GskGpuFrame            *this) G_GNUC_PURE;
GskGpuDevice *          gsk_gpu_frame_get_device                        (GskGpuFrame            *this) G_GNUC_PURE;
gint64                  gsk_gpu_frame_get_timestamp                     (GskGpuFrame            *this) G_GNUC_PURE;
gboolean                gsk_gpu_frame_should_optimize                   (GskGpuFrame            *this,
                                                                         GskGpuOptimizations     optimization) G_GNUC_PURE;

gpointer                gsk_gpu_frame_alloc_op                          (GskGpuFrame            *this,
                                                                         gsize                   size);
GskGpuImage *           gsk_gpu_frame_upload_texture                    (GskGpuFrame            *this,
                                                                         gboolean                with_mipmap,
                                                                         GdkTexture             *texture);
gsize                   gsk_gpu_frame_reserve_vertex_data               (GskGpuFrame            *this,
                                                                         gsize                   size);
guchar *                gsk_gpu_frame_get_vertex_data                   (GskGpuFrame            *this,
                                                                         gsize                   offset);
void                    gsk_gpu_frame_write_texture_vertex_data         (GskGpuFrame            *this,
                                                                         guchar                 *data,
                                                                         GskGpuImage           **images,
                                                                         GskGpuSampler          *samplers,
                                                                         gsize                   n_images);
gsize                   gsk_gpu_frame_add_globals                       (GskGpuFrame            *this,
                                                                         const GskGpuGlobalsInstance *globals);
GskGpuBuffer *          gsk_gpu_frame_write_storage_buffer              (GskGpuFrame            *this,
                                                                         const guchar           *data,
                                                                         gsize                   size,
                                                                         gsize                  *out_offset);

gboolean                gsk_gpu_frame_is_busy                           (GskGpuFrame            *this);
void                    gsk_gpu_frame_wait                              (GskGpuFrame            *this);

void                    gsk_gpu_frame_render                            (GskGpuFrame            *this,
                                                                         gint64                  timestamp,
                                                                         GskGpuImage            *target,
                                                                         GdkColorState          *target_color_state,
                                                                         cairo_region_t         *clip,
                                                                         GskRenderNode          *node,
                                                                         const graphene_rect_t  *viewport,
                                                                         GdkTexture            **texture);
gboolean                gsk_gpu_frame_download_texture                  (GskGpuFrame            *this,
                                                                         gint64                  timestamp,
                                                                         GdkTexture             *texture,
                                                                         guchar                 *data,
                                                                         const GdkMemoryLayout  *layout,
                                                                         GdkColorState          *color_state);
GskGpuOp               *gsk_gpu_frame_get_last_op                       (GskGpuFrame            *this);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GskGpuFrame, g_object_unref)

G_END_DECLS
