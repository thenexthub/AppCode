#include "config.h"

#include "gtkpagethumbnailprivate.h"

enum
{
  PROP_PAGE_NUM = 1,
  NUM_PROPERTIES,
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

struct _GtkPageThumbnail
{
  GtkWidget parent_instance;

  GtkWidget *label;
  int page_num;
};

struct _GtkPageThumbnailClass
{
  GtkWidgetClass parent_class;
};

G_DEFINE_TYPE (GtkPageThumbnail, gtk_page_thumbnail, GTK_TYPE_WIDGET)

static void
gtk_page_thumbnail_init (GtkPageThumbnail *this)
{
  this->label = gtk_inscription_new ("0");
  gtk_widget_set_parent (this->label, GTK_WIDGET (this));
  gtk_inscription_set_min_chars (GTK_INSCRIPTION (this->label), 1);
  gtk_inscription_set_nat_chars (GTK_INSCRIPTION (this->label), 1);
}

static void
gtk_page_thumbnail_dispose (GObject *object)
{
  GtkPageThumbnail *this = GTK_PAGE_THUMBNAIL (object);

  g_clear_pointer (&this->label, gtk_widget_unparent);

  G_OBJECT_CLASS (gtk_page_thumbnail_parent_class)->dispose (object);
}

static void
gtk_page_thumbnail_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GtkPageThumbnail *this = GTK_PAGE_THUMBNAIL (object);

  switch (prop_id)
    {
    case PROP_PAGE_NUM:
      gtk_page_thumbnail_set_page_num (this, g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_page_thumbnail_get_property (GObject     *object,
                                 guint        prop_id,
                                 GValue      *value,
                                 GParamSpec  *pspec)
{
  GtkPageThumbnail *this = GTK_PAGE_THUMBNAIL (object);

  switch (prop_id)
    {
    case PROP_PAGE_NUM:
      g_value_set_int (value, this->page_num);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_page_thumbnail_size_allocate (GtkWidget *widget,
                                  int        width,
                                  int        height,
                                  int        baseline)
{
  GtkPageThumbnail *this = GTK_PAGE_THUMBNAIL (widget);
  GtkRequisition nat;
  GtkAllocation alloc;

  gtk_widget_get_preferred_size (this->label, NULL, &nat);
  alloc.x = width - nat.width;
  alloc.y = height - nat.height;
  alloc.width = nat.width;
  alloc.height = nat.height;
  gtk_widget_size_allocate (this->label, &alloc, -1);
}

static void
gtk_page_thumbnail_class_init (GtkPageThumbnailClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->dispose = gtk_page_thumbnail_dispose;
  object_class->set_property = gtk_page_thumbnail_set_property;
  object_class->get_property = gtk_page_thumbnail_get_property;

  widget_class->size_allocate = gtk_page_thumbnail_size_allocate;

  properties[PROP_PAGE_NUM] =
      g_param_spec_int ("page-num", NULL, NULL,
                        0, G_MAXINT, 0, G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, NUM_PROPERTIES, properties);

  gtk_widget_class_set_css_name (widget_class, "page-thumbnail");
}

GtkPageThumbnail *
gtk_page_thumbnail_new (void)
{
  return g_object_new (GTK_TYPE_PAGE_THUMBNAIL, NULL);
}

void
gtk_page_thumbnail_set_page_num (GtkPageThumbnail *this,
                                 int               page_num)
{
  g_return_if_fail (GTK_IS_PAGE_THUMBNAIL (this));
  g_return_if_fail (page_num >= 0);
  char text[64];

  if (this->page_num == page_num)
    return;

  this->page_num = page_num;

  g_snprintf (text, sizeof (text), "%d", this->page_num);

  gtk_inscription_set_text (GTK_INSCRIPTION (this->label), text);
  gtk_widget_queue_draw (GTK_WIDGET (this));
  g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_PAGE_NUM]);
}

int
gtk_page_thumbnail_get_page_num (GtkPageThumbnail *this)
{
  g_return_val_if_fail (GTK_IS_PAGE_THUMBNAIL (this), 0);

  return this->page_num;
}
