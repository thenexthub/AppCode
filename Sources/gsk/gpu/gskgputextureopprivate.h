#pragma once

#include "gskgpushaderopprivate.h"

#include <graphene.h>

G_BEGIN_DECLS

void                    gsk_gpu_texture_op                              (GskGpuFrame                    *frame,
                                                                         GskGpuShaderClip                clip,
                                                                         const graphene_point_t         *offset,
                                                                         const GskGpuShaderImage        *image);


G_END_DECLS

