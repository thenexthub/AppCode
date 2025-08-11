/* GTK - The GIMP Toolkit
 *
 * Copyright (C) 2010  Intel Corporation
 * Copyright (C) 2010  RedHat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *      Emmanuele Bassi <ebassi@linux.intel.com>
 *      Matthias Clasen <mclasen@redhat.com>
 *
 * Based on similar code from Mx.
 */

/**
 * GtkSwitch:
 *
 * Shows a "light switch" that has two states: on or off.
 *
 * <picture>
 *   <source srcset="switch-dark.png" media="(prefers-color-scheme: dark)">
 *   <img alt="An example GtkSwitch" src="switch.png">
 * </picture>
 *
 * The user can control which state should be active by clicking the
 * empty area, or by dragging the slider.
 *
 * `GtkSwitch` can also express situations where the underlying state changes
 * with a delay. In this case, the slider position indicates the user's recent
 * change (represented by the [property@Gtk.Switch:active] property), while the
 * trough color indicates the present underlying state (represented by the
 * [property@Gtk.Switch:state] property).
 *
 * <picture>
 *   <source srcset="switch-state-dark.png" media="(prefers-color-scheme: dark)">
 *   <img alt="GtkSwitch with delayed state change" src="switch-state.png">
 * </picture>
 *
 * See [signal@Gtk.Switch::state-set] for details.
 *
 * # Shortcuts and Gestures
 *
 * `GtkSwitch` supports pan and drag gestures to move the slider.
 *
 * # CSS nodes
 *
 * ```
 * switch
 * ├── image
 * ├── image
 * ╰── slider
 * ```
 *
 * `GtkSwitch` has four css nodes, the main node with the name switch and
 * subnodes for the slider and the on and off images. Neither of them is
 * using any style classes.
 *
 * # Accessibility
 *
 * `GtkSwitch` uses the [enum@Gtk.AccessibleRole.switch] role.
 */

#include "config.h"

#include "gtkswitch.h"

#include "gtkactionable.h"
#include "gtkactionhelperprivate.h"
#include "gtkgestureclick.h"
#include "gtkgesturepan.h"
#include "gtkgesturesingle.h"
#include "gtkgizmoprivate.h"
#include "gtkimage.h"
#include "gtkcustomlayout.h"
#include "gtkmarshalers.h"
#include "gtkprivate.h"
#include "gtkprogresstrackerprivate.h"
#include "gtksettingsprivate.h"
#include "gtkwidgetprivate.h"

typedef struct _GtkSwitchClass   GtkSwitchClass;

struct _GtkSwitch
{
  GtkWidget parent_instance;

  GtkActionHelper *action_helper;

  GtkGesture *pan_gesture;
  GtkGesture *click_gesture;

  double handle_pos;
  guint tick_id;

  guint state                 : 1;
  guint is_active             : 1;

  GtkProgressTracker tracker;

  GtkWidget *on_image;
  GtkWidget *off_image;
  GtkWidget *slider;
};

struct _GtkSwitchClass
{
  GtkWidgetClass parent_class;

  void (* activate) (GtkSwitch *this);

  gboolean (* state_set) (GtkSwitch *this,
                          gboolean   state);
};

enum
{
  PROP_0,
  PROP_ACTIVE,
  PROP_STATE,
  LAST_PROP,
  PROP_ACTION_NAME,
  PROP_ACTION_TARGET
};

enum
{
  ACTIVATE,
  STATE_SET,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static GParamSpec *switch_props[LAST_PROP] = { NULL, };

static void gtk_switch_actionable_iface_init (GtkActionableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GtkSwitch, gtk_switch, GTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_ACTIONABLE,
                                                gtk_switch_actionable_iface_init))

static gboolean
is_right_side (GtkWidget *widget,
               gboolean   active)
{
  if (_gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR)
    return active;
  else
    return !active;
}

static void
gtk_switch_end_toggle_animation (GtkSwitch *this)
{
  if (this->tick_id != 0)
    {
      gtk_widget_remove_tick_callback (GTK_WIDGET (this), this->tick_id);
      this->tick_id = 0;
    }
}

static gboolean
gtk_switch_on_frame_clock_update (GtkWidget     *widget,
                                  GdkFrameClock *clock,
                                  gpointer       user_data)
{
  GtkSwitch *this = GTK_SWITCH (widget);
  double progress;

  gtk_progress_tracker_advance_frame (&this->tracker,
                                      gdk_frame_clock_get_frame_time (clock));

  if (gtk_progress_tracker_get_state (&this->tracker) != GTK_PROGRESS_STATE_AFTER)
    {
      progress = gtk_progress_tracker_get_ease_out_cubic (&this->tracker, FALSE);
      if (is_right_side (widget, this->is_active))
        this->handle_pos = 1.0 - progress;
      else
        this->handle_pos = progress;
    }
  else
    {
      gtk_switch_set_active (this, !this->is_active);
    }

  gtk_widget_queue_allocate (GTK_WIDGET (this));

  return G_SOURCE_CONTINUE;
}

#define ANIMATION_DURATION 100

static void
gtk_switch_begin_toggle_animation (GtkSwitch *this)
{
  if (gtk_settings_get_enable_animations (gtk_widget_get_settings (GTK_WIDGET (this))))
    {
      gtk_progress_tracker_start (&this->tracker, 1000 * ANIMATION_DURATION, 0, 1.0);
      if (this->tick_id == 0)
        this->tick_id = gtk_widget_add_tick_callback (GTK_WIDGET (this),
                                                      gtk_switch_on_frame_clock_update,
                                                      NULL, NULL);
    }
  else
    {
      gtk_switch_set_active (this, !this->is_active);
    }
}

static void
gtk_switch_click_gesture_pressed (GtkGestureClick *gesture,
                                  int              n_press,
                                  double           x,
                                  double           y,
                                  GtkSwitch       *this)
{
  graphene_rect_t switch_bounds;

  if (!gtk_widget_compute_bounds (GTK_WIDGET (this), GTK_WIDGET (this), &switch_bounds))
    return;

  /* If the press didn't happen in the draggable handle,
   * cancel the pan gesture right away
   */
  if ((this->is_active && x <= switch_bounds.size.width / 2.0) ||
      (!this->is_active && x > switch_bounds.size.width / 2.0))
    gtk_gesture_set_state (this->pan_gesture, GTK_EVENT_SEQUENCE_DENIED);
}

static void
gtk_switch_click_gesture_released (GtkGestureClick *gesture,
                                   int              n_press,
                                   double           x,
                                   double           y,
                                   GtkSwitch       *this)
{
  GdkEventSequence *sequence;

  sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));

  if (gtk_widget_contains (GTK_WIDGET (this), x, y) &&
      gtk_gesture_handles_sequence (GTK_GESTURE (gesture), sequence))
    {
      gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);
      gtk_switch_begin_toggle_animation (this);
    }
}

static void
gtk_switch_pan_gesture_pan (GtkGesturePan   *gesture,
                            GtkPanDirection  direction,
                            double           offset,
                            GtkSwitch       *this)
{
  GtkWidget *widget = GTK_WIDGET (this);
  int width;

  width = gtk_widget_get_width (widget);

  if (direction == GTK_PAN_DIRECTION_LEFT)
    offset = -offset;

  gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);

  if (is_right_side (widget, this->is_active))
    offset += width / 2;

  offset /= width / 2;
  /* constrain the handle within the trough width */
  this->handle_pos = CLAMP (offset, 0, 1.0);

  /* we need to redraw the handle */
  gtk_widget_queue_allocate (widget);
}

static void
gtk_switch_pan_gesture_drag_end (GtkGestureDrag *gesture,
                                 double          x,
                                 double          y,
                                 GtkSwitch      *this)
{
  GtkWidget *widget = GTK_WIDGET (this);
  GdkEventSequence *sequence;
  gboolean active;

  sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));

  if (gtk_gesture_get_sequence_state (GTK_GESTURE (gesture), sequence) == GTK_EVENT_SEQUENCE_CLAIMED)
    {
      /* if half the handle passed the middle of the switch, then we
       * consider it to be on
       */
      if (_gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR)
        active = this->handle_pos >= 0.5;
      else
        active = this->handle_pos <= 0.5;
    }
  else if (!gtk_gesture_handles_sequence (this->click_gesture, sequence))
    active = this->is_active;
  else
    return;

  this->handle_pos = is_right_side (widget, active) ? 1.0 : 0.0;
  gtk_switch_set_active (this, active);
  gtk_widget_queue_allocate (widget);
}

static void
gtk_switch_activate (GtkSwitch *this)
{
  gtk_switch_begin_toggle_animation (this);
}

static void
gtk_switch_measure (GtkWidget      *widget,
                    GtkOrientation  orientation,
                    int             for_size,
                    int            *minimum,
                    int            *natural,
                    int            *minimum_baseline,
                    int            *natural_baseline)
{
  GtkSwitch *this = GTK_SWITCH (widget);
  int slider_minimum, slider_natural;
  int on_nat, off_nat;
  int on_baseline, off_baseline;

  gtk_widget_measure (this->slider, orientation, -1,
                      &slider_minimum, &slider_natural,
                      NULL, NULL);

  gtk_widget_measure (this->on_image, orientation, for_size, NULL, &on_nat, NULL, &on_baseline);
  gtk_widget_measure (this->off_image, orientation, for_size, NULL, &off_nat, NULL, &off_baseline);

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      int text_width = MAX (on_nat, off_nat);
      *minimum = 2 * MAX (slider_minimum, text_width);
      *natural = 2 * MAX (slider_natural, text_width);
    }
  else
    {
      int text_height = MAX (on_nat, off_nat);

      *minimum = MAX (slider_minimum, text_height);
      *natural = MAX (slider_natural, text_height);

      *minimum_baseline = MAX (on_baseline, off_baseline) + MAX ((slider_minimum - text_height) / 2, 0);
      *natural_baseline = MAX (on_baseline, off_baseline) + MAX ((slider_natural - text_height) / 2, 0);
    }
}

static void
gtk_switch_allocate (GtkWidget *widget,
                     int        width,
                     int        height,
                     int        baseline)
{
  GtkSwitch *this = GTK_SWITCH (widget);
  GtkAllocation child_alloc;
  int min;

  gtk_widget_size_allocate (this->slider,
                            &(GtkAllocation) {
                              round (this->handle_pos * (width / 2)), 0,
                              width / 2, height
                            }, -1);

  /* Center ON icon in left half */
  gtk_widget_measure (this->on_image, GTK_ORIENTATION_HORIZONTAL, -1, &min, NULL, NULL, NULL);
  child_alloc.x = ((width / 2) - min) / 2;
  if (is_right_side (widget, FALSE))
    child_alloc.x += width / 2;
  child_alloc.width = min;
  gtk_widget_measure (this->on_image, GTK_ORIENTATION_VERTICAL, min, &min, NULL, NULL, NULL);
  child_alloc.y = (height - min) / 2;
  child_alloc.height = min;
  gtk_widget_size_allocate (this->on_image, &child_alloc, -1);

  /* Center OFF icon in right half */
  gtk_widget_measure (this->off_image, GTK_ORIENTATION_HORIZONTAL, -1, &min, NULL, NULL, NULL);
  child_alloc.x = ((width / 2) - min) / 2;
  if (is_right_side (widget, TRUE))
    child_alloc.x += width / 2;
  child_alloc.width = min;
  gtk_widget_measure (this->off_image, GTK_ORIENTATION_VERTICAL, min, &min, NULL, NULL, NULL);
  child_alloc.y = (height - min) / 2;
  child_alloc.height = min;
  gtk_widget_size_allocate (this->off_image, &child_alloc, -1);
}

static void
gtk_switch_direction_changed (GtkWidget       *widget,
                              GtkTextDirection previous_dir)
{
  GtkSwitch *this = GTK_SWITCH (widget);

  this->handle_pos = 1.0 - this->handle_pos;
  gtk_widget_queue_allocate (widget);

  GTK_WIDGET_CLASS (gtk_switch_parent_class)->direction_changed (widget, previous_dir);
}

static void
gtk_switch_set_action_name (GtkActionable *actionable,
                            const char    *action_name)
{
  GtkSwitch *this = GTK_SWITCH (actionable);

  if (!this->action_helper)
    this->action_helper = gtk_action_helper_new (actionable);

  gtk_action_helper_set_action_name (this->action_helper, action_name);
}

static void
gtk_switch_set_action_target_value (GtkActionable *actionable,
                                    GVariant      *action_target)
{
  GtkSwitch *this = GTK_SWITCH (actionable);

  if (!this->action_helper)
    this->action_helper = gtk_action_helper_new (actionable);

  gtk_action_helper_set_action_target_value (this->action_helper, action_target);
}

static const char *
gtk_switch_get_action_name (GtkActionable *actionable)
{
  GtkSwitch *this = GTK_SWITCH (actionable);

  return gtk_action_helper_get_action_name (this->action_helper);
}

static GVariant *
gtk_switch_get_action_target_value (GtkActionable *actionable)
{
  GtkSwitch *this = GTK_SWITCH (actionable);

  return gtk_action_helper_get_action_target_value (this->action_helper);
}

static void
gtk_switch_actionable_iface_init (GtkActionableInterface *iface)
{
  iface->get_action_name = gtk_switch_get_action_name;
  iface->set_action_name = gtk_switch_set_action_name;
  iface->get_action_target_value = gtk_switch_get_action_target_value;
  iface->set_action_target_value = gtk_switch_set_action_target_value;
}

static void
gtk_switch_set_property (GObject      *gobject,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  GtkSwitch *this = GTK_SWITCH (gobject);

  switch (prop_id)
    {
    case PROP_ACTIVE:
      gtk_switch_set_active (this, g_value_get_boolean (value));
      break;

    case PROP_STATE:
      gtk_switch_set_state (this, g_value_get_boolean (value));
      break;

    case PROP_ACTION_NAME:
      gtk_switch_set_action_name (GTK_ACTIONABLE (this), g_value_get_string (value));
      break;

    case PROP_ACTION_TARGET:
      gtk_switch_set_action_target_value (GTK_ACTIONABLE (this), g_value_get_variant (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
gtk_switch_get_property (GObject    *gobject,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  GtkSwitch *this = GTK_SWITCH (gobject);

  switch (prop_id)
    {
    case PROP_ACTIVE:
      g_value_set_boolean (value, this->is_active);
      break;

    case PROP_STATE:
      g_value_set_boolean (value, this->state);
      break;

    case PROP_ACTION_NAME:
      g_value_set_string (value, gtk_action_helper_get_action_name (this->action_helper));
      break;

    case PROP_ACTION_TARGET:
      g_value_set_variant (value, gtk_action_helper_get_action_target_value (this->action_helper));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
gtk_switch_dispose (GObject *object)
{
  GtkSwitch *this = GTK_SWITCH (object);

  g_clear_object (&this->action_helper);

  G_OBJECT_CLASS (gtk_switch_parent_class)->dispose (object);
}

static void
gtk_switch_finalize (GObject *object)
{
  GtkSwitch *this = GTK_SWITCH (object);

  gtk_switch_end_toggle_animation (this);

  gtk_widget_unparent (this->on_image);
  gtk_widget_unparent (this->off_image);
  gtk_widget_unparent (this->slider);

  G_OBJECT_CLASS (gtk_switch_parent_class)->finalize (object);
}

static gboolean
state_set (GtkSwitch *this,
           gboolean   state)
{
  if (this->action_helper)
    gtk_action_helper_activate (this->action_helper);

  gtk_switch_set_state (this, state);

  return TRUE;
}

static gboolean
translate_switch_shapes_to_opacity (GBinding     *binding,
                                    const GValue *from_value,
                                    GValue       *to_value,
                                    gpointer      user_data)
{
  gboolean visible = g_value_get_boolean (from_value);
  g_value_set_double (to_value, visible ? 1.0 : 0.0);

  return TRUE;
}

static void
gtk_switch_class_init (GtkSwitchClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /**
   * GtkSwitch:active:
   *
   * Whether the `GtkSwitch` widget is in its on or off state.
   */
  switch_props[PROP_ACTIVE] =
    g_param_spec_boolean ("active", NULL, NULL,
                          FALSE,
                          GTK_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY);

  /**
   * GtkSwitch:state:
   *
   * The backend state that is controlled by the switch.
   *
   * Applications should usually set the [property@Gtk.Switch:active] property,
   * except when indicating a change to the backend state which occurs
   * separately from the user's interaction.
   *
   * See [signal@Gtk.Switch::state-set] for details.
   */
  switch_props[PROP_STATE] =
    g_param_spec_boolean ("state", NULL, NULL,
                          FALSE,
                          GTK_PARAM_READWRITE|G_PARAM_EXPLICIT_NOTIFY);

  gobject_class->set_property = gtk_switch_set_property;
  gobject_class->get_property = gtk_switch_get_property;
  gobject_class->dispose = gtk_switch_dispose;
  gobject_class->finalize = gtk_switch_finalize;

  g_object_class_install_properties (gobject_class, LAST_PROP, switch_props);

  widget_class->direction_changed = gtk_switch_direction_changed;

  klass->activate = gtk_switch_activate;
  klass->state_set = state_set;

  /**
   * GtkSwitch::activate:
   * @widget: the object which received the signal
   *
   * Emitted to animate the switch.
   *
   * Applications should never connect to this signal,
   * but use the [property@Gtk.Switch:active] property.
   */
  signals[ACTIVATE] =
    g_signal_new (I_("activate"),
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GtkSwitchClass, activate),
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 0);

  gtk_widget_class_set_activate_signal (widget_class, signals[ACTIVATE]);

  /**
   * GtkSwitch::state-set:
   * @widget: the object on which the signal was emitted
   * @state: the new state of the switch
   *
   * Emitted to change the underlying state.
   *
   * The ::state-set signal is emitted when the user changes the switch
   * position. The default handler calls [method@Gtk.Switch.set_state] with the
   * value of @state.
   *
   * To implement delayed state change, applications can connect to this
   * signal, initiate the change of the underlying state, and call
   * [method@Gtk.Switch.set_state] when the underlying state change is
   * complete. The signal handler should return %TRUE to prevent the
   * default handler from running.
   *
   * Returns: %TRUE to stop the signal emission
   */
  signals[STATE_SET] =
    g_signal_new (I_("state-set"),
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GtkSwitchClass, state_set),
                  _gtk_boolean_handled_accumulator, NULL,
                  _gtk_marshal_BOOLEAN__BOOLEAN,
                  G_TYPE_BOOLEAN, 1,
                  G_TYPE_BOOLEAN);
  g_signal_set_va_marshaller (signals[STATE_SET],
                              G_TYPE_FROM_CLASS (gobject_class),
                              _gtk_marshal_BOOLEAN__BOOLEANv);

  g_object_class_override_property (gobject_class, PROP_ACTION_NAME, "action-name");
  g_object_class_override_property (gobject_class, PROP_ACTION_TARGET, "action-target");

  gtk_widget_class_set_css_name (widget_class, I_("switch"));

  gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_SWITCH);
}

static void
gtk_switch_init (GtkSwitch *this)
{
  GtkLayoutManager *layout;
  GtkGesture *gesture;
  GtkSettings *gtk_settings;

  gtk_widget_set_focusable (GTK_WIDGET (this), TRUE);

  gesture = gtk_gesture_click_new ();
  gtk_gesture_single_set_touch_only (GTK_GESTURE_SINGLE (gesture), FALSE);
  gtk_gesture_single_set_exclusive (GTK_GESTURE_SINGLE (gesture), TRUE);
  g_signal_connect (gesture, "pressed",
                    G_CALLBACK (gtk_switch_click_gesture_pressed), this);
  g_signal_connect (gesture, "released",
                    G_CALLBACK (gtk_switch_click_gesture_released), this);
  gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (gesture),
                                              GTK_PHASE_BUBBLE);
  gtk_widget_add_controller (GTK_WIDGET (this), GTK_EVENT_CONTROLLER (gesture));
  this->click_gesture = gesture;

  gesture = gtk_gesture_pan_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_gesture_single_set_touch_only (GTK_GESTURE_SINGLE (gesture), FALSE);
  gtk_gesture_single_set_exclusive (GTK_GESTURE_SINGLE (gesture), TRUE);
  g_signal_connect (gesture, "pan",
                    G_CALLBACK (gtk_switch_pan_gesture_pan), this);
  g_signal_connect (gesture, "drag-end",
                    G_CALLBACK (gtk_switch_pan_gesture_drag_end), this);
  gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (gesture),
                                              GTK_PHASE_CAPTURE);
  gtk_widget_add_controller (GTK_WIDGET (this), GTK_EVENT_CONTROLLER (gesture));
  this->pan_gesture = gesture;

  layout = gtk_custom_layout_new (NULL,
                                  gtk_switch_measure,
                                  gtk_switch_allocate);
  gtk_widget_set_layout_manager (GTK_WIDGET (this), layout);

  gtk_settings = gtk_settings_get_default ();

  this->on_image = g_object_new (GTK_TYPE_IMAGE,
                                 "accessible-role", GTK_ACCESSIBLE_ROLE_NONE,
                                 "icon-name", "switch-on-symbolic",
                                 NULL);
  gtk_widget_set_parent (this->on_image, GTK_WIDGET (this));

  g_object_bind_property_full (gtk_settings, "gtk-show-status-shapes",
                               this->on_image, "opacity",
                               G_BINDING_SYNC_CREATE,
                               translate_switch_shapes_to_opacity,
                               NULL, NULL, NULL);

  this->off_image = g_object_new (GTK_TYPE_IMAGE,
                                  "accessible-role", GTK_ACCESSIBLE_ROLE_NONE,
                                  "icon-name", "switch-off-symbolic",
                                  NULL);
  gtk_widget_set_parent (this->off_image, GTK_WIDGET (this));

  g_object_bind_property_full (gtk_settings, "gtk-show-status-shapes",
                               this->off_image, "opacity",
                               G_BINDING_SYNC_CREATE,
                               translate_switch_shapes_to_opacity,
                               NULL, NULL, NULL);

  this->slider = gtk_gizmo_new_with_role ("slider",
                                          GTK_ACCESSIBLE_ROLE_NONE,
                                          NULL, NULL, NULL, NULL, NULL, NULL);
  gtk_widget_set_parent (this->slider, GTK_WIDGET (this));

  gtk_accessible_update_state (GTK_ACCESSIBLE (this),
                               GTK_ACCESSIBLE_STATE_CHECKED, FALSE,
                               -1);
  if (is_right_side (GTK_WIDGET (this), FALSE))
    this->handle_pos = 1.0;
  else
    this->handle_pos = 0.0;
}

/**
 * gtk_switch_new:
 *
 * Creates a new `GtkSwitch` widget.
 *
 * Returns: the newly created `GtkSwitch` instance
 */
GtkWidget *
gtk_switch_new (void)
{
  return g_object_new (GTK_TYPE_SWITCH, NULL);
}

/**
 * gtk_switch_set_active:
 * @this: a `GtkSwitch`
 * @is_active: %TRUE if @this should be active, and %FALSE otherwise
 *
 * Changes the state of @this to the desired one.
 */
void
gtk_switch_set_active (GtkSwitch *this,
                       gboolean   is_active)
{
  g_return_if_fail (GTK_IS_SWITCH (this));

  gtk_switch_end_toggle_animation (this);

  is_active = !!is_active;

  if (this->is_active != is_active)
    {
      gboolean handled;

      this->is_active = is_active;

      if (is_right_side (GTK_WIDGET (this), this->is_active))
        this->handle_pos = 1.0;
      else
        this->handle_pos = 0.0;

      g_signal_emit (this, signals[STATE_SET], 0, is_active, &handled);

      g_object_notify_by_pspec (G_OBJECT (this), switch_props[PROP_ACTIVE]);

      gtk_accessible_update_state (GTK_ACCESSIBLE (this),
                                   GTK_ACCESSIBLE_STATE_CHECKED, is_active,
                                   -1);

      gtk_widget_queue_allocate (GTK_WIDGET (this));
    }
}

/**
 * gtk_switch_get_active:
 * @this: a `GtkSwitch`
 *
 * Gets whether the `GtkSwitch` is in its “on” or “off” state.
 *
 * Returns: %TRUE if the `GtkSwitch` is active, and %FALSE otherwise
 */
gboolean
gtk_switch_get_active (GtkSwitch *this)
{
  g_return_val_if_fail (GTK_IS_SWITCH (this), FALSE);

  return this->is_active;
}

/**
 * gtk_switch_set_state:
 * @this: a `GtkSwitch`
 * @state: the new state
 *
 * Sets the underlying state of the `GtkSwitch`.
 *
 * This function is typically called from a [signal@Gtk.Switch::state-set]
 * signal handler in order to set up delayed state changes.
 *
 * See [signal@Gtk.Switch::state-set] for details.
 */
void
gtk_switch_set_state (GtkSwitch *this,
                      gboolean   state)
{
  g_return_if_fail (GTK_IS_SWITCH (this));

  state = state != FALSE;

  if (this->state == state)
    return;

  this->state = state;

  if (state)
    gtk_widget_set_state_flags (GTK_WIDGET (this), GTK_STATE_FLAG_CHECKED, FALSE);
  else
    gtk_widget_unset_state_flags (GTK_WIDGET (this), GTK_STATE_FLAG_CHECKED);

  g_object_notify_by_pspec (G_OBJECT (this), switch_props[PROP_STATE]);
}

/**
 * gtk_switch_get_state:
 * @this: a `GtkSwitch`
 *
 * Gets the underlying state of the `GtkSwitch`.
 *
 * Returns: the underlying state
 */
gboolean
gtk_switch_get_state (GtkSwitch *this)
{
  g_return_val_if_fail (GTK_IS_SWITCH (this), FALSE);

  return this->state;
}
