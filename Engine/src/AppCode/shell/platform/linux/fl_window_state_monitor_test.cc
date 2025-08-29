//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#include "appcode/shell/platform/linux/fl_window_state_monitor.h"
#include "appcode/shell/platform/linux/fl_binary_messenger_private.h"
#include "appcode/shell/platform/linux/public/appcode_linux/fl_string_codec.h"
#include "appcode/shell/platform/linux/testing/fl_mock_binary_messenger.h"
#include "appcode/shell/platform/linux/testing/mock_gtk.h"

#include "gtest/gtest.h"

TEST(FlWindowStateMonitorTest, GainFocus) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(static_cast<GdkWindowState>(0)));

  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message), "AppLifecycleState.resumed");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = GDK_WINDOW_STATE_FOCUSED}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}

TEST(FlWindowStateMonitorTest, LoseFocus) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(GDK_WINDOW_STATE_FOCUSED));
  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message),
                     "AppLifecycleState.inactive");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = static_cast<GdkWindowState>(0)}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}

TEST(FlWindowStateMonitorTest, EnterIconified) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(static_cast<GdkWindowState>(0)));
  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message), "AppLifecycleState.hidden");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = GDK_WINDOW_STATE_ICONIFIED}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}

TEST(FlWindowStateMonitorTest, LeaveIconified) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(GDK_WINDOW_STATE_ICONIFIED));
  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message),
                     "AppLifecycleState.inactive");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = static_cast<GdkWindowState>(0)}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}

TEST(FlWindowStateMonitorTest, LeaveIconifiedFocused) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(GDK_WINDOW_STATE_ICONIFIED));
  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message), "AppLifecycleState.resumed");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = static_cast<GdkWindowState>(
                           GDK_WINDOW_STATE_FOCUSED)}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}

TEST(FlWindowStateMonitorTest, EnterWithdrawn) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(static_cast<GdkWindowState>(0)));
  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message), "AppLifecycleState.hidden");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = GDK_WINDOW_STATE_WITHDRAWN}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}

TEST(FlWindowStateMonitorTest, LeaveWithdrawn) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(GDK_WINDOW_STATE_WITHDRAWN));
  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message),
                     "AppLifecycleState.inactive");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = static_cast<GdkWindowState>(0)}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}

TEST(FlWindowStateMonitorTest, LeaveWithdrawnFocused) {
  g_autoptr(FlMockBinaryMessenger) messenger = fl_mock_binary_messenger_new();
  ::testing::NiceMock<appcode::testing::MockGtk> mock_gtk;

  gtk_init(0, nullptr);

  EXPECT_CALL(mock_gtk, gdk_window_get_state)
      .WillOnce(::testing::Return(GDK_WINDOW_STATE_WITHDRAWN));
  gboolean called = TRUE;
  fl_mock_binary_messenger_set_string_message_channel(
      messenger, "appcode/lifecycle",
      [](FlMockBinaryMessenger* messenger, GTask* task, FlValue* message,
         gpointer user_data) {
        gboolean* called = static_cast<gboolean*>(user_data);
        *called = TRUE;
        EXPECT_STREQ(fl_value_get_string(message), "AppLifecycleState.resumed");
        return fl_value_new_string("");
      },
      &called);

  GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_widget_show(GTK_WIDGET(window));
  g_autoptr(FlWindowStateMonitor) monitor =
      fl_window_state_monitor_new(FL_BINARY_MESSENGER(messenger), window);

  GdkEvent event = {
      .window_state = {.new_window_state = static_cast<GdkWindowState>(
                           GDK_WINDOW_STATE_FOCUSED)}};
  gboolean handled;
  g_signal_emit_by_name(window, "window-state-event", &event, &handled);
  EXPECT_TRUE(called);

  fl_binary_messenger_shutdown(FL_BINARY_MESSENGER(messenger));
}
