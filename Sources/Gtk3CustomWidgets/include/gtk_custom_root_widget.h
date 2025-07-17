#ifndef __ROOT_WIDGET_H__
#define __ROOT_WIDGET_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <glib-object.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    gint width;
    gint height;
} Size;

typedef struct _GtkCustomRootWidget {
    GtkBin parent_instance;
  
    gint minimum_width;
    gint minimum_height;
    gint natural_width;
    gint natural_height;
    gint allocated_width;
    gint allocated_height;
    gboolean has_been_allocated;

    void *resize_callback_data;
    void (*resize_callback)(void*, Size);

    GtkWidget *child;
} GtkCustomRootWidget;

#define GTK_CUSTOM_ROOT_WIDGET_TYPE (gtk_custom_root_widget_get_type())

G_DECLARE_FINAL_TYPE(GtkCustomRootWidget, gtk_custom_root_widget, GTK, CUSTOM_ROOT_WIDGET, GtkBin)

void gtk_custom_root_widget_realize(GtkWidget *widget);

GtkSizeRequestMode gtk_custom_root_widget_size_request_mode(GtkWidget *widget);

void gtk_custom_root_widget_get_preferred_width(
    GtkWidget *widget,
    int *minimum,
    int *natural
);

void gtk_custom_root_widget_get_preferred_height(
    GtkWidget *widget,
    int *minimum,
    int *natural
);

void gtk_custom_root_widget_size_allocate(
    GtkWidget *widget,
    GtkAllocation *allocation
);

GtkWidget *gtk_custom_root_widget_new(void);

void gtk_custom_root_widget_set_child(GtkCustomRootWidget *this, GtkWidget *child);

void gtk_custom_root_widget_get_size(GtkCustomRootWidget *widget, gint *width, gint *height);

void gtk_custom_root_widget_set_minimum_size(
    GtkCustomRootWidget *this,
    gint minimum_width,
    gint minimum_height
);

void gtk_custom_root_widget_set_natural_size(
    GtkCustomRootWidget *this,
    gint natural_width,
    gint natural_height
);

void gtk_custom_root_widget_preempt_allocated_size(
    GtkCustomRootWidget *this,
    gint allocated_width,
    gint allocated_height
);

void gtk_custom_root_widget_set_resize_callback(
    GtkCustomRootWidget *this,
    void (*callback)(void*, Size),
    void *data
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ROOT_WIDGET_H__ */
