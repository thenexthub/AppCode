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

#include "appcode/shell/platform/windows/public/appcode_windows.h"

#include <io.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <vector>

#include "appcode/shell/platform/common/client_wrapper/include/appcode/plugin_registrar.h"
#include "appcode/shell/platform/common/incoming_message_dispatcher.h"
#include "appcode/shell/platform/common/path_utils.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/windows/dpi_utils.h"
#include "appcode/shell/platform/windows/appcode_project_bundle.h"
#include "appcode/shell/platform/windows/appcode_window.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"
#include "appcode/shell/platform/windows/appcode_windows_view_controller.h"
#include "appcode/shell/platform/windows/window_binding_handler.h"
#include "appcode/shell/platform/windows/window_state.h"

static_assert(appcode_ENGINE_VERSION == 1, "");

// Returns the engine corresponding to the given opaque API handle.
static appcode::appcodeWindowsEngine* EngineFromHandle(
    appcodeDesktopEngineRef ref) {
  return reinterpret_cast<appcode::appcodeWindowsEngine*>(ref);
}

// Returns the opaque API handle for the given engine instance.
static appcodeDesktopEngineRef HandleForEngine(
    appcode::appcodeWindowsEngine* engine) {
  return reinterpret_cast<appcodeDesktopEngineRef>(engine);
}

static appcode::appcodeWindowsViewController* ViewControllerFromHandle(
    appcodeDesktopViewControllerRef ref) {
  return reinterpret_cast<appcode::appcodeWindowsViewController*>(ref);
}

static appcodeDesktopViewControllerRef HandleForViewController(
    appcode::appcodeWindowsViewController* view_controller) {
  return reinterpret_cast<appcodeDesktopViewControllerRef>(view_controller);
}

// Returns the view corresponding to the given opaque API handle.
static appcode::appcodeWindowsView* ViewFromHandle(appcodeDesktopViewRef ref) {
  return reinterpret_cast<appcode::appcodeWindowsView*>(ref);
}

// Returns the opaque API handle for the given view instance.
static appcodeDesktopViewRef HandleForView(appcode::appcodeWindowsView* view) {
  return reinterpret_cast<appcodeDesktopViewRef>(view);
}

// Returns the texture registrar corresponding to the given opaque API handle.
static appcode::appcodeWindowsTextureRegistrar* TextureRegistrarFromHandle(
    appcodeDesktopTextureRegistrarRef ref) {
  return reinterpret_cast<appcode::appcodeWindowsTextureRegistrar*>(ref);
}

// Returns the opaque API handle for the given texture registrar instance.
static appcodeDesktopTextureRegistrarRef HandleForTextureRegistrar(
    appcode::appcodeWindowsTextureRegistrar* registrar) {
  return reinterpret_cast<appcodeDesktopTextureRegistrarRef>(registrar);
}

// Creates a view controller that might own the engine.
//
// If `owns_engine` is true, then the returned `appcodeDesktopViewControllerRef`
// owns `engine_ref` and will deallocate `engine_ref` upon its own destruction.
static appcodeDesktopViewControllerRef CreateViewController(
    appcodeDesktopEngineRef engine_ref,
    int width,
    int height,
    bool owns_engine) {
  appcode::appcodeWindowsEngine* engine_ptr = EngineFromHandle(engine_ref);
  std::unique_ptr<appcode::WindowBindingHandler> window_wrapper =
      std::make_unique<appcode::appcodeWindow>(
          width, height, engine_ptr->windows_proc_table());

  std::unique_ptr<appcode::appcodeWindowsEngine> engine;
  if (owns_engine) {
    engine = std::unique_ptr<appcode::appcodeWindowsEngine>(engine_ptr);
  }

  std::unique_ptr<appcode::appcodeWindowsView> view =
      engine_ptr->CreateView(std::move(window_wrapper));
  if (!view) {
    return nullptr;
  }

  auto controller = std::make_unique<appcode::appcodeWindowsViewController>(
      std::move(engine), std::move(view));

  // Launch the engine if it is not running already.
  if (!controller->engine()->running()) {
    if (!controller->engine()->Run()) {
      return nullptr;
    }
  }

  // Must happen after engine is running.
  controller->view()->SendInitialBounds();

  // The Windows embedder listens to accessibility updates using the
  // view's HWND. The embedder's accessibility features may be stale if
  // the app was in headless mode.
  controller->engine()->UpdateAccessibilityFeatures();

  return HandleForViewController(controller.release());
}

appcodeDesktopViewControllerRef appcodeDesktopViewControllerCreate(
    int width,
    int height,
    appcodeDesktopEngineRef engine) {
  return CreateViewController(engine, width, height, /*owns_engine=*/true);
}

appcodeDesktopViewControllerRef appcodeDesktopEngineCreateViewController(
    appcodeDesktopEngineRef engine,
    const appcodeDesktopViewControllerProperties* properties) {
  return CreateViewController(engine, properties->width, properties->height,
                              /*owns_engine=*/false);
}

void appcodeDesktopViewControllerDestroy(appcodeDesktopViewControllerRef ref) {
  auto controller = ViewControllerFromHandle(ref);
  controller->Destroy();
  delete controller;
}

appcodeDesktopViewId appcodeDesktopViewControllerGetViewId(
    appcodeDesktopViewControllerRef ref) {
  auto controller = ViewControllerFromHandle(ref);
  return static_cast<appcodeDesktopViewId>(controller->view()->view_id());
}

appcodeDesktopEngineRef appcodeDesktopViewControllerGetEngine(
    appcodeDesktopViewControllerRef ref) {
  auto controller = ViewControllerFromHandle(ref);
  return HandleForEngine(controller->engine());
}

appcodeDesktopViewRef appcodeDesktopViewControllerGetView(
    appcodeDesktopViewControllerRef ref) {
  auto controller = ViewControllerFromHandle(ref);
  return HandleForView(controller->view());
}

void appcodeDesktopViewControllerForceRedraw(
    appcodeDesktopViewControllerRef ref) {
  auto controller = ViewControllerFromHandle(ref);
  controller->view()->ForceRedraw();
}

bool appcodeDesktopViewControllerHandleTopLevelWindowProc(
    appcodeDesktopViewControllerRef ref,
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam,
    LRESULT* result) {
  auto controller = ViewControllerFromHandle(ref);
  std::optional<LRESULT> delegate_result =
      controller->engine()
          ->window_proc_delegate_manager()
          ->OnTopLevelWindowProc(hwnd, message, wparam, lparam);
  if (delegate_result) {
    *result = *delegate_result;
  }
  return delegate_result.has_value();
}

appcodeDesktopEngineRef appcodeDesktopEngineCreate(
    const appcodeDesktopEngineProperties* engine_properties) {
  appcode::appcodeProjectBundle project(*engine_properties);
  auto engine = std::make_unique<appcode::appcodeWindowsEngine>(project);
  return HandleForEngine(engine.release());
}

bool appcodeDesktopEngineDestroy(appcodeDesktopEngineRef engine_ref) {
  appcode::appcodeWindowsEngine* engine = EngineFromHandle(engine_ref);
  bool result = true;
  if (engine->running()) {
    result = engine->Stop();
  }
  delete engine;
  return result;
}

appcode_EXPORT appcodeDesktopEngineRef appcodeDesktopEngineForId(
    int64_t engine_id) {
  return HandleForEngine(
      appcode::appcodeWindowsEngine::GetEngineForId(engine_id));
}

bool appcodeDesktopEngineRun(appcodeDesktopEngineRef engine,
                             const char* entry_point) {
  std::string_view entry_point_view{""};
  if (entry_point != nullptr) {
    entry_point_view = entry_point;
  }

  return EngineFromHandle(engine)->Run(entry_point_view);
}

uint64_t appcodeDesktopEngineProcessMessages(appcodeDesktopEngineRef engine) {
  return std::chrono::nanoseconds::max().count();
}

void appcodeDesktopEngineReloadSystemFonts(appcodeDesktopEngineRef engine) {
  EngineFromHandle(engine)->ReloadSystemFonts();
}

appcodeDesktopPluginRegistrarRef appcodeDesktopEngineGetPluginRegistrar(
    appcodeDesktopEngineRef engine,
    const char* plugin_name) {
  // Currently, one registrar acts as the registrar for all plugins, so the
  // name is ignored. It is part of the API to reduce churn in the future when
  // aligning more closely with the appcode registrar system.

  return EngineFromHandle(engine)->GetRegistrar();
}

appcodeDesktopMessengerRef appcodeDesktopEngineGetMessenger(
    appcodeDesktopEngineRef engine) {
  return EngineFromHandle(engine)->messenger();
}

appcodeDesktopTextureRegistrarRef appcodeDesktopEngineGetTextureRegistrar(
    appcodeDesktopEngineRef engine) {
  return HandleForTextureRegistrar(
      EngineFromHandle(engine)->texture_registrar());
}

void appcodeDesktopEngineSetNextFrameCallback(appcodeDesktopEngineRef engine,
                                              VoidCallback callback,
                                              void* user_data) {
  EngineFromHandle(engine)->SetNextFrameCallback(
      [callback, user_data]() { callback(user_data); });
}

HWND appcodeDesktopViewGetHWND(appcodeDesktopViewRef view) {
  return ViewFromHandle(view)->GetWindowHandle();
}

IDXGIAdapter* appcodeDesktopViewGetGraphicsAdapter(appcodeDesktopViewRef view) {
  auto egl_manager = ViewFromHandle(view)->GetEngine()->egl_manager();
  if (egl_manager) {
    Microsoft::WRL::ComPtr<ID3D11Device> d3d_device;
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
    if (egl_manager->GetDevice(d3d_device.GetAddressOf()) &&
        SUCCEEDED(d3d_device.As(&dxgi_device))) {
      IDXGIAdapter* adapter;
      if (SUCCEEDED(dxgi_device->GetAdapter(&adapter))) {
        return adapter;
      }
    }
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
  std::optional<LRESULT> lresult =
      EngineFromHandle(engine)->ProcessExternalWindowMessage(hwnd, message,
                                                             wparam, lparam);
  if (result && lresult.has_value()) {
    *result = lresult.value();
  }
  return lresult.has_value();
}

void appcodeDesktopEngineRegisterPlatformViewType(
    appcodeDesktopEngineRef engine,
    const char* view_type_name,
    appcodePlatformViewTypeEntry view_type) {
  // TODO(schectman): forward to platform view manager.
  // https://github.com/appcode/appcode/issues/143375
}

appcodeDesktopViewRef appcodeDesktopPluginRegistrarGetView(
    appcodeDesktopPluginRegistrarRef registrar) {
  return HandleForView(registrar->engine->view(appcode::kImplicitViewId));
}

appcodeDesktopViewRef appcodeDesktopPluginRegistrarGetViewById(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopViewId view_id) {
  return HandleForView(registrar->engine->view(view_id));
}

void appcodeDesktopPluginRegistrarRegisterTopLevelWindowProcDelegate(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopWindowProcCallback delegate,
    void* user_data) {
  registrar->engine->window_proc_delegate_manager()
      ->RegisterTopLevelWindowProcDelegate(delegate, user_data);
}

void appcodeDesktopPluginRegistrarUnregisterTopLevelWindowProcDelegate(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopWindowProcCallback delegate) {
  registrar->engine->window_proc_delegate_manager()
      ->UnregisterTopLevelWindowProcDelegate(delegate);
}

UINT appcodeDesktopGetDpiForHWND(HWND hwnd) {
  return appcode::GetDpiForHWND(hwnd);
}

UINT appcodeDesktopGetDpiForMonitor(HMONITOR monitor) {
  return appcode::GetDpiForMonitor(monitor);
}

void appcodeDesktopResyncOutputStreams() {
  FILE* unused;
  if (freopen_s(&unused, "CONOUT$", "w", stdout)) {
    _dup2(_fileno(stdout), 1);
  }
  if (freopen_s(&unused, "CONOUT$", "w", stderr)) {
    _dup2(_fileno(stdout), 2);
  }
  std::ios::sync_with_stdio();
}

// Implementations of common/ API methods.

appcodeDesktopMessengerRef appcodeDesktopPluginRegistrarGetMessenger(
    appcodeDesktopPluginRegistrarRef registrar) {
  return registrar->engine->messenger();
}

void appcodeDesktopPluginRegistrarSetDestructionHandler(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopOnPluginRegistrarDestroyed callback) {
  registrar->engine->AddPluginRegistrarDestructionCallback(callback, registrar);
}

bool appcodeDesktopMessengerSendWithReply(appcodeDesktopMessengerRef messenger,
                                          const char* channel,
                                          const uint8_t* message,
                                          const size_t message_size,
                                          const appcodeDesktopBinaryReply reply,
                                          void* user_data) {
  FML_DCHECK(appcodeDesktopMessengerIsAvailable(messenger))
      << "Messenger must reference a running engine to send a message";

  return appcode::appcodeDesktopMessenger::FromRef(messenger)
      ->GetEngine()
      ->SendPlatformMessage(channel, message, message_size, reply, user_data);
}

bool appcodeDesktopMessengerSend(appcodeDesktopMessengerRef messenger,
                                 const char* channel,
                                 const uint8_t* message,
                                 const size_t message_size) {
  return appcodeDesktopMessengerSendWithReply(messenger, channel, message,
                                              message_size, nullptr, nullptr);
}

void appcodeDesktopMessengerSendResponse(
    appcodeDesktopMessengerRef messenger,
    const appcodeDesktopMessageResponseHandle* handle,
    const uint8_t* data,
    size_t data_length) {
  FML_DCHECK(appcodeDesktopMessengerIsAvailable(messenger))
      << "Messenger must reference a running engine to send a response";

  appcode::appcodeDesktopMessenger::FromRef(messenger)
      ->GetEngine()
      ->SendPlatformMessageResponse(handle, data, data_length);
}

void appcodeDesktopMessengerSetCallback(appcodeDesktopMessengerRef messenger,
                                        const char* channel,
                                        appcodeDesktopMessageCallback callback,
                                        void* user_data) {
  FML_DCHECK(appcodeDesktopMessengerIsAvailable(messenger))
      << "Messenger must reference a running engine to set a callback";

  appcode::appcodeDesktopMessenger::FromRef(messenger)
      ->GetEngine()
      ->message_dispatcher()
      ->SetMessageCallback(channel, callback, user_data);
}

appcodeDesktopMessengerRef appcodeDesktopMessengerAddRef(
    appcodeDesktopMessengerRef messenger) {
  return appcode::appcodeDesktopMessenger::FromRef(messenger)
      ->AddRef()
      ->ToRef();
}

void appcodeDesktopMessengerRelease(appcodeDesktopMessengerRef messenger) {
  appcode::appcodeDesktopMessenger::FromRef(messenger)->Release();
}

bool appcodeDesktopMessengerIsAvailable(appcodeDesktopMessengerRef messenger) {
  return appcode::appcodeDesktopMessenger::FromRef(messenger)->GetEngine() !=
         nullptr;
}

appcodeDesktopMessengerRef appcodeDesktopMessengerLock(
    appcodeDesktopMessengerRef messenger) {
  appcode::appcodeDesktopMessenger::FromRef(messenger)->GetMutex().lock();
  return messenger;
}

void appcodeDesktopMessengerUnlock(appcodeDesktopMessengerRef messenger) {
  appcode::appcodeDesktopMessenger::FromRef(messenger)->GetMutex().unlock();
}

appcodeDesktopTextureRegistrarRef appcodeDesktopRegistrarGetTextureRegistrar(
    appcodeDesktopPluginRegistrarRef registrar) {
  return HandleForTextureRegistrar(registrar->engine->texture_registrar());
}

int64_t appcodeDesktopTextureRegistrarRegisterExternalTexture(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    const appcodeDesktopTextureInfo* texture_info) {
  return TextureRegistrarFromHandle(texture_registrar)
      ->RegisterTexture(texture_info);
}

void appcodeDesktopTextureRegistrarUnregisterExternalTexture(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id,
    void (*callback)(void* user_data),
    void* user_data) {
  auto registrar = TextureRegistrarFromHandle(texture_registrar);
  if (callback) {
    registrar->UnregisterTexture(
        texture_id, [callback, user_data]() { callback(user_data); });
    return;
  }
  registrar->UnregisterTexture(texture_id);
}

bool appcodeDesktopTextureRegistrarMarkExternalTextureFrameAvailable(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id) {
  return TextureRegistrarFromHandle(texture_registrar)
      ->MarkTextureFrameAvailable(texture_id);
}
