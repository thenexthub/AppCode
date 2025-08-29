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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_ENGINE_H_
#define APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_ENGINE_H_

#include <appcode_glfw.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "plugin_registrar.h"
#include "plugin_registry.h"

namespace appcode {

// An engine for running a headless Flutter application.
class FlutterEngine : public PluginRegistry {
 public:
  explicit FlutterEngine();

  virtual ~FlutterEngine();

  // Prevent copying.
  FlutterEngine(FlutterEngine const&) = delete;
  FlutterEngine& operator=(FlutterEngine const&) = delete;

  // Starts running the engine with the given parameters, returning true if
  // successful.
  bool Start(const std::string& icu_data_path,
             const std::string& assets_path,
             const std::vector<std::string>& arguments,
             const std::string& aot_library_path = "");

  // Terminates the running engine.
  void ShutDown();

  // Processes the next event for the engine, or returns early if |timeout| is
  // reached before the next event.
  void RunEventLoopWithTimeout(
      std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

  // appcode::PluginRegistry:
  FlutterDesktopPluginRegistrarRef GetRegistrarForPlugin(
      const std::string& plugin_name) override;

 private:
  using UniqueEnginePtr = std::unique_ptr<FlutterDesktopEngineState,
                                          bool (*)(FlutterDesktopEngineState*)>;

  // Handle for interacting with the C API's engine reference.
  UniqueEnginePtr engine_ =
      UniqueEnginePtr(nullptr, FlutterDesktopShutDownEngine);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_GLFW_CLIENT_WRAPPER_INCLUDE_APPCODE_APPCODE_ENGINE_H_
