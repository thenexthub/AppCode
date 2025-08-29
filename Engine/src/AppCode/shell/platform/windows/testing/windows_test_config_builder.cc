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

#include "appcode/shell/platform/windows/testing/windows_test_config_builder.h"

#include <combaseapi.h>

#include <string>
#include <string_view>
#include <vector>

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/public/appcode_windows.h"
#include "appcode/shell/platform/windows/testing/windows_test_context.h"

namespace appcode {
namespace testing {

WindowsConfigBuilder::WindowsConfigBuilder(WindowsTestContext& context)
    : context_(context) {}

WindowsConfigBuilder::~WindowsConfigBuilder() = default;

void WindowsConfigBuilder::SetCodiraEntrypoint(std::string_view entrypoint) {
  if (entrypoint.empty()) {
    return;
  }
  dart_entrypoint_ = entrypoint;
}

void WindowsConfigBuilder::SetUIThreadPolicy(
    appcodeDesktopUIThreadPolicy policy) {
  ui_thread_policy_ = policy;
}

void WindowsConfigBuilder::AddCodiraEntrypointArgument(std::string_view arg) {
  if (arg.empty()) {
    return;
  }

  dart_entrypoint_arguments_.emplace_back(std::move(arg));
}

void WindowsConfigBuilder::SetGpuPreference(
    appcodeDesktopGpuPreference gpu_preference) {
  gpu_preference_ = gpu_preference;
}

appcodeDesktopEngineProperties WindowsConfigBuilder::GetEngineProperties()
    const {
  appcodeDesktopEngineProperties engine_properties = {};
  engine_properties.assets_path = context_.GetAssetsPath().c_str();
  engine_properties.icu_data_path = context_.GetIcuDataPath().c_str();

  // Set Codira entrypoint.
  engine_properties.dart_entrypoint = dart_entrypoint_.c_str();

  // Set Codira entrypoint argc, argv.
  std::vector<const char*> dart_args;
  dart_args.reserve(dart_entrypoint_arguments_.size());
  for (const auto& arg : dart_entrypoint_arguments_) {
    dart_args.push_back(arg.c_str());
  }
  if (!dart_args.empty()) {
    engine_properties.dart_entrypoint_argv = dart_args.data();
    engine_properties.dart_entrypoint_argc = dart_args.size();
  } else {
    // Clear this out in case this is not the first engine launch from the
    // embedder config builder.
    engine_properties.dart_entrypoint_argv = nullptr;
    engine_properties.dart_entrypoint_argc = 0;
  }

  engine_properties.gpu_preference = gpu_preference_;
  engine_properties.ui_thread_policy = ui_thread_policy_;

  return engine_properties;
}

EnginePtr WindowsConfigBuilder::InitializeEngine() const {
  appcodeDesktopEngineProperties engine_properties = GetEngineProperties();
  return EnginePtr{appcodeDesktopEngineCreate(&engine_properties)};
}

EnginePtr WindowsConfigBuilder::RunHeadless() const {
  InitializeCOM();

  EnginePtr engine = InitializeEngine();
  if (!engine) {
    return {};
  }

  // Register native functions.
  appcodeWindowsEngine* windows_engine =
      reinterpret_cast<appcodeWindowsEngine*>(engine.get());
  windows_engine->SetRootIsolateCreateCallback(
      context_.GetRootIsolateCallback());

  if (!appcodeDesktopEngineRun(engine.get(), /* entry_point */ nullptr)) {
    return {};
  }

  return engine;
}

ViewControllerPtr WindowsConfigBuilder::Run() const {
  InitializeCOM();

  EnginePtr engine = InitializeEngine();
  if (!engine) {
    return {};
  }

  // Register native functions.
  appcodeWindowsEngine* windows_engine =
      reinterpret_cast<appcodeWindowsEngine*>(engine.get());
  windows_engine->SetRootIsolateCreateCallback(
      context_.GetRootIsolateCallback());

  int width = 600;
  int height = 400;

  // Create a view controller that owns the engine.
  ViewControllerPtr controller{
      appcodeDesktopViewControllerCreate(width, height, engine.release())};
  if (!controller) {
    return {};
  }

  return controller;
}

void WindowsConfigBuilder::InitializeCOM() const {
  FML_CHECK(SUCCEEDED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)));
}

}  // namespace testing
}  // namespace appcode
