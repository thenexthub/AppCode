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

#include "appcode/shell/platform/glfw/client_wrapper/testing/stub_appcode_glfw_api.h"

static appcode::testing::StubappcodeGlfwApi* s_stub_implementation;

namespace appcode {
namespace testing {

// static
void StubappcodeGlfwApi::SetTestStub(StubappcodeGlfwApi* stub) {
  s_stub_implementation = stub;
}

// static
StubappcodeGlfwApi* StubappcodeGlfwApi::GetTestStub() {
  return s_stub_implementation;
}

ScopedStubappcodeGlfwApi::ScopedStubappcodeGlfwApi(
    std::unique_ptr<StubappcodeGlfwApi> stub)
    : stub_(std::move(stub)) {
  previous_stub_ = StubappcodeGlfwApi::GetTestStub();
  StubappcodeGlfwApi::SetTestStub(stub_.get());
}

ScopedStubappcodeGlfwApi::~ScopedStubappcodeGlfwApi() {
  StubappcodeGlfwApi::SetTestStub(previous_stub_);
}

}  // namespace testing
}  // namespace appcode

// Forwarding dummy implementations of the C API.

bool appcodeDesktopInit() {
  if (s_stub_implementation) {
    s_stub_implementation->Init();
  }
  return true;
}

void appcodeDesktopTerminate() {
  if (s_stub_implementation) {
    s_stub_implementation->Terminate();
  }
}

appcodeDesktopWindowControllerRef appcodeDesktopCreateWindow(
    const appcodeDesktopWindowProperties& window_properties,
    const appcodeDesktopEngineProperties& engine_properties) {
  if (s_stub_implementation) {
    return s_stub_implementation->CreateWindow(window_properties,
                                               engine_properties);
  }
  return nullptr;
}

void appcodeDesktopDestroyWindow(appcodeDesktopWindowControllerRef controller) {
  if (s_stub_implementation) {
    s_stub_implementation->DestroyWindow();
  }
}

void appcodeDesktopWindowSetHoverEnabled(appcodeDesktopWindowRef appcode_window,
                                         bool enabled) {
  if (s_stub_implementation) {
    s_stub_implementation->SetHoverEnabled(enabled);
  }
}

void appcodeDesktopWindowSetTitle(appcodeDesktopWindowRef appcode_window,
                                  const char* title) {
  if (s_stub_implementation) {
    s_stub_implementation->SetWindowTitle(title);
  }
}

void appcodeDesktopWindowSetIcon(appcodeDesktopWindowRef appcode_window,
                                 uint8_t* pixel_data,
                                 int width,
                                 int height) {
  if (s_stub_implementation) {
    s_stub_implementation->SetWindowIcon(pixel_data, width, height);
  }
}

void appcodeDesktopWindowGetFrame(appcodeDesktopWindowRef appcode_window,
                                  int* x,
                                  int* y,
                                  int* width,
                                  int* height) {
  if (s_stub_implementation) {
    s_stub_implementation->GetWindowFrame(x, y, width, height);
  }
}

void appcodeDesktopWindowSetFrame(appcodeDesktopWindowRef appcode_window,
                                  int x,
                                  int y,
                                  int width,
                                  int height) {
  if (s_stub_implementation) {
    s_stub_implementation->SetWindowFrame(x, y, width, height);
  }
}

void appcodeDesktopWindowSetSizeLimits(appcodeDesktopWindowRef appcode_window,
                                       appcodeDesktopSize minimum_size,
                                       appcodeDesktopSize maximum_size) {
  if (s_stub_implementation) {
    s_stub_implementation->SetSizeLimits(minimum_size, maximum_size);
  }
}

double appcodeDesktopWindowGetScaleFactor(
    appcodeDesktopWindowRef appcode_window) {
  if (s_stub_implementation) {
    return s_stub_implementation->GetWindowScaleFactor();
  }
  return 1.0;
}

void appcodeDesktopWindowSetPixelRatioOverride(
    appcodeDesktopWindowRef appcode_window,
    double pixel_ratio) {
  if (s_stub_implementation) {
    return s_stub_implementation->SetPixelRatioOverride(pixel_ratio);
  }
}

bool appcodeDesktopRunWindowEventLoopWithTimeout(
    appcodeDesktopWindowControllerRef controller,
    uint32_t millisecond_timeout) {
  if (s_stub_implementation) {
    return s_stub_implementation->RunWindowEventLoopWithTimeout(
        millisecond_timeout);
  }
  return true;
}

appcodeDesktopEngineRef appcodeDesktopRunEngine(
    const appcodeDesktopEngineProperties& properties) {
  if (s_stub_implementation) {
    return s_stub_implementation->RunEngine(properties);
  }
  return nullptr;
}

void appcodeDesktopRunEngineEventLoopWithTimeout(
    appcodeDesktopEngineRef engine,
    uint32_t timeout_milliseconds) {
  if (s_stub_implementation) {
    s_stub_implementation->RunEngineEventLoopWithTimeout(timeout_milliseconds);
  }
}

bool appcodeDesktopShutDownEngine(appcodeDesktopEngineRef engine_ref) {
  if (s_stub_implementation) {
    return s_stub_implementation->ShutDownEngine();
  }
  return true;
}

appcodeDesktopWindowRef appcodeDesktopGetWindow(
    appcodeDesktopWindowControllerRef controller) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopWindowRef>(1);
}

appcodeDesktopEngineRef appcodeDesktopGetEngine(
    appcodeDesktopWindowControllerRef controller) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopEngineRef>(3);
}

appcodeDesktopPluginRegistrarRef appcodeDesktopGetPluginRegistrar(
    appcodeDesktopEngineRef engine,
    const char* plugin_name) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopPluginRegistrarRef>(2);
}

appcodeDesktopWindowRef appcodeDesktopPluginRegistrarGetWindow(
    appcodeDesktopPluginRegistrarRef registrar) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopWindowRef>(3);
}

void appcodeDesktopPluginRegistrarEnableInputBlocking(
    appcodeDesktopPluginRegistrarRef registrar,
    const char* channel) {
  if (s_stub_implementation) {
    s_stub_implementation->PluginRegistrarEnableInputBlocking(channel);
  }
}
