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

#include <memory>
#include <string>

#include "appcode/shell/platform/glfw/client_wrapper/include/appcode/plugin_registrar_glfw.h"
#include "appcode/shell/platform/glfw/client_wrapper/testing/stub_appcode_glfw_api.h"
#include "gtest/gtest.h"

namespace appcode {

namespace {

// A test plugin that tries to access registrar state during destruction and
// reports it out via a flag provided at construction.
class TestPlugin : public Plugin {
 public:
  // registrar_valid_at_destruction will be set at destruction to indicate
  // whether or not |registrar->window()| was non-null.
  TestPlugin(PluginRegistrarGlfw* registrar,
             bool* registrar_valid_at_destruction)
      : registrar_(registrar),
        registrar_valid_at_destruction_(registrar_valid_at_destruction) {}
  virtual ~TestPlugin() {
    *registrar_valid_at_destruction_ = registrar_->window() != nullptr;
  }

 private:
  PluginRegistrarGlfw* registrar_;
  bool* registrar_valid_at_destruction_;
};

}  // namespace

TEST(PluginRegistrarGlfwTest, GetView) {
  testing::ScopedStubappcodeGlfwApi scoped_api_stub(
      std::make_unique<testing::StubappcodeGlfwApi>());
  PluginRegistrarGlfw registrar(
      reinterpret_cast<appcodeDesktopPluginRegistrarRef>(1));
  EXPECT_NE(registrar.window(), nullptr);
}

// Tests that the registrar runs plugin destructors before its own teardown.
TEST(PluginRegistrarGlfwTest, PluginDestroyedBeforeRegistrar) {
  auto dummy_registrar_handle =
      reinterpret_cast<appcodeDesktopPluginRegistrarRef>(1);
  bool registrar_valid_at_destruction = false;
  {
    PluginRegistrarGlfw registrar(dummy_registrar_handle);

    auto plugin = std::make_unique<TestPlugin>(&registrar,
                                               &registrar_valid_at_destruction);
    registrar.AddPlugin(std::move(plugin));
  }
  EXPECT_TRUE(registrar_valid_at_destruction);
}

}  // namespace appcode
