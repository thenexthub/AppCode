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

#include "appcode/shell/platform/windows/client_wrapper/testing/stub_appcode_windows_api.h"

static appcode::testing::StubappcodeWindowsApi* s_stub_implementation;

namespace appcode {
namespace testing {

// static
void StubappcodeWindowsApi::SetTestStub(StubappcodeWindowsApi* stub) {
  s_stub_implementation = stub;
}

// static
StubappcodeWindowsApi* StubappcodeWindowsApi::GetTestStub() {
  return s_stub_implementation;
}

ScopedStubappcodeWindowsApi::ScopedStubappcodeWindowsApi(
    std::unique_ptr<StubappcodeWindowsApi> stub)
    : stub_(std::move(stub)) {
  previous_stub_ = StubappcodeWindowsApi::GetTestStub();
  StubappcodeWindowsApi::SetTestStub(stub_.get());
}

ScopedStubappcodeWindowsApi::~ScopedStubappcodeWindowsApi() {
  StubappcodeWindowsApi::SetTestStub(previous_stub_);
}

}  // namespace testing
}  // namespace appcode

// Forwarding dummy implementations of the C API.

appcodeDesktopViewControllerRef appcodeDesktopViewControllerCreate(
    int width,
    int height,
    appcodeDesktopEngineRef engine) {
  if (s_stub_implementation) {
    return s_stub_implementation->ViewControllerCreate(width, height, engine);
  }
  return nullptr;
}

void appcodeDesktopViewControllerDestroy(
    appcodeDesktopViewControllerRef controller) {
  if (s_stub_implementation) {
    s_stub_implementation->ViewControllerDestroy();
  }
}

appcodeDesktopViewId appcodeDesktopViewControllerGetViewId(
    appcodeDesktopViewControllerRef controller) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return static_cast<appcodeDesktopViewId>(1);
}

appcodeDesktopEngineRef appcodeDesktopViewControllerGetEngine(
    appcodeDesktopViewControllerRef controller) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopEngineRef>(1);
}

appcodeDesktopViewRef appcodeDesktopViewControllerGetView(
    appcodeDesktopViewControllerRef controller) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopViewRef>(1);
}

void appcodeDesktopViewControllerForceRedraw(
    appcodeDesktopViewControllerRef controller) {
  if (s_stub_implementation) {
    s_stub_implementation->ViewControllerForceRedraw();
  }
}

bool appcodeDesktopViewControllerHandleTopLevelWindowProc(
    appcodeDesktopViewControllerRef controller,
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam,
    LRESULT* result) {
  if (s_stub_implementation) {
    return s_stub_implementation->ViewControllerHandleTopLevelWindowProc(
        hwnd, message, wparam, lparam, result);
  }
  return false;
}

appcodeDesktopEngineRef appcodeDesktopEngineCreate(
    const appcodeDesktopEngineProperties* engine_properties) {
  if (s_stub_implementation) {
    return s_stub_implementation->EngineCreate(*engine_properties);
  }
  return nullptr;
}

bool appcodeDesktopEngineDestroy(appcodeDesktopEngineRef engine_ref) {
  if (s_stub_implementation) {
    return s_stub_implementation->EngineDestroy();
  }
  return true;
}

bool appcodeDesktopEngineRun(appcodeDesktopEngineRef engine,
                             const char* entry_point) {
  if (s_stub_implementation) {
    return s_stub_implementation->EngineRun(entry_point);
  }
  return true;
}

uint64_t appcodeDesktopEngineProcessMessages(appcodeDesktopEngineRef engine) {
  if (s_stub_implementation) {
    return s_stub_implementation->EngineProcessMessages();
  }
  return 0;
}

void appcodeDesktopEngineSetNextFrameCallback(appcodeDesktopEngineRef engine,
                                              VoidCallback callback,
                                              void* user_data) {
  if (s_stub_implementation) {
    s_stub_implementation->EngineSetNextFrameCallback(callback, user_data);
  }
}

void appcodeDesktopEngineReloadSystemFonts(appcodeDesktopEngineRef engine) {
  if (s_stub_implementation) {
    s_stub_implementation->EngineReloadSystemFonts();
  }
}

appcodeDesktopPluginRegistrarRef appcodeDesktopEngineGetPluginRegistrar(
    appcodeDesktopEngineRef engine,
    const char* plugin_name) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopPluginRegistrarRef>(1);
}

appcodeDesktopMessengerRef appcodeDesktopEngineGetMessenger(
    appcodeDesktopEngineRef engine) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopMessengerRef>(2);
}

appcodeDesktopTextureRegistrarRef appcodeDesktopEngineGetTextureRegistrar(
    appcodeDesktopEngineRef engine) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopTextureRegistrarRef>(3);
}

HWND appcodeDesktopViewGetHWND(appcodeDesktopViewRef controller) {
  if (s_stub_implementation) {
    return s_stub_implementation->ViewGetHWND();
  }
  return reinterpret_cast<HWND>(-1);
}

IDXGIAdapter* appcodeDesktopViewGetGraphicsAdapter(appcodeDesktopViewRef view) {
  if (s_stub_implementation) {
    return s_stub_implementation->ViewGetGraphicsAdapter();
  }
  return nullptr;
}

bool appcodeDesktopEngineProcessExternalWindowMessage(
    appcodeDesktopEngineRef engine,
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam,
    LRESULT* result) {
  if (s_stub_implementation) {
    return s_stub_implementation->EngineProcessExternalWindowMessage(
        engine, hwnd, message, wparam, lparam, result);
  }
  return false;
}

void appcodeDesktopEngineRegisterPlatformViewType(
    appcodeDesktopEngineRef engine,
    const char* view_type_name,
    appcodePlatformViewTypeEntry view_type) {
  if (s_stub_implementation) {
    s_stub_implementation->EngineRegisterPlatformViewType(view_type_name,
                                                          view_type);
  }
}

appcodeDesktopViewRef appcodeDesktopPluginRegistrarGetView(
    appcodeDesktopPluginRegistrarRef controller) {
  if (s_stub_implementation) {
    return s_stub_implementation->PluginRegistrarGetView();
  }
  return nullptr;
}

appcodeDesktopViewRef appcodeDesktopPluginRegistrarGetViewById(
    appcodeDesktopPluginRegistrarRef controller,
    appcodeDesktopViewId view_id) {
  if (s_stub_implementation) {
    return s_stub_implementation->PluginRegistrarGetViewById(view_id);
  }
  return nullptr;
}

void appcodeDesktopPluginRegistrarRegisterTopLevelWindowProcDelegate(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopWindowProcCallback delegate,
    void* user_data) {
  if (s_stub_implementation) {
    return s_stub_implementation
        ->PluginRegistrarRegisterTopLevelWindowProcDelegate(delegate,
                                                            user_data);
  }
}

void appcodeDesktopPluginRegistrarUnregisterTopLevelWindowProcDelegate(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopWindowProcCallback delegate) {
  if (s_stub_implementation) {
    return s_stub_implementation
        ->PluginRegistrarUnregisterTopLevelWindowProcDelegate(delegate);
  }
}
