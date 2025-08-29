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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRAR_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRAR_H_

#include <appcode_plugin_registrar.h>

#include <map>
#include <memory>
#include <set>
#include <string>

#include "binary_messenger.h"
#include "texture_registrar.h"

namespace appcode {

class Plugin;

// A object managing the registration of a plugin for various events.
//
// Currently this class has very limited functionality, but is expected to
// expand over time to more closely match the functionality of
// the Flutter mobile plugin APIs' plugin registrars.
class PluginRegistrar {
 public:
  // Creates a new PluginRegistrar. |core_registrar| and the messenger it
  // provides must remain valid as long as this object exists.
  explicit PluginRegistrar(FlutterDesktopPluginRegistrarRef core_registrar);

  virtual ~PluginRegistrar();

  // Prevent copying.
  PluginRegistrar(PluginRegistrar const&) = delete;
  PluginRegistrar& operator=(PluginRegistrar const&) = delete;

  // Returns the messenger to use for creating channels to communicate with the
  // Flutter engine.
  //
  // This pointer will remain valid for the lifetime of this instance.
  BinaryMessenger* messenger() { return messenger_.get(); }

  // Returns the texture registrar to use for the plugin to render a pixel
  // buffer.
  TextureRegistrar* texture_registrar() { return texture_registrar_.get(); }

  // Takes ownership of |plugin|.
  //
  // Plugins are not required to call this method if they have other lifetime
  // management, but this is a convenient place for plugins to be owned to
  // ensure that they stay valid for any registered callbacks.
  void AddPlugin(std::unique_ptr<Plugin> plugin);

 protected:
  FlutterDesktopPluginRegistrarRef registrar() const { return registrar_; }

  // Destroys all owned plugins. Subclasses should call this at the beginning of
  // their destructors to prevent the possibility of an owned plugin trying to
  // access destroyed state during its own destruction.
  void ClearPlugins();

 private:
  // Handle for interacting with the C API's registrar.
  FlutterDesktopPluginRegistrarRef registrar_;

  std::unique_ptr<BinaryMessenger> messenger_;

  std::unique_ptr<TextureRegistrar> texture_registrar_;

  // Plugins registered for ownership.
  std::set<std::unique_ptr<Plugin>> plugins_;
};

// A plugin that can be registered for ownership by a PluginRegistrar.
class Plugin {
 public:
  virtual ~Plugin() = default;
};

// A singleton to own PluginRegistrars. This is intended for use in plugins,
// where there is no higher-level object to own a PluginRegistrar that can
// own plugin instances and ensure that they live as long as the engine they
// are registered with.
class PluginRegistrarManager {
 public:
  static PluginRegistrarManager* GetInstance();

  // Prevent copying.
  PluginRegistrarManager(PluginRegistrarManager const&) = delete;
  PluginRegistrarManager& operator=(PluginRegistrarManager const&) = delete;

  // Returns a plugin registrar wrapper of type T, which must be a kind of
  // PluginRegistrar, creating it if necessary. The returned registrar will
  // live as long as the underlying FlutterDesktopPluginRegistrarRef, so
  // can be used to own plugin instances.
  //
  // Calling this multiple times for the same registrar_ref with different
  // template types results in undefined behavior.
  template <class T>
  T* GetRegistrar(FlutterDesktopPluginRegistrarRef registrar_ref) {
    auto insert_result =
        registrars_.emplace(registrar_ref, std::make_unique<T>(registrar_ref));
    auto& registrar_pair = *(insert_result.first);
    FlutterDesktopPluginRegistrarSetDestructionHandler(registrar_pair.first,
                                                       OnRegistrarDestroyed);
    return static_cast<T*>(registrar_pair.second.get());
  }

  // Destroys all registrar wrappers created by the manager.
  //
  // This is intended primarily for use in tests.
  void Reset() { registrars_.clear(); }

 private:
  PluginRegistrarManager();

  using WrapperMap = std::map<FlutterDesktopPluginRegistrarRef,
                              std::unique_ptr<PluginRegistrar>>;

  static void OnRegistrarDestroyed(FlutterDesktopPluginRegistrarRef registrar);

  WrapperMap* registrars() { return &registrars_; }

  WrapperMap registrars_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_PLUGIN_REGISTRAR_H_
