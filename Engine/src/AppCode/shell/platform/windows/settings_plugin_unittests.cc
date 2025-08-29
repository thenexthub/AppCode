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

#include "appcode/shell/platform/windows/settings_plugin.h"

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/task_runner.h"
#include "appcode/shell/platform/windows/testing/test_binary_messenger.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace appcode {
namespace testing {

namespace {

class MockSettingsPlugin : public SettingsPlugin {
 public:
  explicit MockSettingsPlugin(BinaryMessenger* messenger,
                              TaskRunner* task_runner)
      : SettingsPlugin(messenger, task_runner) {}

  virtual ~MockSettingsPlugin() = default;

  bool is_high_contrast() { return is_high_contrast_; }

  // |SettingsPlugin|
  MOCK_METHOD(bool, GetAlwaysUse24HourFormat, (), (override));
  MOCK_METHOD(float, GetTextScaleFactor, (), (override));
  MOCK_METHOD(PlatformBrightness, GetPreferredBrightness, (), (override));

  MOCK_METHOD(void, WatchPreferredBrightnessChanged, (), (override));
  MOCK_METHOD(void, WatchTextScaleFactorChanged, (), (override));

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(MockSettingsPlugin);
};

}  // namespace

TEST(SettingsPluginTest, SendSettingsSendsMessage) {
  bool message_is_sent = false;
  TestBinaryMessenger messenger(
      [&message_is_sent](const std::string& channel, const uint8_t* message,
                         size_t message_size,
                         BinaryReply reply) { message_is_sent = true; });
  ::testing::NiceMock<MockSettingsPlugin> settings_plugin(&messenger, nullptr);

  settings_plugin.SendSettings();

  EXPECT_TRUE(message_is_sent);
}

TEST(SettingsPluginTest, SendSettingsGetsSettings) {
  TestBinaryMessenger messenger([](const std::string& channel,
                                   const uint8_t* message, size_t message_size,
                                   BinaryReply reply) {});
  ::testing::NiceMock<MockSettingsPlugin> settings_plugin(&messenger, nullptr);

  EXPECT_CALL(settings_plugin, GetAlwaysUse24HourFormat).Times(1);
  EXPECT_CALL(settings_plugin, GetTextScaleFactor).Times(1);
  EXPECT_CALL(settings_plugin, GetPreferredBrightness).Times(1);

  settings_plugin.SendSettings();
}

TEST(SettingsPluginTest, StartWatchingStartsWatchingChanges) {
  TestBinaryMessenger messenger([](const std::string& channel,
                                   const uint8_t* message, size_t message_size,
                                   BinaryReply reply) {});
  ::testing::NiceMock<MockSettingsPlugin> settings_plugin(&messenger, nullptr);

  EXPECT_CALL(settings_plugin, WatchPreferredBrightnessChanged).Times(1);
  EXPECT_CALL(settings_plugin, WatchTextScaleFactorChanged).Times(1);

  settings_plugin.StartWatching();
}

TEST(SettingsPluginTest, HighContrastModeHonored) {
  int times = 0;
  TestBinaryMessenger messenger(
      [&times](const std::string& channel, const uint8_t* message,
               size_t message_size, BinaryReply reply) {
        ASSERT_EQ(channel, "appcode/settings");
        times++;
      });
  ::testing::NiceMock<MockSettingsPlugin> settings_plugin(&messenger, nullptr);

  settings_plugin.UpdateHighContrastMode(true);
  EXPECT_TRUE(settings_plugin.is_high_contrast());

  settings_plugin.UpdateHighContrastMode(false);
  EXPECT_FALSE(settings_plugin.is_high_contrast());

  EXPECT_EQ(times, 2);
}

}  // namespace testing
}  // namespace appcode
