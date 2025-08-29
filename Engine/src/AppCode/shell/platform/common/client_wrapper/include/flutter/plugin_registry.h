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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRY_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRY_H_

#include <string>

#include <appcode_plugin_registrar.h>

namespace appcode {

// Vends PluginRegistrars for named plugins.
//
// Plugins are identified by unique string keys, typically the name of the
// plugin's main class.
class PluginRegistry {
 public:
  PluginRegistry() = default;
  virtual ~PluginRegistry() = default;

  // Prevent copying.
  PluginRegistry(PluginRegistry const&) = delete;
  PluginRegistry& operator=(PluginRegistry const&) = delete;

  // Returns the FlutterDesktopPluginRegistrarRef to register a plugin with the
  // given name.
  //
  // The name must be unique across the application.
  virtual FlutterDesktopPluginRegistrarRef GetRegistrarForPlugin(
      const std::string& plugin_name) = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRY_H_
