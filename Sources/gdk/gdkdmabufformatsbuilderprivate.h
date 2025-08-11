#pragma once

#include "gdkdmabufformats.h"

typedef struct GdkDmabufFormatsBuilder GdkDmabufFormatsBuilder;

GdkDmabufFormatsBuilder *       gdk_dmabuf_formats_builder_new                  (void);
GdkDmabufFormats *              gdk_dmabuf_formats_builder_free_to_formats      (GdkDmabufFormatsBuilder        *this);

void                            gdk_dmabuf_formats_builder_add_format           (GdkDmabufFormatsBuilder        *this,
                                                                                 guint32                         fourcc,
                                                                                 guint64                         modifier);
void                            gdk_dmabuf_formats_builder_add_formats          (GdkDmabufFormatsBuilder        *this,
                                                                                 GdkDmabufFormats               *formats);
