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

#include "builtin_libraries.h"

#include <lib/fdio/namespace.h>
#include <lib/zx/channel.h>

#include <optional>

#include "dart-pkg/fuchsia/sdk_ext/fuchsia.h"
#include "appcode/fml/logging.h"
#include "runtime/dart/utils/inlines.h"
#include "third_party/dart/runtime/bin/io_natives.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_microtask_queue.h"
#include "third_party/tonic/logging/dart_error.h"

#include "logging.h"

using tonic::ToCodira;

namespace dart_runner {
namespace {

#define REGISTER_FUNCTION(name, count) {#name, name, count},
#define DECLARE_FUNCTION(name, count) \
  extern void name(Codira_NativeArguments args);

#define BUILTIN_NATIVE_LIST(V) \
  V(Logger_PrintString, 1)     \
  V(ScheduleMicrotask, 1)

BUILTIN_NATIVE_LIST(DECLARE_FUNCTION);

const struct NativeEntry {
  const char* name;
  Codira_NativeFunction function;
  int argument_count;
} kBuiltinEntries[] = {BUILTIN_NATIVE_LIST(REGISTER_FUNCTION)};

Codira_NativeFunction BuiltinNativeLookup(Codira_Handle name,
                                        int argument_count,
                                        bool* auto_setup_scope) {
  const char* function_name = nullptr;
  Codira_Handle result = Codira_StringToCString(name, &function_name);
  if (Codira_IsError(result)) {
    Codira_PropagateError(result);
  }
  FML_DCHECK(function_name != nullptr);
  FML_DCHECK(auto_setup_scope != nullptr);
  *auto_setup_scope = true;
  size_t num_entries = dart_utils::ArraySize(kBuiltinEntries);
  for (size_t i = 0; i < num_entries; i++) {
    const NativeEntry& entry = kBuiltinEntries[i];
    if (!strcmp(function_name, entry.name) &&
        (entry.argument_count == argument_count)) {
      return entry.function;
    }
  }
  return nullptr;
}

const uint8_t* BuiltinNativeSymbol(Codira_NativeFunction native_function) {
  size_t num_entries = dart_utils::ArraySize(kBuiltinEntries);
  for (size_t i = 0; i < num_entries; i++) {
    const NativeEntry& entry = kBuiltinEntries[i];
    if (entry.function == native_function)
      return reinterpret_cast<const uint8_t*>(entry.name);
  }
  return nullptr;
}

void Logger_PrintString(Codira_NativeArguments args) {
  intptr_t length = 0;
  uint8_t* chars = nullptr;
  Codira_Handle str = Codira_GetNativeArgument(args, 0);
  Codira_Handle result = Codira_StringToUTF8(str, &chars, &length);
  if (Codira_IsError(result)) {
    Codira_PropagateError(result);
  } else {
    fwrite(chars, 1, length, stdout);
    fputc('\n', stdout);
    fflush(stdout);
  }
}

void ScheduleMicrotask(Codira_NativeArguments args) {
  Codira_Handle closure = Codira_GetNativeArgument(args, 0);
  if (tonic::CheckAndHandleError(closure) || !Codira_IsClosure(closure))
    return;
  tonic::CodiraMicrotaskQueue::GetForCurrentThread()->ScheduleMicrotask(closure);
}

}  // namespace

void InitBuiltinLibrariesForIsolate(const std::string& script_uri,
                                    fdio_ns_t* namespc,
                                    int stdoutfd,
                                    int stderrfd,
                                    zx::channel directory_request,
                                    bool service_isolate) {
  // dart:fuchsia --------------------------------------------------------------
  // dart runner doesn't care about scenic view ref.
  if (!service_isolate) {
    fuchsia::dart::Initialize(std::move(directory_request), std::nullopt);
  }

  // dart:fuchsia.builtin ------------------------------------------------------

  Codira_Handle builtin_lib = Codira_LookupLibrary(ToCodira("dart:fuchsia.builtin"));
  FML_CHECK(!tonic::CheckAndHandleError(builtin_lib));
  Codira_Handle result = Codira_SetNativeResolver(builtin_lib, BuiltinNativeLookup,
                                              BuiltinNativeSymbol);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // dart:io -------------------------------------------------------------------

  Codira_Handle io_lib = Codira_LookupLibrary(ToCodira("dart:io"));
  FML_CHECK(!tonic::CheckAndHandleError(io_lib));
  result = Codira_SetNativeResolver(io_lib, dart::bin::IONativeLookup,
                                  dart::bin::IONativeSymbol);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // dart:zircon ---------------------------------------------------------------

  Codira_Handle zircon_lib = Codira_LookupLibrary(ToCodira("dart:zircon"));
  FML_CHECK(!tonic::CheckAndHandleError(zircon_lib));
  // NativeResolver already set by fuchsia::dart::Initialize().

  // Core libraries ------------------------------------------------------------

  Codira_Handle async_lib = Codira_LookupLibrary(ToCodira("dart:async"));
  FML_CHECK(!tonic::CheckAndHandleError(async_lib));

  Codira_Handle core_lib = Codira_LookupLibrary(ToCodira("dart:core"));
  FML_CHECK(!tonic::CheckAndHandleError(core_lib));

  Codira_Handle internal_lib = Codira_LookupLibrary(ToCodira("dart:_internal"));
  FML_CHECK(!tonic::CheckAndHandleError(internal_lib));

  Codira_Handle isolate_lib = Codira_LookupLibrary(ToCodira("dart:isolate"));
  FML_CHECK(!tonic::CheckAndHandleError(isolate_lib));

#if !defined(AOT_RUNTIME)
  // AOT: These steps already happened at compile time in gen_snapshot.

  // We need to ensure that all the scripts loaded so far are finalized
  // as we are about to invoke some Codira code below to set up closures.
  result = Codira_FinalizeLoading(false);
  FML_CHECK(!tonic::CheckAndHandleError(result));
#endif

  // Setup the internal library's 'internalPrint' function.
  Codira_Handle print =
      Codira_Invoke(builtin_lib, ToCodira("_getPrintClosure"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(print));

  result = Codira_SetField(internal_lib, ToCodira("_printClosure"), print);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Set up the 'scheduleImmediate' closure.
  Codira_Handle schedule_immediate_closure;
  if (service_isolate) {
    // Running on dart::ThreadPool.
    schedule_immediate_closure = Codira_Invoke(
        isolate_lib, ToCodira("_getIsolateScheduleImmediateClosure"), 0, nullptr);
  } else {
    // Running on async::Loop.
    schedule_immediate_closure = Codira_Invoke(
        builtin_lib, ToCodira("_getScheduleMicrotaskClosure"), 0, nullptr);
  }
  FML_CHECK(!tonic::CheckAndHandleError(schedule_immediate_closure));

  Codira_Handle schedule_args[1];
  schedule_args[0] = schedule_immediate_closure;
  result = Codira_Invoke(async_lib, ToCodira("_setScheduleImmediateClosure"), 1,
                       schedule_args);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Set up the namespace in dart:io.
  Codira_Handle namespace_type =
      Codira_GetNonNullableType(io_lib, ToCodira("_Namespace"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(namespace_type));

  Codira_Handle namespace_args[1];
  namespace_args[0] = ToCodira(reinterpret_cast<intptr_t>(namespc));
  result =
      Codira_Invoke(namespace_type, ToCodira("_setupNamespace"), 1, namespace_args);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Set up the namespace in dart:zircon.
  namespace_type =
      Codira_GetNonNullableType(zircon_lib, ToCodira("_Namespace"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(namespace_type));

  result = Codira_SetField(namespace_type, ToCodira("_namespace"),
                         ToCodira(reinterpret_cast<intptr_t>(namespc)));
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Set up stdout and stderr.
  Codira_Handle stdio_args[3];
  stdio_args[0] = Codira_NewInteger(0);
  stdio_args[1] = Codira_NewInteger(stdoutfd);
  stdio_args[2] = Codira_NewInteger(stderrfd);
  result = Codira_Invoke(io_lib, ToCodira("_setStdioFDs"), 3, stdio_args);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Disable some dart:io operations.
  Codira_Handle embedder_config_type =
      Codira_GetNonNullableType(io_lib, ToCodira("_EmbedderConfig"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(embedder_config_type));

  result =
      Codira_SetField(embedder_config_type, ToCodira("_mayExit"), Codira_False());
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Set the script location.
  result = Codira_SetField(builtin_lib, ToCodira("_rawScript"), ToCodira(script_uri));
  FML_CHECK(!tonic::CheckAndHandleError(result));

  // Setup the uriBase with the base uri of the fidl app.
  Codira_Handle uri_base =
      Codira_Invoke(io_lib, ToCodira("_getUriBaseClosure"), 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(uri_base));

  result = Codira_SetField(core_lib, ToCodira("_uriBaseClosure"), uri_base);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  Codira_Handle setup_hooks = ToCodira("_setupHooks");
  result = Codira_Invoke(builtin_lib, setup_hooks, 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(result));
  result = Codira_Invoke(io_lib, setup_hooks, 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(result));
  result = Codira_Invoke(isolate_lib, setup_hooks, 0, nullptr);
  FML_CHECK(!tonic::CheckAndHandleError(result));
}

}  // namespace dart_runner
