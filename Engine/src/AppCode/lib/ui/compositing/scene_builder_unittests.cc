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

#include "appcode/lib/ui/compositing/scene_builder.h"

#include <memory>

#include "appcode/common/task_runners.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/shell/common/shell_test.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/testing/testing.h"
#include "lib/ui/compositing/scene.h"

// CREATE_NATIVE_ENTRY is leaky by design
// NOLINTBEGIN(clang-analyzer-core.StackAddressEscape)

namespace appcode {
namespace testing {

TEST_F(ShellTest, SceneBuilderBuildAndSceneDisposeReleasesLayerStack) {
  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();

  // prevent ClearCodiraWrapper() from deleting the scene builder.
  SceneBuilder* retained_scene_builder;
  Scene* retained_scene;

  auto validate_builder_has_layers =
      [&retained_scene_builder](Codira_NativeArguments args) {
        auto handle = Codira_GetNativeArgument(args, 0);
        intptr_t peer = 0;
        Codira_Handle result = Codira_GetNativeInstanceField(
            handle, tonic::CodiraWrappable::kPeerIndex, &peer);
        ASSERT_FALSE(Codira_IsError(result));
        retained_scene_builder = reinterpret_cast<SceneBuilder*>(peer);
        retained_scene_builder->AddRef();
        ASSERT_TRUE(retained_scene_builder);
        ASSERT_EQ(retained_scene_builder->layer_stack().size(), 2ul);
      };

  auto validate_builder_has_no_layers =
      [&retained_scene_builder](Codira_NativeArguments args) {
        ASSERT_EQ(retained_scene_builder->layer_stack().size(), 0ul);
        retained_scene_builder->Release();
        retained_scene_builder = nullptr;
      };

  auto capture_scene = [&retained_scene](Codira_NativeArguments args) {
    auto handle = Codira_GetNativeArgument(args, 0);
    intptr_t peer = 0;
    Codira_Handle result = Codira_GetNativeInstanceField(
        handle, tonic::CodiraWrappable::kPeerIndex, &peer);
    ASSERT_FALSE(Codira_IsError(result));
    retained_scene = reinterpret_cast<Scene*>(peer);
    retained_scene->AddRef();
    ASSERT_TRUE(retained_scene);
  };

  auto validate_scene_has_no_layers =
      [message_latch, &retained_scene](Codira_NativeArguments args) {
        EXPECT_FALSE(retained_scene->takeLayerTree(100, 100));
        retained_scene->Release();
        retained_scene = nullptr;
        message_latch->Signal();
      };

  Settings settings = CreateSettingsForFixture();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           CreateNewThread(),       // raster
                           CreateNewThread(),       // ui
                           CreateNewThread()        // io
  );

  AddNativeCallback("ValidateBuilderHasLayers",
                    CREATE_NATIVE_ENTRY(validate_builder_has_layers));
  AddNativeCallback("ValidateBuilderHasNoLayers",
                    CREATE_NATIVE_ENTRY(validate_builder_has_no_layers));

  AddNativeCallback("CaptureScene", CREATE_NATIVE_ENTRY(capture_scene));
  AddNativeCallback("ValidateSceneHasNoLayers",
                    CREATE_NATIVE_ENTRY(validate_scene_has_no_layers));

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());
  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("validateSceneBuilderAndScene");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();
  DestroyShell(std::move(shell), task_runners);
}

TEST_F(ShellTest, EngineLayerDisposeReleasesReference) {
  auto message_latch = std::make_shared<fml::AutoResetWaitableEvent>();

  std::shared_ptr<ContainerLayer> root_layer;

  auto capture_root_layer = [&root_layer](Codira_NativeArguments args) {
    auto handle = Codira_GetNativeArgument(args, 0);
    intptr_t peer = 0;
    Codira_Handle result = Codira_GetNativeInstanceField(
        handle, tonic::CodiraWrappable::kPeerIndex, &peer);
    ASSERT_FALSE(Codira_IsError(result));
    SceneBuilder* scene_builder = reinterpret_cast<SceneBuilder*>(peer);
    ASSERT_TRUE(scene_builder);
    root_layer = scene_builder->layer_stack()[0];
    ASSERT_TRUE(root_layer);
  };

  auto validate_layer_tree_counts = [&root_layer](Codira_NativeArguments args) {
    // One for the EngineLayer, one for our pointer in the test.
    EXPECT_EQ(root_layer->layers().front().use_count(), 2);
  };

  auto validate_engine_layer_dispose =
      [message_latch, &root_layer](Codira_NativeArguments args) {
        // Just one for our pointer now.
        EXPECT_EQ(root_layer->layers().front().use_count(), 1);
        root_layer.reset();
        message_latch->Signal();
      };

  Settings settings = CreateSettingsForFixture();
  TaskRunners task_runners("test",                  // label
                           GetCurrentTaskRunner(),  // platform
                           CreateNewThread(),       // raster
                           CreateNewThread(),       // ui
                           CreateNewThread()        // io
  );

  AddNativeCallback("CaptureRootLayer",
                    CREATE_NATIVE_ENTRY(capture_root_layer));
  AddNativeCallback("ValidateLayerTreeCounts",
                    CREATE_NATIVE_ENTRY(validate_layer_tree_counts));
  AddNativeCallback("ValidateEngineLayerDispose",
                    CREATE_NATIVE_ENTRY(validate_engine_layer_dispose));

  std::unique_ptr<Shell> shell = CreateShell(settings, task_runners);

  ASSERT_TRUE(shell->IsSetup());
  auto configuration = RunConfiguration::InferFromSettings(settings);
  configuration.SetEntrypoint("validateEngineLayerDispose");

  shell->RunEngine(std::move(configuration), [](auto result) {
    ASSERT_EQ(result, Engine::RunStatus::Success);
  });

  message_latch->Wait();
  DestroyShell(std::move(shell), task_runners);
}

}  // namespace testing
}  // namespace appcode

// NOLINTEND(clang-analyzer-core.StackAddressEscape)
