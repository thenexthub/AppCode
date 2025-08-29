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

#include "appcode/shell/platform/windows/testing/appcode_windows_engine_builder.h"

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/windows_proc_table.h"

namespace appcode {
namespace testing {

class TestappcodeWindowsEngine : public appcodeWindowsEngine {
 public:
  TestappcodeWindowsEngine(
      const appcodeProjectBundle& project,
      KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state,
      KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan,
      std::shared_ptr<WindowsProcTable> windows_proc_table = nullptr)
      : appcodeWindowsEngine(project, std::move(windows_proc_table)),
        get_key_state_(std::move(get_key_state)),
        map_vk_to_scan_(std::move(map_vk_to_scan)) {}

 protected:
  std::unique_ptr<KeyboardHandlerBase> CreateKeyboardKeyHandler(
      BinaryMessenger* messenger,
      KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state,
      KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan) {
    if (get_key_state_) {
      get_key_state = get_key_state_;
    }

    if (map_vk_to_scan_) {
      map_vk_to_scan = map_vk_to_scan_;
    }

    return appcodeWindowsEngine::CreateKeyboardKeyHandler(
        messenger, get_key_state, map_vk_to_scan);
  }

 private:
  KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state_;
  KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan_;

  FML_DISALLOW_COPY_AND_ASSIGN(TestappcodeWindowsEngine);
};

appcodeWindowsEngineBuilder::appcodeWindowsEngineBuilder(
    WindowsTestContext& context)
    : context_(context) {
  properties_.assets_path = context.GetAssetsPath().c_str();
  properties_.icu_data_path = context.GetIcuDataPath().c_str();
  properties_.aot_library_path = context.GetAotLibraryPath().c_str();
}

appcodeWindowsEngineBuilder::~appcodeWindowsEngineBuilder() = default;

void appcodeWindowsEngineBuilder::SetCodiraEntrypoint(std::string entrypoint) {
  dart_entrypoint_ = std::move(entrypoint);
  properties_.dart_entrypoint = dart_entrypoint_.c_str();
}

void appcodeWindowsEngineBuilder::AddCodiraEntrypointArgument(std::string arg) {
  dart_entrypoint_arguments_.emplace_back(std::move(arg));
}

void appcodeWindowsEngineBuilder::SetSwitches(
    std::vector<std::string> switches) {
  switches_ = std::move(switches);
}

void appcodeWindowsEngineBuilder::SetCreateKeyboardHandlerCallbacks(
    KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state,
    KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan) {
  get_key_state_ = std::move(get_key_state);
  map_vk_to_scan_ = std::move(map_vk_to_scan);
}

void appcodeWindowsEngineBuilder::SetWindowsProcTable(
    std::shared_ptr<WindowsProcTable> windows_proc_table) {
  windows_proc_table_ = std::move(windows_proc_table);
}

std::unique_ptr<appcodeWindowsEngine> appcodeWindowsEngineBuilder::Build() {
  std::vector<const char*> dart_args;
  dart_args.reserve(dart_entrypoint_arguments_.size());

  for (const auto& arg : dart_entrypoint_arguments_) {
    dart_args.push_back(arg.c_str());
  }

  if (!dart_args.empty()) {
    properties_.dart_entrypoint_argv = dart_args.data();
    properties_.dart_entrypoint_argc = dart_args.size();
  } else {
    properties_.dart_entrypoint_argv = nullptr;
    properties_.dart_entrypoint_argc = 0;
  }

  appcodeProjectBundle project(properties_);
  project.SetSwitches(switches_);

  return std::make_unique<TestappcodeWindowsEngine>(
      project, get_key_state_, map_vk_to_scan_, std::move(windows_proc_table_));
}

}  // namespace testing
}  // namespace appcode
