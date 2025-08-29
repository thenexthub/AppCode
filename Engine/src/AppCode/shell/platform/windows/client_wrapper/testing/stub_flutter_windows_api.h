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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_CLIENT_WRAPPER_TESTING_STUB_APPCODE_WINDOWS_API_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_CLIENT_WRAPPER_TESTING_STUB_APPCODE_WINDOWS_API_H_

#include <memory>

#include "appcode/shell/platform/windows/appcode_windows_internal.h"
#include "appcode/shell/platform/windows/public/appcode_windows.h"

namespace appcode {
namespace testing {

// Base class for a object that provides test implementations of the APIs in
// the headers in platform/windows/public/.

// Linking this class into a test binary will provide dummy forwarding
// implementations of that C API, so that the wrapper can be tested separately
// from the actual library.
class StubFlutterWindowsApi {
 public:
  // Sets |stub| as the instance to which calls to the Flutter library C APIs
  // will be forwarded.
  static void SetTestStub(StubFlutterWindowsApi* stub);

  // Returns the current stub, as last set by SetTestFlutterStub.
  static StubFlutterWindowsApi* GetTestStub();

  virtual ~StubFlutterWindowsApi() {}

  // Called for FlutterDesktopViewControllerCreate.
  virtual FlutterDesktopViewControllerRef
  ViewControllerCreate(int width, int height, FlutterDesktopEngineRef engine) {
    return nullptr;
  }

  // Called for FlutterDesktopViewControllerDestroy.
  virtual void ViewControllerDestroy() {}

  // Called for FlutterDesktopViewControllerForceRedraw.
  virtual void ViewControllerForceRedraw() {}

  // Called for FlutterDesktopViewControllerHandleTopLevelWindowProc.
  virtual bool ViewControllerHandleTopLevelWindowProc(HWND hwnd,
                                                      UINT message,
                                                      WPARAM wparam,
                                                      LPARAM lparam,
                                                      LRESULT* result) {
    return false;
  }

  // Called for FlutterDesktopEngineCreate.
  virtual FlutterDesktopEngineRef EngineCreate(
      const FlutterDesktopEngineProperties& engine_properties) {
    return nullptr;
  }

  // Called for FlutterDesktopEngineDestroy.
  virtual bool EngineDestroy() { return true; }

  // Called for FlutterDesktopEngineRun.
  virtual bool EngineRun(const char* entry_point) { return true; }

  // Called for FlutterDesktopEngineProcessMessages.
  virtual uint64_t EngineProcessMessages() { return 0; }

  // Called for FlutterDesktopEngineSetNextFrameCallback.
  virtual void EngineSetNextFrameCallback(VoidCallback callback,
                                          void* user_data) {}

  // Called for FlutterDesktopEngineReloadSystemFonts.
  virtual void EngineReloadSystemFonts() {}

  // Called for FlutterDesktopEngineRegisterPlatformViewType.
  virtual void EngineRegisterPlatformViewType(
      const char* view_type_name,
      FlutterPlatformViewTypeEntry view_type) {}

  // Called for FlutterDesktopViewGetHWND.
  virtual HWND ViewGetHWND() { return reinterpret_cast<HWND>(1); }

  // Called for FlutterDesktopViewGetGraphicsAdapter.
  virtual IDXGIAdapter* ViewGetGraphicsAdapter() {
    return reinterpret_cast<IDXGIAdapter*>(2);
  }

  // Called for FlutterDesktopPluginRegistrarGetView.
  virtual FlutterDesktopViewRef PluginRegistrarGetView() { return nullptr; }

  // Called for FlutterDesktopPluginRegistrarGetViewById.
  virtual FlutterDesktopViewRef PluginRegistrarGetViewById(
      FlutterDesktopViewId view_id) {
    return nullptr;
  }

  // Called for FlutterDesktopPluginRegistrarRegisterTopLevelWindowProcDelegate.
  virtual void PluginRegistrarRegisterTopLevelWindowProcDelegate(
      FlutterDesktopWindowProcCallback delegate,
      void* user_data) {}

  // Called for
  // FlutterDesktopPluginRegistrarUnregisterTopLevelWindowProcDelegate.
  virtual void PluginRegistrarUnregisterTopLevelWindowProcDelegate(
      FlutterDesktopWindowProcCallback delegate) {}

  // Called for FlutterDesktopEngineProcessExternalWindowMessage.
  virtual bool EngineProcessExternalWindowMessage(
      FlutterDesktopEngineRef engine,
      HWND hwnd,
      UINT message,
      WPARAM wparam,
      LPARAM lparam,
      LRESULT* result) {
    return false;
  }
};

// A test helper that owns a stub implementation, making it the test stub for
// the lifetime of the object, then restoring the previous value.
class ScopedStubFlutterWindowsApi {
 public:
  // Calls SetTestFlutterStub with |stub|.
  ScopedStubFlutterWindowsApi(std::unique_ptr<StubFlutterWindowsApi> stub);

  // Restores the previous test stub.
  ~ScopedStubFlutterWindowsApi();

  StubFlutterWindowsApi* stub() { return stub_.get(); }

 private:
  std::unique_ptr<StubFlutterWindowsApi> stub_;
  // The previous stub.
  StubFlutterWindowsApi* previous_stub_;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_CLIENT_WRAPPER_TESTING_STUB_APPCODE_WINDOWS_API_H_
