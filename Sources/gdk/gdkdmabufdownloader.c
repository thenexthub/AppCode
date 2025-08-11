#include "config.h"

#include "gdkdmabufdownloaderprivate.h"

G_DEFINE_INTERFACE (GdkDmabufDownloader, gdk_dmabuf_downloader, G_TYPE_OBJECT)

static void
gdk_dmabuf_downloader_default_init (GdkDmabufDownloaderInterface *iface)
{
}

void
gdk_dmabuf_downloader_close (GdkDmabufDownloader *this)
{
  GdkDmabufDownloaderInterface *iface;

  iface = GDK_DMABUF_DOWNLOADER_GET_IFACE (this);
  iface->close (this);
}

gboolean
gdk_dmabuf_downloader_download (GdkDmabufDownloader   *this,
                                GdkDmabufTexture      *texture,
                                guchar                *data,
                                const GdkMemoryLayout *layout,
                                GdkColorState         *color_state)
{
  GdkDmabufDownloaderInterface *iface;

  g_return_val_if_fail (GDK_IS_DMABUF_DOWNLOADER (this), FALSE);

  iface = GDK_DMABUF_DOWNLOADER_GET_IFACE (this);
  return iface->download (this, texture, data, layout, color_state);
}
