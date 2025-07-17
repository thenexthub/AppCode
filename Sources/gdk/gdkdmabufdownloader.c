#include "config.h"

#include "gdkdmabufdownloaderprivate.h"

G_DEFINE_INTERFACE (GdkDmabufDownloader, gdk_dmabuf_downloader, G_TYPE_OBJECT)

static void
gdk_dmabuf_downloader_default_init (GdkDmabufDownloaderInterface *iface)
{
}

void
gdk_dmabuf_downloader_close (GdkDmabufDownloader *self)
{
  GdkDmabufDownloaderInterface *iface;

  iface = GDK_DMABUF_DOWNLOADER_GET_IFACE (self);
  iface->close (self);
}

gboolean
gdk_dmabuf_downloader_download (GdkDmabufDownloader   *self,
                                GdkDmabufTexture      *texture,
                                guchar                *data,
                                const GdkMemoryLayout *layout,
                                GdkColorState         *color_state)
{
  GdkDmabufDownloaderInterface *iface;

  g_return_val_if_fail (GDK_IS_DMABUF_DOWNLOADER (self), FALSE);

  iface = GDK_DMABUF_DOWNLOADER_GET_IFACE (self);
  return iface->download (self, texture, data, layout, color_state);
}
