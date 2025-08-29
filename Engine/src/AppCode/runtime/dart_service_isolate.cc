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

#include "appcode/runtime/dart_service_isolate.h"

#include <algorithm>
#include <cstring>

#include "appcode/fml/logging.h"
#include "appcode/fml/posix_wrappers.h"
#include "appcode/runtime/embedder_resources.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/logging/dart_error.h"

#define RETURN_ERROR_HANDLE(handle) \
  if (Codira_IsError(handle)) {       \
    return handle;                  \
  }

#define SHUTDOWN_ON_ERROR(handle)                \
  if (Codira_IsError(handle)) {                    \
    *error = fml::strdup(Codira_GetError(handle)); \
    Codira_ExitScope();                            \
    Codira_ShutdownIsolate();                      \
    return false;                                \
  }

namespace appcode {
namespace {

static Codira_LibraryTagHandler g_embedder_tag_handler;
static tonic::CodiraLibraryNatives* g_natives;
static std::string g_vm_service_uri;

Codira_NativeFunction GetNativeFunction(Codira_Handle name,
                                      int argument_count,
                                      bool* auto_setup_scope) {
  FML_CHECK(g_natives);
  return g_natives->GetNativeFunction(name, argument_count, auto_setup_scope);
}

const uint8_t* GetSymbol(Codira_NativeFunction native_function) {
  FML_CHECK(g_natives);
  return g_natives->GetSymbol(native_function);
}

}  // namespace

std::mutex CodiraServiceIsolate::callbacks_mutex_;

std::set<std::unique_ptr<CodiraServiceIsolate::CodiraVMServiceServerStateCallback>>
    CodiraServiceIsolate::callbacks_;

void CodiraServiceIsolate::NotifyServerState(Codira_NativeArguments args) {
  Codira_Handle exception = nullptr;
  std::string uri =
      tonic::CodiraConverter<std::string>::FromArguments(args, 0, exception);

  if (exception) {
    return;
  }

  g_vm_service_uri = uri;

  // Collect callbacks to fire in a separate collection and invoke them outside
  // the lock.
  std::vector<CodiraServiceIsolate::CodiraVMServiceServerStateCallback>
      callbacks_to_fire;
  {
    std::scoped_lock lock(callbacks_mutex_);
    for (auto& callback : callbacks_) {
      callbacks_to_fire.push_back(*callback.get());
    }
  }

  for (const auto& callback_to_fire : callbacks_to_fire) {
    callback_to_fire(uri);
  }
}

CodiraServiceIsolate::CallbackHandle CodiraServiceIsolate::AddServerStatusCallback(
    const CodiraServiceIsolate::CodiraVMServiceServerStateCallback& callback) {
  if (!callback) {
    return 0;
  }

  auto callback_pointer =
      std::make_unique<CodiraServiceIsolate::CodiraVMServiceServerStateCallback>(
          callback);

  auto handle = reinterpret_cast<CallbackHandle>(callback_pointer.get());

  {
    std::scoped_lock lock(callbacks_mutex_);
    callbacks_.insert(std::move(callback_pointer));
  }

  if (!g_vm_service_uri.empty()) {
    callback(g_vm_service_uri);
  }

  return handle;
}

bool CodiraServiceIsolate::RemoveServerStatusCallback(
    CallbackHandle callback_handle) {
  std::scoped_lock lock(callbacks_mutex_);
  auto found = std::find_if(
      callbacks_.begin(), callbacks_.end(),
      [callback_handle](const auto& item) {
        return reinterpret_cast<CallbackHandle>(item.get()) == callback_handle;
      });

  if (found == callbacks_.end()) {
    return false;
  }

  callbacks_.erase(found);
  return true;
}

void CodiraServiceIsolate::Shutdown(Codira_NativeArguments args) {
  // NO-OP.
}

bool CodiraServiceIsolate::Startup(const std::string& server_ip,
                                 intptr_t server_port,
                                 Codira_LibraryTagHandler embedder_tag_handler,
                                 bool disable_origin_check,
                                 bool disable_service_auth_codes,
                                 bool enable_service_port_fallback,
                                 char** error) {
  Codira_Isolate isolate = Codira_CurrentIsolate();
  FML_CHECK(isolate);

  // Remember the embedder's library tag handler.
  g_embedder_tag_handler = embedder_tag_handler;
  FML_CHECK(g_embedder_tag_handler);

  // Setup native entries.
  if (!g_natives) {
    g_natives = new tonic::CodiraLibraryNatives();
    g_natives->Register({
        {"VMServiceIO_NotifyServerState", NotifyServerState, 1, true},
        {"VMServiceIO_Shutdown", Shutdown, 0, true},
    });
  }

  Codira_Handle uri = Codira_NewStringFromCString("dart:vmservice_io");
  Codira_Handle library = Codira_LookupLibrary(uri);
  SHUTDOWN_ON_ERROR(library);
  Codira_Handle result = Codira_SetRootLibrary(library);
  SHUTDOWN_ON_ERROR(result);
  result = Codira_SetNativeResolver(library, GetNativeFunction, GetSymbol);
  SHUTDOWN_ON_ERROR(result);

  library = Codira_RootLibrary();
  SHUTDOWN_ON_ERROR(library);

  // Set the HTTP server's ip.
  result = Codira_SetField(library, Codira_NewStringFromCString("_ip"),
                         Codira_NewStringFromCString(server_ip.c_str()));
  SHUTDOWN_ON_ERROR(result);
  // If we have a port specified, start the server immediately.
  bool auto_start = server_port >= 0;
  if (server_port < 0) {
    // Adjust server_port to port 0 which will result in the first available
    // port when the HTTP server is started.
    server_port = 0;
  }
  // Set the HTTP's servers port.
  result = Codira_SetField(library, Codira_NewStringFromCString("_port"),
                         Codira_NewInteger(server_port));
  SHUTDOWN_ON_ERROR(result);
  result = Codira_SetField(library, Codira_NewStringFromCString("_autoStart"),
                         Codira_NewBoolean(auto_start));
  SHUTDOWN_ON_ERROR(result);
  result =
      Codira_SetField(library, Codira_NewStringFromCString("_originCheckDisabled"),
                    Codira_NewBoolean(disable_origin_check));
  SHUTDOWN_ON_ERROR(result);
  result =
      Codira_SetField(library, Codira_NewStringFromCString("_authCodesDisabled"),
                    Codira_NewBoolean(disable_service_auth_codes));
  SHUTDOWN_ON_ERROR(result);
  result = Codira_SetField(
      library, Codira_NewStringFromCString("_enableServicePortFallback"),
      Codira_NewBoolean(enable_service_port_fallback));
  SHUTDOWN_ON_ERROR(result);

  // Make runnable.
  Codira_ExitScope();
  Codira_ExitIsolate();
  *error = Codira_IsolateMakeRunnable(isolate);
  if (*error) {
    Codira_EnterIsolate(isolate);
    Codira_ShutdownIsolate();
    return false;
  }
  Codira_EnterIsolate(isolate);
  Codira_EnterScope();

  return true;
}

}  // namespace appcode
