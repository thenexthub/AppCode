/* gtkshortcutlabel.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gtkshortcutlabel.h"
#include "gtkboxlayout.h"
#include "gtklabel.h"
#include "gtkframe.h"
#include "gtkwidgetprivate.h"
#include <glib/gi18n-lib.h>
#include "gtkprivate.h"

/**
 * GtkShortcutLabel:
 *
 * `GtkShortcutLabel` displays a single keyboard shortcut or gesture.
 *
 * The main use case for `GtkShortcutLabel` is inside a [class@Gtk.ShortcutsWindow].
 *
 * Deprecated: 4.18: This widget will be removed in GTK 5
 */

struct _GtkShortcutLabel
{
  GtkWidget parent_instance;
  char   *accelerator;
  char   *disabled_text;
};

struct _GtkShortcutLabelClass
{
  GtkWidgetClass parent_class;
};

G_DEFINE_TYPE (GtkShortcutLabel, gtk_shortcut_label, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_ACCELERATOR,
  PROP_DISABLED_TEXT,
  LAST_PROP
};

static GParamSpec *properties[LAST_PROP];

static char *
get_modifier_label (guint key)
{
  const char *subscript;
  const char *label;

  switch (key)
    {
    case GDK_KEY_Shift_L:
    case GDK_KEY_Control_L:
    case GDK_KEY_Alt_L:
    case GDK_KEY_Meta_L:
    case GDK_KEY_Super_L:
    case GDK_KEY_Hyper_L:
      /* Translators: This string is used to mark left/right variants of modifier
       * keys in the shortcut window (e.g. Control_L vs Control_R). Please keep
       * this string very short, ideally just a single character, since it will
       * be rendered as part of the key.
       */
      subscript = C_("keyboard side marker", "L");
      break;
    case GDK_KEY_Shift_R:
    case GDK_KEY_Control_R:
    case GDK_KEY_Alt_R:
    case GDK_KEY_Meta_R:
    case GDK_KEY_Super_R:
    case GDK_KEY_Hyper_R:
      /* Translators: This string is used to mark left/right variants of modifier
       * keys in the shortcut window (e.g. Control_L vs Control_R). Please keep
       * this string very short, ideally just a single character, since it will
       * be rendered as part of the key.
       */
      subscript = C_("keyboard side marker", "R");
      break;
    default:
      g_assert_not_reached ();
   }

 switch (key)
   {
   case GDK_KEY_Shift_L:   case GDK_KEY_Shift_R:
     label = C_("keyboard label", "Shift");
     break;
   case GDK_KEY_Control_L: case GDK_KEY_Control_R:
     label = C_("keyboard label", "Ctrl");
     break;
   case GDK_KEY_Alt_L:     case GDK_KEY_Alt_R:
     label = C_("keyboard label", "Alt");
     break;
   case GDK_KEY_Meta_L:    case GDK_KEY_Meta_R:
     label = C_("keyboard label", "Meta");
     break;
   case GDK_KEY_Super_L:   case GDK_KEY_Super_R:
     label = C_("keyboard label", "Super");
     break;
   case GDK_KEY_Hyper_L:   case GDK_KEY_Hyper_R:
     label = C_("keyboard label", "Hyper");
     break;
    default:
      g_assert_not_reached ();
   }

  return g_strdup_printf ("%s <small><b>%s</b></small>", label, subscript);
}

static char **
get_labels (guint key, GdkModifierType modifier, guint *n_mods)
{
  const char *labels[16];
  GList *freeme = NULL;
  char key_label[16];
  const char *tmp;
  gunichar ch;
  int i = 0;
  char **retval;

  if (modifier & GDK_SHIFT_MASK)
    labels[i++] = C_("keyboard label", "Shift");
  if (modifier & GDK_CONTROL_MASK)
    labels[i++] = C_("keyboard label", "Ctrl");
  if (modifier & GDK_ALT_MASK)
    labels[i++] = C_("keyboard label", "Alt");
  if (modifier & GDK_SUPER_MASK)
    labels[i++] = C_("keyboard label", "Super");
  if (modifier & GDK_HYPER_MASK)
    labels[i++] = C_("keyboard label", "Hyper");
  if (modifier & GDK_META_MASK)
#ifndef GDK_WINDOWING_MACOS
    labels[i++] = C_("keyboard label", "Meta");
#else
    labels[i++] = "⌘";
#endif

  *n_mods = i;

  ch = gdk_keyval_to_unicode (key);
  if (ch && ch < 0x80 && g_unichar_isgraph (ch))
    {
      switch (ch)
        {
        case '<':
          labels[i++] = "&lt;";
          break;
        case '>':
          labels[i++] = "&gt;";
          break;
        case '&':
          labels[i++] = "&amp;";
          break;
        case '"':
          labels[i++] = "&quot;";
          break;
        case '\'':
          labels[i++] = "&apos;";
          break;
        case '\\':
          labels[i++] = C_("keyboard label", "Backslash");
          break;
        default:
          memset (key_label, 0, sizeof (key_label));
          if (key >= GDK_KEY_KP_Space && key <= GDK_KEY_KP_9)
            {
              key_label[0] = 'K';
              key_label[1] = 'P';
              key_label[2] = ' ';
              g_unichar_to_utf8 (g_unichar_toupper (ch), key_label + 3);
            }
          else
            {
              g_unichar_to_utf8 (g_unichar_toupper (ch), key_label);
            }
          labels[i++] = key_label;
          break;
        }
    }
  else
    {
      switch (key)
        {
        case GDK_KEY_Shift_L:   case GDK_KEY_Shift_R:
        case GDK_KEY_Control_L: case GDK_KEY_Control_R:
        case GDK_KEY_Alt_L:     case GDK_KEY_Alt_R:
        case GDK_KEY_Meta_L:    case GDK_KEY_Meta_R:
        case GDK_KEY_Super_L:   case GDK_KEY_Super_R:
        case GDK_KEY_Hyper_L:   case GDK_KEY_Hyper_R:
          freeme = g_list_prepend (freeme, get_modifier_label (key));
          labels[i++] = (const char *)freeme->data;
           break;
        case GDK_KEY_Left:
          labels[i++] = "\xe2\x86\x90";
          break;
        case GDK_KEY_Up:
          labels[i++] = "\xe2\x86\x91";
          break;
        case GDK_KEY_Right:
          labels[i++] = "\xe2\x86\x92";
          break;
        case GDK_KEY_Down:
          labels[i++] = "\xe2\x86\x93";
          break;
        case GDK_KEY_space:
          labels[i++] = "\xe2\x90\xa3";
          break;
        case GDK_KEY_Return:
          labels[i++] = "\xe2\x8f\x8e";
          break;
        case GDK_KEY_Page_Up:
          labels[i++] = C_("keyboard label", "Page_Up");
          break;
        case GDK_KEY_Page_Down:
          labels[i++] = C_("keyboard label", "Page_Down");
          break;
        default:
          tmp = gdk_keyval_name (gdk_keyval_to_lower (key));
          if (tmp != NULL)
            {
              if (tmp[0] != 0 && tmp[1] == 0)
                {
                  key_label[0] = g_ascii_toupper (tmp[0]);
                  key_label[1] = '\0';
                  labels[i++] = key_label;
                }
              else
                {
                  labels[i++] = g_dpgettext2 (GETTEXT_PACKAGE, "keyboard label", tmp);
                }
            }
        }
    }

  labels[i] = NULL;

  retval = g_strdupv ((char **)labels);

  g_list_free_full (freeme, g_free);

  return retval;
}

static GtkWidget *
dim_label (const char *text)
{
  GtkWidget *label;

  label = gtk_label_new (text);
  gtk_widget_add_css_class (label, "dim-label");

  return label;
}

static void
display_shortcut (GtkWidget       *this,
                  guint            key,
                  GdkModifierType  modifier)
{
  char **keys = NULL;
  int i;
  guint n_mods;

  keys = get_labels (key, modifier, &n_mods);
  for (i = 0; keys[i]; i++)
    {
      GtkWidget *disp;

      if (i > 0)
        gtk_widget_set_parent (dim_label ("+"), this);

      disp = gtk_label_new (keys[i]);
      if (i < n_mods)
        gtk_widget_set_size_request (disp, 50, -1);

      gtk_widget_add_css_class (disp, "keycap");
      gtk_label_set_use_markup (GTK_LABEL (disp), TRUE);

      gtk_widget_set_parent (disp, this);
    }
  g_strfreev (keys);
}

static gboolean
parse_combination (GtkShortcutLabel *this,
                   const char       *str)
{
  char **accels;
  int k;
  GdkModifierType modifier = 0;
  guint key = 0;
  gboolean retval = TRUE;

  accels = g_strsplit (str, "&", 0);
  for (k = 0; accels[k]; k++)
    {
      if (!gtk_accelerator_parse (accels[k], &key, &modifier))
        {
          retval = FALSE;
          break;
        }
      if (k > 0)
        gtk_widget_set_parent (dim_label ("+"), GTK_WIDGET (this));

      display_shortcut (GTK_WIDGET (this), key, modifier);
    }
  g_strfreev (accels);

  return retval;
}

static gboolean
parse_sequence (GtkShortcutLabel *this,
                const char       *str)
{
  char **accels;
  int k;
  gboolean retval = TRUE;

  accels = g_strsplit (str, "+", 0);
  for (k = 0; accels[k]; k++)
    {
      if (!parse_combination (this, accels[k]))
        {
          retval = FALSE;
          break;
        }
    }

  g_strfreev (accels);

  return retval;
}

static gboolean
parse_range (GtkShortcutLabel *this,
             const char       *str)
{
  char *dots;

  dots = strstr (str, "...");
  if (!dots)
    return parse_sequence (this, str);

  dots[0] = '\0';
  if (!parse_sequence (this, str))
    return FALSE;

  gtk_widget_set_parent (dim_label ("⋯"), GTK_WIDGET (this));

  if (!parse_sequence (this, dots + 3))
    return FALSE;

  return TRUE;
}

static void
clear_children (GtkShortcutLabel *this)
{
  GtkWidget *child;

  child = gtk_widget_get_first_child (GTK_WIDGET (this));

  while (child)
    {
      GtkWidget *next = gtk_widget_get_next_sibling (child);

      gtk_widget_unparent (child);

      child = next;
    }
}

static void
gtk_shortcut_label_rebuild (GtkShortcutLabel *this)
{
  char **accels;
  int k;
  GtkAccessibleRelation relation = GTK_ACCESSIBLE_RELATION_LABELLED_BY;
  GValue value = G_VALUE_INIT;
  GList *parts = NULL;
  GtkWidget *child;

  gtk_accessible_reset_relation (GTK_ACCESSIBLE (this), GTK_ACCESSIBLE_RELATION_LABELLED_BY);

  clear_children (this);

  if (this->accelerator == NULL || this->accelerator[0] == '\0')
    {
      GtkWidget *label;

      label = dim_label (this->disabled_text);

      gtk_widget_set_parent (label, GTK_WIDGET (this));
      return;
    }

  accels = g_strsplit (this->accelerator, " ", 0);
  for (k = 0; accels[k]; k++)
    {
      if (k > 0)
        gtk_widget_set_parent (dim_label ("/"), GTK_WIDGET (this));

      if (!parse_range (this, accels[k]))
        {
          g_warning ("Failed to parse %s, part of accelerator '%s'", accels[k], this->accelerator);
          break;
        }
    }
  g_strfreev (accels);

  /* All of the child labels are a part of our a11y label */
  for(child = gtk_widget_get_last_child (GTK_WIDGET (this));
      child != NULL;
      child = gtk_widget_get_prev_sibling (child))
    {
      parts = g_list_prepend (parts, child);
    }
  gtk_accessible_relation_init_value (relation, &value);
  g_value_set_pointer (&value, parts);
  gtk_accessible_update_relation_value (GTK_ACCESSIBLE (this),
                                        1, &relation, &value);

}

static void
gtk_shortcut_label_finalize (GObject *object)
{
  GtkShortcutLabel *this = (GtkShortcutLabel *)object;

  g_free (this->accelerator);
  g_free (this->disabled_text);

  clear_children (this);

  G_OBJECT_CLASS (gtk_shortcut_label_parent_class)->finalize (object);
}

static void
gtk_shortcut_label_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GtkShortcutLabel *this = GTK_SHORTCUT_LABEL (object);

  switch (prop_id)
    {
    case PROP_ACCELERATOR:
      g_value_set_string (value, gtk_shortcut_label_get_accelerator (this));
      break;

    case PROP_DISABLED_TEXT:
      g_value_set_string (value, gtk_shortcut_label_get_disabled_text (this));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtk_shortcut_label_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GtkShortcutLabel *this = GTK_SHORTCUT_LABEL (object);

  switch (prop_id)
    {
    case PROP_ACCELERATOR:
      gtk_shortcut_label_set_accelerator (this, g_value_get_string (value));
      break;

    case PROP_DISABLED_TEXT:
      gtk_shortcut_label_set_disabled_text (this, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtk_shortcut_label_class_init (GtkShortcutLabelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gtk_shortcut_label_finalize;
  object_class->get_property = gtk_shortcut_label_get_property;
  object_class->set_property = gtk_shortcut_label_set_property;

  /**
   * GtkShortcutLabel:accelerator:
   *
   * The accelerator that @this displays.
   *
   * See [property@Gtk.ShortcutsShortcut:accelerator]
   * for the accepted syntax.
   *
   * Deprecated: 4.18: This widget will be removed in GTK 5
   */
  properties[PROP_ACCELERATOR] =
    g_param_spec_string ("accelerator", NULL, NULL,
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  /**
   * GtkShortcutLabel:disabled-text:
   *
   * The text that is displayed when no accelerator is set.
   *
   * Deprecated: 4.18: This widget will be removed in GTK 5
   */
  properties[PROP_DISABLED_TEXT] =
    g_param_spec_string ("disabled-text", NULL, NULL,
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, properties);

  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);
  gtk_widget_class_set_css_name (widget_class, I_("shortcut"));
  gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_GROUP);
}

static void
gtk_shortcut_label_init (GtkShortcutLabel *this)
{
  /* Always use LTR so that modifiers are always left to the keyval */
  gtk_widget_set_direction (GTK_WIDGET (this), GTK_TEXT_DIR_LTR);
}

/**
 * gtk_shortcut_label_new:
 * @accelerator: the initial accelerator
 *
 * Creates a new `GtkShortcutLabel` with @accelerator set.
 *
 * Returns: a newly-allocated `GtkShortcutLabel`
 *
 * Deprecated: 4.18: This widget will be removed in GTK 5
 */
GtkWidget *
gtk_shortcut_label_new (const char *accelerator)
{
  return g_object_new (GTK_TYPE_SHORTCUT_LABEL,
                       "accelerator", accelerator,
                       NULL);
}

/**
 * gtk_shortcut_label_get_accelerator:
 * @this: a `GtkShortcutLabel`
 *
 * Retrieves the current accelerator of @this.
 *
 * Returns: (transfer none)(nullable): the current accelerator.
 *
 * Deprecated: 4.18: This widget will be removed in GTK 5
 */
const char *
gtk_shortcut_label_get_accelerator (GtkShortcutLabel *this)
{
  g_return_val_if_fail (GTK_IS_SHORTCUT_LABEL (this), NULL);

  return this->accelerator;
}

/**
 * gtk_shortcut_label_set_accelerator:
 * @this: a `GtkShortcutLabel`
 * @accelerator: the new accelerator
 *
 * Sets the accelerator to be displayed by @this.
 *
 * Deprecated: 4.18: This widget will be removed in GTK 5
 */
void
gtk_shortcut_label_set_accelerator (GtkShortcutLabel *this,
                                    const char       *accelerator)
{
  g_return_if_fail (GTK_IS_SHORTCUT_LABEL (this));

  if (g_strcmp0 (accelerator, this->accelerator) != 0)
    {
      g_free (this->accelerator);
      this->accelerator = g_strdup (accelerator);
      gtk_shortcut_label_rebuild (this);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_ACCELERATOR]);
    }
}

/**
 * gtk_shortcut_label_get_disabled_text:
 * @this: a `GtkShortcutLabel`
 *
 * Retrieves the text that is displayed when no accelerator is set.
 *
 * Returns: (transfer none)(nullable): the current text displayed when no
 * accelerator is set.
 *
 * Deprecated: 4.18: This widget will be removed in GTK 5
 */
const char *
gtk_shortcut_label_get_disabled_text (GtkShortcutLabel *this)
{
  g_return_val_if_fail (GTK_IS_SHORTCUT_LABEL (this), NULL);

  return this->disabled_text;
}

/**
 * gtk_shortcut_label_set_disabled_text:
 * @this: a `GtkShortcutLabel`
 * @disabled_text: the text to be displayed when no accelerator is set
 *
 * Sets the text to be displayed by @this when no accelerator is set.
 *
 * Deprecated: 4.18: This widget will be removed in GTK 5
 */
void
gtk_shortcut_label_set_disabled_text (GtkShortcutLabel *this,
                                      const char       *disabled_text)
{
  g_return_if_fail (GTK_IS_SHORTCUT_LABEL (this));

  if (g_strcmp0 (disabled_text, this->disabled_text) != 0)
    {
      g_free (this->disabled_text);
      this->disabled_text = g_strdup (disabled_text);
      gtk_shortcut_label_rebuild (this);
      g_object_notify_by_pspec (G_OBJECT (this), properties[PROP_DISABLED_TEXT]);
    }
}
