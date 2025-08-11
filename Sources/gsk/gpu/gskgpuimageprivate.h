#pragma once

#include "gskgputypesprivate.h"

#include <graphene.h>

G_BEGIN_DECLS

#define GSK_TYPE_GPU_IMAGE         (gsk_gpu_image_get_type ())
#define GSK_GPU_IMAGE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GSK_TYPE_GPU_IMAGE, GskGpuImage))
#define GSK_GPU_IMAGE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), GSK_TYPE_GPU_IMAGE, GskGpuImageClass))
#define GSK_IS_GPU_IMAGE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GSK_TYPE_GPU_IMAGE))
#define GSK_IS_GPU_IMAGE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GSK_TYPE_GPU_IMAGE))
#define GSK_GPU_IMAGE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GSK_TYPE_GPU_IMAGE, GskGpuImageClass))

typedef struct _GskGpuImageClass GskGpuImageClass;

struct _GskGpuImage
{
  GObject parent_instance;
};

struct _GskGpuImageClass
{
  GObjectClass parent_class;

  void                  (* get_projection_matrix)                       (GskGpuImage            *this,
                                                                         graphene_matrix_t      *out_projection);
};

GType                   gsk_gpu_image_get_type                          (void) G_GNUC_CONST;

void                    gsk_gpu_image_setup                             (GskGpuImage            *this,
                                                                         GskGpuImageFlags        flags,
                                                                         GskGpuConversion        conversion,
                                                                         GdkShaderOp             shader_op,
                                                                         GdkMemoryFormat         format,
                                                                         gsize                   width,
                                                                         gsize                   height);
void                    gsk_gpu_image_toggle_ref_texture                (GskGpuImage            *this,
                                                                         GdkTexture             *texture);

GdkMemoryFormat         gsk_gpu_image_get_format                        (GskGpuImage            *this);
gsize                   gsk_gpu_image_get_width                         (GskGpuImage            *this);
gsize                   gsk_gpu_image_get_height                        (GskGpuImage            *this);
GskGpuImageFlags        gsk_gpu_image_get_flags                         (GskGpuImage            *this);
void                    gsk_gpu_image_set_flags                         (GskGpuImage            *this,
                                                                         GskGpuImageFlags        flags);
GskGpuConversion        gsk_gpu_image_get_conversion                    (GskGpuImage            *this);
GdkShaderOp             gsk_gpu_image_get_shader_op                     (GskGpuImage            *this);

void                    gsk_gpu_image_get_projection_matrix             (GskGpuImage            *this,
                                                                         graphene_matrix_t      *out_projection);


G_DEFINE_AUTOPTR_CLEANUP_FUNC(GskGpuImage, g_object_unref)

G_END_DECLS
