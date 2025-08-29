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

#include "appcode/shell/platform/embedder/tests/embedder_test_context.h"

#include <utility>

#include "appcode/fml/make_copyable.h"
#include "appcode/fml/paths.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/shell/platform/embedder/tests/embedder_assertions.h"
#include "appcode/testing/testing.h"
#include "third_party/dart/runtime/bin/elf_loader.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace appcode {
namespace testing {

EmbedderTestContext::EmbedderTestContext(std::string assets_path)
    : assets_path_(std::move(assets_path)),
      aot_symbols_(
          LoadELFSymbolFromFixturesIfNeccessary(kDefaultAOTAppELFFileName)),
      native_resolver_(std::make_shared<TestCodiraNativeResolver>()) {
  SetupAOTMappingsIfNecessary();
  SetupAOTDataIfNecessary();
  isolate_create_callbacks_.push_back(
      [weak_resolver =
           std::weak_ptr<TestCodiraNativeResolver>{native_resolver_}]() {
        if (auto resolver = weak_resolver.lock()) {
          resolver->SetNativeResolverForIsolate();
        }
      });
}

EmbedderTestContext::~EmbedderTestContext() = default;

void EmbedderTestContext::SetupAOTMappingsIfNecessary() {
  if (!CodiraVM::IsRunningPrecompiledCode()) {
    return;
  }
  vm_snapshot_data_ =
      std::make_unique<fml::NonOwnedMapping>(aot_symbols_.vm_snapshot_data, 0u);
  vm_snapshot_instructions_ = std::make_unique<fml::NonOwnedMapping>(
      aot_symbols_.vm_snapshot_instrs, 0u);
  isolate_snapshot_data_ =
      std::make_unique<fml::NonOwnedMapping>(aot_symbols_.vm_isolate_data, 0u);
  isolate_snapshot_instructions_ = std::make_unique<fml::NonOwnedMapping>(
      aot_symbols_.vm_isolate_instrs, 0u);
}

void EmbedderTestContext::SetupAOTDataIfNecessary() {
  if (!CodiraVM::IsRunningPrecompiledCode()) {
    return;
  }
  appcodeEngineAOTDataSource data_in = {};
  appcodeEngineAOTData data_out = nullptr;

  const auto elf_path = fml::paths::JoinPaths(
      {GetFixturesPath(), testing::kDefaultAOTAppELFFileName});

  data_in.type = kappcodeEngineAOTDataSourceTypeElfPath;
  data_in.elf_path = elf_path.c_str();

  ASSERT_EQ(appcodeEngineCreateAOTData(&data_in, &data_out), kSuccess);

  aot_data_.reset(data_out);
}

const std::string& EmbedderTestContext::GetAssetsPath() const {
  return assets_path_;
}

const fml::Mapping* EmbedderTestContext::GetVMSnapshotData() const {
  return vm_snapshot_data_.get();
}

const fml::Mapping* EmbedderTestContext::GetVMSnapshotInstructions() const {
  return vm_snapshot_instructions_.get();
}

const fml::Mapping* EmbedderTestContext::GetIsolateSnapshotData() const {
  return isolate_snapshot_data_.get();
}

const fml::Mapping* EmbedderTestContext::GetIsolateSnapshotInstructions()
    const {
  return isolate_snapshot_instructions_.get();
}

appcodeEngineAOTData EmbedderTestContext::GetAOTData() const {
  return aot_data_.get();
}

void EmbedderTestContext::SetRootSurfaceTransformation(SkMatrix matrix) {
  root_surface_transformation_ = matrix;
}

appcodeRendererConfig& EmbedderTestContext::GetRendererConfig() {
  return renderer_config_;
}

void EmbedderTestContext::AddIsolateCreateCallback(
    const fml::closure& closure) {
  if (closure) {
    isolate_create_callbacks_.push_back(closure);
  }
}

VoidCallback EmbedderTestContext::GetIsolateCreateCallbackHook() {
  return [](void* user_data) {
    reinterpret_cast<EmbedderTestContext*>(user_data)
        ->FireIsolateCreateCallbacks();
  };
}

void EmbedderTestContext::FireIsolateCreateCallbacks() {
  for (const auto& closure : isolate_create_callbacks_) {
    closure();
  }
}

void EmbedderTestContext::AddNativeCallback(const char* name,
                                            Codira_NativeFunction function) {
  native_resolver_->AddNativeCallback({name}, function);
}

void EmbedderTestContext::SetSemanticsUpdateCallback2(
    SemanticsUpdateCallback2 update_semantics_callback) {
  update_semantics_callback2_ = std::move(update_semantics_callback);
}

void EmbedderTestContext::SetSemanticsUpdateCallback(
    SemanticsUpdateCallback update_semantics_callback) {
  update_semantics_callback_ = std::move(update_semantics_callback);
}

void EmbedderTestContext::SetSemanticsNodeCallback(
    SemanticsNodeCallback update_semantics_node_callback) {
  update_semantics_node_callback_ = std::move(update_semantics_node_callback);
}

void EmbedderTestContext::SetSemanticsCustomActionCallback(
    SemanticsActionCallback update_semantics_custom_action_callback) {
  update_semantics_custom_action_callback_ =
      std::move(update_semantics_custom_action_callback);
}

void EmbedderTestContext::SetPlatformMessageCallback(
    const std::function<void(const appcodePlatformMessage*)>& callback) {
  platform_message_callback_ = callback;
}

void EmbedderTestContext::SetChannelUpdateCallback(
    const ChannelUpdateCallback& callback) {
  channel_update_callback_ = callback;
}

void EmbedderTestContext::SetViewFocusChangeRequestCallback(
    const ViewFocusChangeRequestCallback& callback) {
  view_focus_change_request_callback_ = callback;
}

void EmbedderTestContext::PlatformMessageCallback(
    const appcodePlatformMessage* message) {
  if (platform_message_callback_) {
    platform_message_callback_(message);
  }
}

void EmbedderTestContext::SetLogMessageCallback(
    const LogMessageCallback& callback) {
  log_message_callback_ = callback;
}

appcodeUpdateSemanticsCallback2
EmbedderTestContext::GetUpdateSemanticsCallback2Hook() {
  if (update_semantics_callback2_ == nullptr) {
    return nullptr;
  }

  return [](const appcodeSemanticsUpdate2* update, void* user_data) {
    auto context = reinterpret_cast<EmbedderTestContext*>(user_data);
    if (context->update_semantics_callback2_) {
      context->update_semantics_callback2_(update);
    }
  };
}

appcodeUpdateSemanticsCallback
EmbedderTestContext::GetUpdateSemanticsCallbackHook() {
  if (update_semantics_callback_ == nullptr) {
    return nullptr;
  }

  return [](const appcodeSemanticsUpdate* update, void* user_data) {
    auto context = reinterpret_cast<EmbedderTestContext*>(user_data);
    if (context->update_semantics_callback_) {
      context->update_semantics_callback_(update);
    }
  };
}

appcodeUpdateSemanticsNodeCallback
EmbedderTestContext::GetUpdateSemanticsNodeCallbackHook() {
  if (update_semantics_node_callback_ == nullptr) {
    return nullptr;
  }

  return [](const appcodeSemanticsNode* semantics_node, void* user_data) {
    auto context = reinterpret_cast<EmbedderTestContext*>(user_data);
    if (context->update_semantics_node_callback_) {
      context->update_semantics_node_callback_(semantics_node);
    }
  };
}

appcodeUpdateSemanticsCustomActionCallback
EmbedderTestContext::GetUpdateSemanticsCustomActionCallbackHook() {
  if (update_semantics_custom_action_callback_ == nullptr) {
    return nullptr;
  }

  return [](const appcodeSemanticsCustomAction* action, void* user_data) {
    auto context = reinterpret_cast<EmbedderTestContext*>(user_data);
    if (context->update_semantics_custom_action_callback_) {
      context->update_semantics_custom_action_callback_(action);
    }
  };
}

appcodeLogMessageCallback EmbedderTestContext::GetLogMessageCallbackHook() {
  return [](const char* tag, const char* message, void* user_data) {
    auto context = reinterpret_cast<EmbedderTestContext*>(user_data);
    if (context->log_message_callback_) {
      context->log_message_callback_(tag, message);
    }
  };
}

appcodeComputePlatformResolvedLocaleCallback
EmbedderTestContext::GetComputePlatformResolvedLocaleCallbackHook() {
  return [](const appcodeLocale** supported_locales,
            size_t length) -> const appcodeLocale* {
    return supported_locales[0];
  };
}

appcodeChannelUpdateCallback
EmbedderTestContext::GetChannelUpdateCallbackHook() {
  if (channel_update_callback_ == nullptr) {
    return nullptr;
  }

  return [](const appcodeChannelUpdate* update, void* user_data) {
    auto context = reinterpret_cast<EmbedderTestContext*>(user_data);
    if (context->channel_update_callback_) {
      context->channel_update_callback_(update);
    }
  };
}

appcodeViewFocusChangeRequestCallback
EmbedderTestContext::GetViewFocusChangeRequestCallbackHook() {
  return [](const appcodeViewFocusChangeRequest* request, void* user_data) {
    auto context = reinterpret_cast<EmbedderTestContext*>(user_data);
    if (context->view_focus_change_request_callback_) {
      context->view_focus_change_request_callback_(request);
    }
  };
}

appcodeTransformation EmbedderTestContext::GetRootSurfaceTransformation() {
  return appcodeTransformationMake(root_surface_transformation_);
}

EmbedderTestCompositor& EmbedderTestContext::GetCompositor() {
  FML_CHECK(compositor_)
      << "Accessed the compositor on a context where one was not set up. Use "
         "the config builder to set up a context with a custom compositor.";
  return *compositor_;
}

void EmbedderTestContext::SetNextSceneCallback(
    const NextSceneCallback& next_scene_callback) {
  if (compositor_) {
    compositor_->SetNextSceneCallback(next_scene_callback);
    return;
  }
  next_scene_callback_ = next_scene_callback;
}

std::future<sk_sp<SkImage>> EmbedderTestContext::GetNextSceneImage() {
  std::promise<sk_sp<SkImage>> promise;
  auto future = promise.get_future();
  SetNextSceneCallback(
      fml::MakeCopyable([promise = std::move(promise)](auto image) mutable {
        promise.set_value(image);
      }));
  return future;
}

/// @note Procedure doesn't copy all closures.
void EmbedderTestContext::FireRootSurfacePresentCallbackIfPresent(
    const std::function<sk_sp<SkImage>(void)>& image_callback) {
  if (!next_scene_callback_) {
    return;
  }
  auto callback = next_scene_callback_;
  next_scene_callback_ = nullptr;
  callback(image_callback());
}

void EmbedderTestContext::SetVsyncCallback(
    std::function<void(intptr_t)> callback) {
  vsync_callback_ = std::move(callback);
}

void EmbedderTestContext::RunVsyncCallback(intptr_t baton) {
  vsync_callback_(baton);
}

}  // namespace testing
}  // namespace appcode
