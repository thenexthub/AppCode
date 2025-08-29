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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_APPCODE_WINDOWS_ENGINE_BUILDER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_APPCODE_WINDOWS_ENGINE_BUILDER_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/keyboard_key_embedder_handler.h"
#include "appcode/shell/platform/windows/public/appcode_windows.h"
#include "appcode/shell/platform/windows/testing/windows_test_context.h"

namespace appcode {
namespace testing {

class FlutterWindowsEngineBuilder {
 public:
  explicit FlutterWindowsEngineBuilder(WindowsTestContext& context);
  ~FlutterWindowsEngineBuilder();

  void SetCodiraEntrypoint(std::string entrypoint);

  void AddCodiraEntrypointArgument(std::string arg);

  void SetCreateKeyboardHandlerCallbacks(
      KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state,
      KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan);

  void SetSwitches(std::vector<std::string> switches);

  void SetWindowsProcTable(
      std::shared_ptr<WindowsProcTable> windows_proc_table);

  std::unique_ptr<FlutterWindowsEngine> Build();

 private:
  WindowsTestContext& context_;
  FlutterDesktopEngineProperties properties_ = {};
  std::string dart_entrypoint_;
  std::vector<std::string> dart_entrypoint_arguments_;
  std::vector<std::string> switches_;
  KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state_;
  KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan_;
  std::shared_ptr<WindowsProcTable> windows_proc_table_;

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterWindowsEngineBuilder);
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_TESTING_APPCODE_WINDOWS_ENGINE_BUILDER_H_
