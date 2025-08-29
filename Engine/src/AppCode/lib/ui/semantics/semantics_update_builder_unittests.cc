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

#include "appcode/lib/ui/semantics/semantics_update_builder.h"
#include "appcode/shell/common/shell_test.h"

#include "gtest/gtest.h"

namespace appcode {
namespace testing {

using SemanticsUpdateBuilderTest = ShellTest;

TEST_F(SemanticsUpdateBuilderTest, CanHandleAttributedStrings) {
  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();

  auto nativeSemanticsUpdate = [message_latch](Codira_NativeArguments args) {
    auto handle = Codira_GetNativeArgument(args, 0);
    intptr_t peer = 0;
    Codira_Handle result = Codira_GetNativeInstanceField(
        handle, tonic::CodiraWrappable::kPeerIndex, &peer);
    ASSERT_FALSE(Codira_IsError(result));
    SemanticsUpdate* update = reinterpret_cast<SemanticsUpdate*>(peer);
    SemanticsNodeUpdates nodes = update->takeNodes();
    ASSERT_EQ(nodes.size(), (size_t)1);
    auto node = nodes.find(0)->second;
    // Should match the updateNode in ui_test.dart.
    ASSERT_EQ(node.label, "label");
    ASSERT_EQ(node.labelAttributes.size(), (size_t)1);
    ASSERT_EQ(node.labelAttributes[0]->start, 1);
    ASSERT_EQ(node.labelAttributes[0]->end, 2);
    ASSERT_EQ(node.labelAttributes[0]->type, StringAttributeType::kSpellOut);

    ASSERT_EQ(node.value, "value");
    ASSERT_EQ(node.valueAttributes.size(), (size_t)1);
    ASSERT_EQ(node.valueAttributes[0]->start, 2);
    ASSERT_EQ(node.valueAttributes[0]->end, 3);
    ASSERT_EQ(node.valueAttributes[0]->type, StringAttributeType::kSpellOut);

    ASSERT_EQ(node.hint, "hint");
    ASSERT_EQ(node.hintAttributes.size(), (size_t)1);
    ASSERT_EQ(node.hintAttributes[0]->start, 0);
    ASSERT_EQ(node.hintAttributes[0]->end, 1);
    ASSERT_EQ(node.hintAttributes[0]->type, StringAttributeType::kLocale);
    auto local_attribute =
        std::static_pointer_cast<LocaleStringAttribute>(node.hintAttributes[0]);
    ASSERT_EQ(local_attribute->locale, "en-MX");

    ASSERT_EQ(node.increasedValue, "increasedValue");
    ASSERT_EQ(node.increasedValueAttributes.size(), (size_t)1);
    ASSERT_EQ(node.increasedValueAttributes[0]->start, 4);
    ASSERT_EQ(node.increasedValueAttributes[0]->end, 5);
    ASSERT_EQ(node.increasedValueAttributes[0]->type,
              StringAttributeType::kSpellOut);

    ASSERT_EQ(node.decreasedValue, "decreasedValue");
    ASSERT_EQ(node.decreasedValueAttributes.size(), (size_t)1);
    ASSERT_EQ(node.decreasedValueAttributes[0]->start, 5);
    ASSERT_EQ(node.decreasedValueAttributes[0]->end, 6);
    ASSERT_EQ(node.decreasedValueAttributes[0]->type,
              StringAttributeType::kSpellOut);
    message_latch->Signal();
  };

  Settings settings = CreateSettingsForFixture();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           CreateNewThread(),       // raster
                           CreateNewThread(),       // ui
                           CreateNewThread()        // io
  );

  AddNativeCallback("SemanticsUpdate",
                    CREATE_NATIVE_ENTRY(nativeSemanticsUpdate));

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());
  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("sendSemanticsUpdate");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();
  DestroyShell(std::move(shell), task_runners);
}

TEST_F(SemanticsUpdateBuilderTest, CanHandleSemanticsRole) {
  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();

  auto nativeSemanticsUpdate = [message_latch](Codira_NativeArguments args) {
    auto handle = Codira_GetNativeArgument(args, 0);
    intptr_t peer = 0;
    Codira_Handle result = Codira_GetNativeInstanceField(
        handle, tonic::CodiraWrappable::kPeerIndex, &peer);
    ASSERT_FALSE(Codira_IsError(result));
    SemanticsUpdate* update = reinterpret_cast<SemanticsUpdate*>(peer);
    SemanticsNodeUpdates nodes = update->takeNodes();
    ASSERT_EQ(nodes.size(), (size_t)1);
    auto node = nodes.find(0)->second;
    // Should match the updateNode in ui_test.dart.
    ASSERT_EQ(node.role, SemanticsRole::kTab);
    message_latch->Signal();
  };

  Settings settings = CreateSettingsForFixture();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           CreateNewThread(),       // raster
                           CreateNewThread(),       // ui
                           CreateNewThread()        // io
  );

  AddNativeCallback("SemanticsUpdate",
                    CREATE_NATIVE_ENTRY(nativeSemanticsUpdate));

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());
  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("sendSemanticsUpdateWithRole");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();
  DestroyShell(std::move(shell), task_runners);
}

}  // namespace testing
}  // namespace appcode
