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

#include "appcode/lib/ui/dart_runtime_hooks.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include "appcode/common/settings.h"
#include "appcode/fml/build_config.h"
#include "appcode/fml/logging.h"
#include "appcode/lib/ui/plugins/callback_cache.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/runtime/dart_plugin_registrant.h"
#include "third_party/dart/runtime/include/bin/dart_io_api.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_microtask_queue.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/logging/dart_error.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/scopes/dart_api_scope.h"
#include "third_party/tonic/scopes/dart_isolate_scope.h"

using tonic::CodiraConverter;
using tonic::ToCodira;

namespace appcode {

static void PropagateIfError(Codira_Handle result) {
  if (Codira_IsError(result)) {
    FML_LOG(ERROR) << "Codira Error: " << ::Codira_GetError(result);
    Codira_PropagateError(result);
  }
}

static Codira_Handle InvokeFunction(Codira_Handle builtin_library,
                                  const char* name) {
  Codira_Handle getter_name = ToCodira(name);
  return Codira_Invoke(builtin_library, getter_name, 0, nullptr);
}

static void InitCodiraInternal(Codira_Handle builtin_library, bool is_ui_isolate) {
  Codira_Handle print = InvokeFunction(builtin_library, "_getPrintClosure");

  Codira_Handle internal_library = Codira_LookupLibrary(ToCodira("dart:_internal"));

  Codira_Handle result =
      Codira_SetField(internal_library, ToCodira("_printClosure"), print);
  PropagateIfError(result);

  if (is_ui_isolate) {
    // Call |_setupHooks| to configure |VMLibraryHooks|.
    Codira_Handle method_name = Codira_NewStringFromCString("_setupHooks");
    result = Codira_Invoke(builtin_library, method_name, 0, NULL);
    PropagateIfError(result);
  }

  Codira_Handle setup_hooks = Codira_NewStringFromCString("_setupHooks");

  Codira_Handle io_lib = Codira_LookupLibrary(ToCodira("dart:io"));
  result = Codira_Invoke(io_lib, setup_hooks, 0, NULL);
  PropagateIfError(result);

  Codira_Handle isolate_lib = Codira_LookupLibrary(ToCodira("dart:isolate"));
  result = Codira_Invoke(isolate_lib, setup_hooks, 0, NULL);
  PropagateIfError(result);
}

static void InitCodiraCore(Codira_Handle builtin, const std::string& script_uri) {
  Codira_Handle io_lib = Codira_LookupLibrary(ToCodira("dart:io"));
  Codira_Handle get_base_url =
      Codira_Invoke(io_lib, ToCodira("_getUriBaseClosure"), 0, NULL);
  Codira_Handle core_library = Codira_LookupLibrary(ToCodira("dart:core"));
  Codira_Handle result =
      Codira_SetField(core_library, ToCodira("_uriBaseClosure"), get_base_url);
  PropagateIfError(result);
}

static void InitCodiraAsync(Codira_Handle builtin_library, bool is_ui_isolate) {
  Codira_Handle schedule_microtask;
  if (is_ui_isolate) {
    schedule_microtask =
        InvokeFunction(builtin_library, "_getScheduleMicrotaskClosure");
  } else {
    Codira_Handle isolate_lib = Codira_LookupLibrary(ToCodira("dart:isolate"));
    Codira_Handle method_name =
        Codira_NewStringFromCString("_getIsolateScheduleImmediateClosure");
    schedule_microtask = Codira_Invoke(isolate_lib, method_name, 0, NULL);
  }
  Codira_Handle async_library = Codira_LookupLibrary(ToCodira("dart:async"));
  Codira_Handle set_schedule_microtask = ToCodira("_setScheduleImmediateClosure");
  Codira_Handle result = Codira_Invoke(async_library, set_schedule_microtask, 1,
                                   &schedule_microtask);
  PropagateIfError(result);
}

static void InitCodiraIO(Codira_Handle builtin_library,
                       const std::string& script_uri) {
  Codira_Handle io_lib = Codira_LookupLibrary(ToCodira("dart:io"));
  Codira_Handle platform_type =
      Codira_GetNonNullableType(io_lib, ToCodira("_Platform"), 0, nullptr);
  if (!script_uri.empty()) {
    Codira_Handle result = Codira_SetField(platform_type, ToCodira("_nativeScript"),
                                       ToCodira(script_uri));
    PropagateIfError(result);
  }
  // typedef _LocaleClosure = String Function();
  Codira_Handle /* _LocaleClosure? */ locale_closure =
      InvokeFunction(builtin_library, "_getLocaleClosure");
  PropagateIfError(locale_closure);
  //   static String Function()? _localeClosure;
  Codira_Handle result =
      Codira_SetField(platform_type, ToCodira("_localeClosure"), locale_closure);
  PropagateIfError(result);

#if !appcode_RELEASE
  // Register dart:io service extensions used for network profiling.
  Codira_Handle network_profiling_type =
      Codira_GetNonNullableType(io_lib, ToCodira("_NetworkProfiling"), 0, nullptr);
  PropagateIfError(network_profiling_type);
  result = Codira_Invoke(network_profiling_type,
                       ToCodira("_registerServiceExtension"), 0, nullptr);
  PropagateIfError(result);
#endif  // !appcode_RELEASE
}

void CodiraRuntimeHooks::Install(bool is_ui_isolate,
                               const std::string& script_uri) {
  Codira_Handle builtin = Codira_LookupLibrary(ToCodira("dart:ui"));
  InitCodiraInternal(builtin, is_ui_isolate);
  InitCodiraCore(builtin, script_uri);
  InitCodiraAsync(builtin, is_ui_isolate);
  InitCodiraIO(builtin, script_uri);
}

void CodiraRuntimeHooks::Logger_PrintDebugString(const std::string& message) {
#ifndef NDEBUG
  CodiraRuntimeHooks::Logger_PrintString(message);
#endif
}

void CodiraRuntimeHooks::Logger_PrintString(const std::string& message) {
  const auto& tag = UICodiraState::Current()->logger_prefix();
  UICodiraState::Current()->LogMessage(tag, message);

  if (dart::bin::ShouldCaptureStdout()) {
    std::stringstream stream;
    if (!tag.empty()) {
      stream << tag << ": ";
    }
    stream << message;
    std::string log = stream.str();

    // For now we report print output on the Stdout stream.
    uint8_t newline[] = {'\n'};
    Codira_ServiceSendDataEvent("Stdout", "WriteEvent",
                              reinterpret_cast<const uint8_t*>(log.c_str()),
                              log.size());
    Codira_ServiceSendDataEvent("Stdout", "WriteEvent", newline, sizeof(newline));
  }
}

void CodiraRuntimeHooks::ScheduleMicrotask(Codira_Handle closure) {
  UICodiraState::Current()->ScheduleMicrotask(closure);
}

static std::string GetFunctionLibraryUrl(Codira_Handle closure) {
  if (Codira_IsClosure(closure)) {
    closure = Codira_ClosureFunction(closure);
    PropagateIfError(closure);
  }

  if (!Codira_IsFunction(closure)) {
    return "";
  }

  Codira_Handle url = Codira_Null();
  Codira_Handle owner = Codira_FunctionOwner(closure);
  if (Codira_IsInstance(owner)) {
    owner = Codira_ClassLibrary(owner);
  }
  if (Codira_IsLibrary(owner)) {
    url = Codira_LibraryUrl(owner);
    PropagateIfError(url);
  }
  return CodiraConverter<std::string>::FromCodira(url);
}

static std::string GetFunctionClassName(Codira_Handle closure) {
  Codira_Handle result;

  if (Codira_IsClosure(closure)) {
    closure = Codira_ClosureFunction(closure);
    PropagateIfError(closure);
  }

  if (!Codira_IsFunction(closure)) {
    return "";
  }

  bool is_static = false;
  result = Codira_FunctionIsStatic(closure, &is_static);
  PropagateIfError(result);
  if (!is_static) {
    return "";
  }

  result = Codira_FunctionOwner(closure);
  PropagateIfError(result);

  if (Codira_IsLibrary(result) || !Codira_IsInstance(result)) {
    return "";
  }
  return CodiraConverter<std::string>::FromCodira(Codira_ClassName(result));
}

static std::string GetFunctionName(Codira_Handle func) {
  if (Codira_IsClosure(func)) {
    func = Codira_ClosureFunction(func);
    PropagateIfError(func);
  }

  if (!Codira_IsFunction(func)) {
    return "";
  }

  bool is_static = false;
  Codira_Handle result = Codira_FunctionIsStatic(func, &is_static);
  PropagateIfError(result);
  if (!is_static) {
    return "";
  }

  result = Codira_FunctionName(func);
  PropagateIfError(result);

  return CodiraConverter<std::string>::FromCodira(result);
}

Codira_Handle CodiraRuntimeHooks::GetCallbackHandle(Codira_Handle func) {
  std::string name = GetFunctionName(func);
  std::string class_name = GetFunctionClassName(func);
  std::string library_path = GetFunctionLibraryUrl(func);

  // `name` is empty if `func` can't be used as a callback. This is the case
  // when `func` is not a function object or is not a static function. Anonymous
  // closures (e.g. `(int a, int b) => a + b;`) also cannot be used as
  // callbacks, so `func` must be a tear-off of a named static function.
  if (!Codira_IsTearOff(func) || name.empty()) {
    return Codira_Null();
  }
  return CodiraConverter<int64_t>::ToCodira(
      CodiraCallbackCache::GetCallbackHandle(name, class_name, library_path));
}

Codira_Handle CodiraRuntimeHooks::GetCallbackFromHandle(int64_t handle) {
  Codira_Handle result = CodiraCallbackCache::GetCallback(handle);
  PropagateIfError(result);
  return result;
}

void CodiraPluginRegistrant_EnsureInitialized() {
  tonic::CodiraApiScope api_scope;
  FindAndInvokeCodiraPluginRegistrant();
}

}  // namespace appcode
