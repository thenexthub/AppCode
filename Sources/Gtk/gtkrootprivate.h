#pragma once

#include "gtkroot.h"

#include "gtkconstraintsolverprivate.h"

G_BEGIN_DECLS

/**
 * GtkRootIface:
 *
 * The list of functions that must be implemented for the `GtkRoot` interface.
 */
struct _GtkRootInterface
{
  /*< private >*/
  GTypeInterface g_iface;

  /*< public >*/
  GdkDisplay * (* get_display)  (GtkRoot *this);

  GtkConstraintSolver * (* get_constraint_solver) (GtkRoot *this);

  GtkWidget *  (* get_focus)    (GtkRoot   *this);
  void         (* set_focus)    (GtkRoot   *this,
                                 GtkWidget *focus);

};

GtkConstraintSolver *   gtk_root_get_constraint_solver  (GtkRoot *this);

void             gtk_root_start_layout  (GtkRoot *this);
void             gtk_root_stop_layout   (GtkRoot *this);
void             gtk_root_queue_restyle (GtkRoot *this);

G_END_DECLS

