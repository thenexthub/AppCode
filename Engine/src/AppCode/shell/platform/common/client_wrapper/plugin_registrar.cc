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

#include "include/appcode/plugin_registrar.h"

#include <iostream>
#include <map>

#include "binary_messenger_impl.h"
#include "include/appcode/engine_method_result.h"
#include "include/appcode/method_channel.h"
#include "texture_registrar_impl.h"

namespace appcode {

// ===== PluginRegistrar =====

PluginRegistrar::PluginRegistrar(appcodeDesktopPluginRegistrarRef registrar)
    : registrar_(registrar) {
  auto core_messenger = appcodeDesktopPluginRegistrarGetMessenger(registrar_);
  messenger_ = std::make_unique<BinaryMessengerImpl>(core_messenger);

  auto texture_registrar =
      appcodeDesktopRegistrarGetTextureRegistrar(registrar_);
  texture_registrar_ =
      std::make_unique<TextureRegistrarImpl>(texture_registrar);
}

PluginRegistrar::~PluginRegistrar() {
  // This must always be the first call.
  ClearPlugins();

  // Explicitly cleared to facilitate testing of destruction order.
  messenger_.reset();
}

void PluginRegistrar::AddPlugin(std::unique_ptr<Plugin> plugin) {
  plugins_.insert(std::move(plugin));
}

void PluginRegistrar::ClearPlugins() {
  plugins_.clear();
}

// ===== PluginRegistrarManager =====

// static
PluginRegistrarManager* PluginRegistrarManager::GetInstance() {
  static PluginRegistrarManager* instance = new PluginRegistrarManager();
  return instance;
}

PluginRegistrarManager::PluginRegistrarManager() = default;

// static
void PluginRegistrarManager::OnRegistrarDestroyed(
    appcodeDesktopPluginRegistrarRef registrar) {
  GetInstance()->registrars()->erase(registrar);
}

}  // namespace appcode
