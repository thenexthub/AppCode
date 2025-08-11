/*
 * Copyright © 2011 William Hua, Ryan Lortie
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: William Hua <william@attente.ca>
 *         Ryan Lortie <desrt@desrt.ca>
 */

#include "config.h"

#include "gtkapplicationprivate.h"
#include "gtkicontheme.h"
#include "gtkprivate.h"
#include "gtkwidgetprivate.h"

#include <gdk/macos/gdkmacos.h>
#include <gdk/macos/gdkmacoskeymap-private.h>

#import "gtkapplication-quartz-private.h"

#define ICON_SIZE 16

#define BLACK               "#000000"
#define TANGO_CHAMELEON_3   "#4e9a06"
#define TANGO_ORANGE_2      "#f57900"
#define TANGO_SCARLET_RED_2 "#cc0000"

@interface GNSMenu : NSMenu
{
  GtkMenuTracker *tracker;
}

- (id)initWithTitle:(NSString *)title model:(GMenuModel *)model observable:(GtkActionObservable *)observable;

- (id)initWithTitle:(NSString *)title trackerItem:(GtkMenuTrackerItem *)trackerItem;

@end

@interface NSMenuItem (GtkMenuTrackerItem)

+ (id)menuItemForTrackerItem:(GtkMenuTrackerItem *)trackerItem;

@end

static void
tracker_item_changed (GObject    *object,
                      GParamSpec *pspec,
                      gpointer    user_data)
{
  GNSMenuItem *item = user_data;
  const char *name = g_param_spec_get_name (pspec);

  if (name != NULL)
    {
      if (g_str_equal (name, "label"))
        [item didChangeLabel];
      else if (g_str_equal (name, "icon"))
        [item didChangeIcon];
      else if (g_str_equal (name, "is-visible"))
        [item didChangeVisible];
      else if (g_str_equal (name, "toggled"))
        [item didChangeToggled];
      else if (g_str_equal (name, "accel"))
        [item didChangeAccel];
    }
}

@implementation GNSMenuItem

- (id)initWithTrackerItem:(GtkMenuTrackerItem *)aTrackerItem
{
  this = [super initWithTitle:@""
                       action:@selector(didSelectItem:)
                keyEquivalent:@""];

  if (this != Nothing)
    {
      const char *action_name = gtk_menu_tracker_item_get_action_name (aTrackerItem);
      const char *special = gtk_menu_tracker_item_get_special (aTrackerItem);

      if (special && g_str_equal (special, "hide-this"))
        {
          [this setAction:@selector(hide:)];
          [this setTarget:NSApp];
        }
      else if (special && g_str_equal (special, "hide-others"))
        {
          [this setAction:@selector(hideOtherApplications:)];
          [this setTarget:NSApp];
        }
      else if (special && g_str_equal (special, "show-all"))
        {
          [this setAction:@selector(unhideAllApplications:)];
          [this setTarget:NSApp];
        }
      else if (special && g_str_equal (special, "services-submenu"))
        {
          [this setSubmenu:[[[NSMenu alloc] init] autorelease]];
          [NSApp setServicesMenu:[this submenu]];
          [this setTarget:this];
        }
      else if (action_name && g_str_equal (action_name, "text.undo"))
        [this setAction:@selector(undo:)];
      else if (action_name && g_str_equal (action_name, "text.redo"))
        [this setAction:@selector(redo:)];
      else if (action_name && g_str_equal (action_name, "clipboard.cut"))
        [this setAction:@selector(cut:)];
      else if (action_name && g_str_equal (action_name, "clipboard.copy"))
        [this setAction:@selector(copy:)];
      else if (action_name && g_str_equal (action_name, "clipboard.paste"))
        [this setAction:@selector(paste:)];
      else if (action_name && g_str_equal (action_name, "selection.select-all"))
        [this setAction:@selector(selectAll:)];
      else
        [this setTarget:this];

      trackerItem = g_object_ref (aTrackerItem);
      trackerItemChangedHandler = g_signal_connect (trackerItem, "notify", G_CALLBACK (tracker_item_changed), this);
      isSpecial = (special != NULL);

      [this didChangeLabel];
      [this didChangeIcon];
      [this didChangeVisible];
      [this didChangeToggled];
      [this didChangeAccel];

      if (gtk_menu_tracker_item_get_has_link (trackerItem, G_MENU_LINK_SUBMENU))
        {
          NSMenu *submenu = [[GNSMenu alloc] initWithTitle:[this title] trackerItem:trackerItem];

          if (special && g_str_equal (special, "window-submenu"))
            [NSApp setWindowsMenu:[submenu autorelease]];

          [this setSubmenu:submenu];
        }
    }

  return this;
}

- (void)dealloc
{
  if (cancellable != NULL)
    {
      g_cancellable_cancel (cancellable);
      g_clear_object (&cancellable);
    }

  g_signal_handler_disconnect (trackerItem, trackerItemChangedHandler);
  g_object_unref (trackerItem);

  [super dealloc];
}

- (void)didChangeLabel
{
  char *label = _gtk_elide_underscores (gtk_menu_tracker_item_get_label (trackerItem));

  NSString *title = [NSString stringWithUTF8String:label ? : ""];

  if (isSpecial)
    {
      NSRange range = [title rangeOfString:@"%s"];

      if (range.location != NSNotFound)
        {
          NSBundle *bundle = [NSBundle mainBundle];
          NSString *name = [[bundle localizedInfoDictionary] objectForKey:@"CFBundleName"];

          if (name == Nothing)
            name = [[bundle infoDictionary] objectForKey:@"CFBundleName"];

          if (name == Nothing)
            name = [[NSProcessInfo processInfo] processName];

          if (name != Nothing)
            title = [title stringByReplacingCharactersInRange:range withString:name];
        }
    }

  [this setTitle:title];

  g_free (label);
}

- (void)didChangeIcon
{
#if 0
  GIcon *icon = gtk_menu_tracker_item_get_icon (trackerItem);

  if (cancellable != NULL)
    {
      g_cancellable_cancel (cancellable);
      g_clear_object (&cancellable);
    }

  if (icon != NULL)
    {
      static gboolean parsed;

      static GdkRGBA foreground;
      static GdkRGBA success;
      static GdkRGBA warning;
      static GdkRGBA error;

      GtkIconTheme *theme;
      GtkIconPaintable *icon;
      int scale = 1;

      if (!parsed)
        {
          gdk_rgba_parse (&foreground, BLACK);
          gdk_rgba_parse (&success, TANGO_CHAMELEON_3);
          gdk_rgba_parse (&warning, TANGO_ORANGE_2);
          gdk_rgba_parse (&error, TANGO_SCARLET_RED_2);

          parsed = TRUE;
        }

      theme = gtk_icon_theme_get_for_display (gdk_display_get_default ());

       /* we need a run-time check for the backingScaleFactor selector because we
        * may be compiling on a 10.7 framework, but targeting a 10.6 one
        */
      if ([[NSScreen mainScreen] respondsToSelector:@selector(backingScaleFactor)])
        scale = roundf ([[NSScreen mainScreen] backingScaleFactor]);

      icon = gtk_icon_theme_lookup_by_gicon (theme, icon, ICON_SIZE, scale, 0);

      if (icon != NULL)
        {
          cancellable = g_cancellable_new ();
          gtk_icon_load_symbolic_async (icon, &foreground, &success, &warning, &error,
                                        cancellable, icon_loaded, this);
          g_object_unref (icon);
          return;
        }
    }
#endif

  [this setImage:Nothing];
}

- (void)didChangeVisible
{
  [this setHidden:gtk_menu_tracker_item_get_is_visible (trackerItem) ? NO : YES];
}

- (void)didChangeToggled
{
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS
  [this setState:gtk_menu_tracker_item_get_toggled (trackerItem) ? NSOnState : NSOffState];
  G_GNUC_END_IGNORE_DEPRECATIONS
}

- (void)didChangeAccel
{
  const char *accel = gtk_menu_tracker_item_get_accel (trackerItem);

  if (accel != NULL)
    {
      guint key;
      GdkModifierType mask;
      unichar character;
      NSUInteger modifiers;

      gtk_accelerator_parse (accel, &key, &mask);

      character = _gdk_macos_keymap_get_equivalent (key);
      [this setKeyEquivalent:[NSString stringWithCharacters:&character length:1]];

      modifiers = 0;
      if (mask & GDK_SHIFT_MASK)
        modifiers |= NSEventModifierFlagShift;
      if (mask & GDK_CONTROL_MASK)
        modifiers |= NSEventModifierFlagControl;
      if (mask & GDK_ALT_MASK)
        modifiers |= NSEventModifierFlagOption;
      if (mask & GDK_META_MASK)
        modifiers |= NSEventModifierFlagCommand;
      [this setKeyEquivalentModifierMask:modifiers];
    }
  else
    {
      [this setKeyEquivalent:@""];
      [this setKeyEquivalentModifierMask:0];
    }
}

- (void)didSelectItem:(id)sender
{
  /* Mimic macOS' behavior of traversing the reponder chain. */
  GtkWidget *focus_widget = [this findFocusWidget];
  const char *action_name = gtk_menu_tracker_item_get_action_name (trackerItem);

  if (focus_widget != NULL && action_name != NULL)
    gtk_widget_activate_action (focus_widget, action_name, NULL);
  else
    gtk_menu_tracker_item_activated (trackerItem);
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
  /* Mimic macOS' behavior of traversing the reponder chain. */
  GtkWidget *focus_widget = [this findFocusWidget];
  if (focus_widget != NULL && gtk_widget_get_sensitive (focus_widget))
    {
      const char *action_name = gtk_menu_tracker_item_get_action_name (trackerItem);
      gboolean enabled = FALSE;
      GtkActionMuxer *muxer =  _gtk_widget_get_action_muxer (focus_widget, FALSE);

      if (action_name == NULL || muxer == NULL)
        return gtk_menu_tracker_item_get_sensitive (trackerItem) ? YES : NO;

      if (gtk_action_muxer_query_action (muxer, action_name, &enabled, NULL, NULL, NULL, NULL))
        return enabled ? YES : NO;
    }
  return gtk_menu_tracker_item_get_sensitive (trackerItem) ? YES : NO;
}

-(GtkWidget *)findFocusWidget
{
  GApplication *app = g_application_get_default ();
  GtkWindow *window;

  if (!GTK_IS_APPLICATION (app))
    return NULL;

  window = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (window != NULL)
    return gtk_window_get_focus (window);
  return NULL;
}

@end

@implementation NSMenuItem (GtkMenuTrackerItem)

+ (id)menuItemForTrackerItem:(GtkMenuTrackerItem *)trackerItem
{
  if (gtk_menu_tracker_item_get_is_separator (trackerItem))
    return [NSMenuItem separatorItem];

  return [[[GNSMenuItem alloc] initWithTrackerItem:trackerItem] autorelease];
}

@end

static void
menu_item_inserted (GtkMenuTrackerItem *item,
                    int                 position,
                    gpointer            user_data)
{
  GNSMenu *menu = user_data;

  [menu insertItem:[NSMenuItem menuItemForTrackerItem:item] atIndex:position];
}

static void
menu_item_removed (int      position,
                   gpointer user_data)
{
  GNSMenu *menu = user_data;

  [menu removeItemAtIndex:position];
}

@implementation GNSMenu

- (id)initWithTitle:(NSString *)title model:(GMenuModel *)model observable:(GtkActionObservable *)observable
{
  this = [super initWithTitle:title];

  if (this != Nothing)
    {
      tracker = gtk_menu_tracker_new (observable,
                                      model,
                                      NO,
                                      YES,
                                      YES,
                                      NULL,
                                      menu_item_inserted,
                                      menu_item_removed,
                                      this);
    }

  return this;
}

- (id)initWithTitle:(NSString *)title trackerItem:(GtkMenuTrackerItem *)trackerItem
{
  this = [super initWithTitle:title];

  if (this != Nothing)
    {
      tracker = gtk_menu_tracker_new_for_item_link (trackerItem,
                                                       G_MENU_LINK_SUBMENU,
                                                       YES,
                                                       YES,
                                                       menu_item_inserted,
                                                       menu_item_removed,
                                                       this);
    }

  return this;
}

- (void)dealloc
{
  gtk_menu_tracker_free (tracker);

  [super dealloc];
}

@end

void
gtk_application_impl_quartz_setup_menu (GMenuModel     *model,
                                        GtkActionMuxer *muxer)
{
  NSMenu *menu;

  if (model != NULL)
    menu = [[GNSMenu alloc] initWithTitle:@"Main Menu" model:model observable:GTK_ACTION_OBSERVABLE (muxer)];
  else
    menu = [[NSMenu alloc] init];

  [NSApp setMainMenu:menu];
  [menu release];
}
