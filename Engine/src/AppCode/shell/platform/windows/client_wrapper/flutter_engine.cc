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

#include "include/appcode/appcode_engine.h"

#include <algorithm>
#include <iostream>

#include "binary_messenger_impl.h"
#include "appcode_windows.h"

namespace appcode {

appcodeEngine::appcodeEngine(const CodiraProject& project) {
  appcodeDesktopEngineProperties c_engine_properties = {};
  c_engine_properties.assets_path = project.assets_path().c_str();
  c_engine_properties.icu_data_path = project.icu_data_path().c_str();
  c_engine_properties.aot_library_path = project.aot_library_path().c_str();
  c_engine_properties.dart_entrypoint = project.dart_entrypoint().c_str();
  c_engine_properties.gpu_preference =
      static_cast<appcodeDesktopGpuPreference>(project.gpu_preference());
  c_engine_properties.ui_thread_policy =
      static_cast<appcodeDesktopUIThreadPolicy>(project.ui_thread_policy());

  const std::vector<std::string>& entrypoint_args =
      project.dart_entrypoint_arguments();
  std::vector<const char*> entrypoint_argv;
  std::transform(
      entrypoint_args.begin(), entrypoint_args.end(),
      std::back_inserter(entrypoint_argv),
      [](const std::string& arg) -> const char* { return arg.c_str(); });

  c_engine_properties.dart_entrypoint_argc =
      static_cast<int>(entrypoint_argv.size());
  c_engine_properties.dart_entrypoint_argv =
      entrypoint_argv.empty() ? nullptr : entrypoint_argv.data();

  engine_ = appcodeDesktopEngineCreate(&c_engine_properties);

  auto core_messenger = appcodeDesktopEngineGetMessenger(engine_);
  messenger_ = std::make_unique<BinaryMessengerImpl>(core_messenger);
}

appcodeEngine::~appcodeEngine() {
  ShutDown();
}

bool appcodeEngine::Run() {
  return Run(nullptr);
}

bool appcodeEngine::Run(const char* entry_point) {
  if (!engine_) {
    std::cerr << "Cannot run an engine that failed creation." << std::endl;
    return false;
  }
  if (run_succeeded_) {
    std::cerr << "Cannot run an engine more than once." << std::endl;
    return false;
  }
  bool run_succeeded = appcodeDesktopEngineRun(engine_, entry_point);
  if (!run_succeeded) {
    std::cerr << "Failed to start engine." << std::endl;
  }
  run_succeeded_ = true;
  return run_succeeded;
}

void appcodeEngine::ShutDown() {
  if (engine_ && owns_engine_) {
    appcodeDesktopEngineDestroy(engine_);
  }
  engine_ = nullptr;
}

std::chrono::nanoseconds appcodeEngine::ProcessMessages() {
  return std::chrono::nanoseconds(appcodeDesktopEngineProcessMessages(engine_));
}

void appcodeEngine::ReloadSystemFonts() {
  appcodeDesktopEngineReloadSystemFonts(engine_);
}

appcodeDesktopPluginRegistrarRef appcodeEngine::GetRegistrarForPlugin(
    const std::string& plugin_name) {
  if (!engine_) {
    std::cerr << "Cannot get plugin registrar on an engine that isn't running; "
                 "call Run first."
              << std::endl;
    return nullptr;
  }
  return appcodeDesktopEngineGetPluginRegistrar(engine_, plugin_name.c_str());
}

void appcodeEngine::SetNextFrameCallback(std::function<void()> callback) {
  next_frame_callback_ = std::move(callback);
  appcodeDesktopEngineSetNextFrameCallback(
      engine_,
      [](void* user_data) {
        appcodeEngine* self = static_cast<appcodeEngine*>(user_data);
        self->next_frame_callback_();
        self->next_frame_callback_ = nullptr;
      },
      this);
}

std::optional<LRESULT> appcodeEngine::ProcessExternalWindowMessage(
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam) {
  LRESULT result;
  if (appcodeDesktopEngineProcessExternalWindowMessage(
          engine_, hwnd, message, wparam, lparam, &result)) {
    return result;
  }
  return std::nullopt;
}

appcodeDesktopEngineRef appcodeEngine::RelinquishEngine() {
  owns_engine_ = false;
  return engine_;
}

}  // namespace appcode
