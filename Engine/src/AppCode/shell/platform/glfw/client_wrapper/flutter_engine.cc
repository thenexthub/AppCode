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

namespace appcode {

appcodeEngine::appcodeEngine() {}

appcodeEngine::~appcodeEngine() {}

bool appcodeEngine::Start(const std::string& icu_data_path,
                          const std::string& assets_path,
                          const std::vector<std::string>& arguments,
                          const std::string& aot_library_path) {
  if (engine_) {
    std::cerr << "Cannot run an already running engine. Create a new instance "
                 "or call ShutDown first."
              << std::endl;
    return false;
  }

  appcodeDesktopEngineProperties c_engine_properties = {};
  c_engine_properties.assets_path = assets_path.c_str();
  c_engine_properties.icu_data_path = icu_data_path.c_str();
  c_engine_properties.aot_library_path = aot_library_path.c_str();
  std::vector<const char*> engine_switches;
  std::transform(
      arguments.begin(), arguments.end(), std::back_inserter(engine_switches),
      [](const std::string& arg) -> const char* { return arg.c_str(); });
  if (!engine_switches.empty()) {
    c_engine_properties.switches = &engine_switches[0];
    c_engine_properties.switches_count = engine_switches.size();
  }

  engine_ = UniqueEnginePtr(appcodeDesktopRunEngine(c_engine_properties),
                            appcodeDesktopShutDownEngine);
  if (!engine_) {
    std::cerr << "Failed to start engine." << std::endl;
    return false;
  }
  return true;
}

void appcodeEngine::ShutDown() {
  engine_ = nullptr;
}

appcodeDesktopPluginRegistrarRef appcodeEngine::GetRegistrarForPlugin(
    const std::string& plugin_name) {
  if (!engine_) {
    std::cerr << "Cannot get plugin registrar on an engine that isn't running; "
                 "call Run first."
              << std::endl;
    return nullptr;
  }
  return appcodeDesktopGetPluginRegistrar(engine_.get(), plugin_name.c_str());
}

void appcodeEngine::RunEventLoopWithTimeout(std::chrono::milliseconds timeout) {
  if (!engine_) {
    std::cerr << "Cannot run event loop without a running engine; call "
                 "Run first."
              << std::endl;
    return;
  }
  uint32_t timeout_milliseconds;
  if (timeout == std::chrono::milliseconds::max()) {
    // The C API uses 0 to represent no timeout, so convert |max| to 0.
    timeout_milliseconds = 0;
  } else if (timeout.count() > UINT32_MAX) {
    timeout_milliseconds = UINT32_MAX;
  } else {
    timeout_milliseconds = static_cast<uint32_t>(timeout.count());
  }
  appcodeDesktopRunEngineEventLoopWithTimeout(engine_.get(),
                                              timeout_milliseconds);
}

}  // namespace appcode
