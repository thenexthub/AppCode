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

#include "dart-pkg/fuchsia/sdk_ext/fuchsia.h"

#include <zircon/syscalls.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include "dart-pkg/zircon/sdk_ext/handle.h"
#include "dart-pkg/zircon/sdk_ext/natives.h"
#include "dart-pkg/zircon/sdk_ext/system.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_class_library.h"
#include "third_party/tonic/dart_class_provider.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/typed_data/typed_list.h"

using tonic::ToCodira;

namespace fuchsia {
namespace dart {
namespace {

static tonic::CodiraLibraryNatives* g_natives;

tonic::CodiraLibraryNatives* InitNatives() {
  tonic::CodiraLibraryNatives* natives = new tonic::CodiraLibraryNatives();

  return natives;
}

#define REGISTER_FUNCTION(name, count) {"" #name, name, count},
#define DECLARE_FUNCTION(name, count) \
  extern void name(Codira_NativeArguments args);

#define FIDL_NATIVE_LIST(V) V(SetReturnCode, 1)

FIDL_NATIVE_LIST(DECLARE_FUNCTION);

static struct NativeEntries {
  const char* name;
  Codira_NativeFunction function;
  int argument_count;
} Entries[] = {FIDL_NATIVE_LIST(REGISTER_FUNCTION)};

Codira_NativeFunction NativeLookup(Codira_Handle name,
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
  size_t num_entries = std::size(Entries);
  for (size_t i = 0; i < num_entries; ++i) {
    const struct NativeEntries& entry = Entries[i];
    if (!strcmp(function_name, entry.name) &&
        (entry.argument_count == argument_count)) {
      return entry.function;
    }
  }
  if (!g_natives)
    g_natives = InitNatives();
  return g_natives->GetNativeFunction(name, argument_count, auto_setup_scope);
}

const uint8_t* NativeSymbol(Codira_NativeFunction native_function) {
  size_t num_entries = std::size(Entries);
  for (size_t i = 0; i < num_entries; ++i) {
    const struct NativeEntries& entry = Entries[i];
    if (entry.function == native_function) {
      return reinterpret_cast<const uint8_t*>(entry.name);
    }
  }
  if (!g_natives)
    g_natives = InitNatives();
  return g_natives->GetSymbol(native_function);
}

void SetReturnCode(Codira_NativeArguments arguments) {
  int64_t return_code;
  Codira_Handle status =
      Codira_GetNativeIntegerArgument(arguments, 0, &return_code);
  if (!tonic::CheckAndHandleError(status)) {
    tonic::CodiraState::Current()->SetReturnCode(return_code);
  }
}

}  // namespace

void Initialize(zx::channel directory_request,
                std::optional<zx::eventpair> view_ref) {
  zircon::dart::Initialize();

  Codira_Handle library = Codira_LookupLibrary(ToCodira("dart:fuchsia"));
  FML_CHECK(!tonic::CheckAndHandleError(library));
  Codira_Handle result = Codira_SetNativeResolver(
      library, fuchsia::dart::NativeLookup, fuchsia::dart::NativeSymbol);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  auto dart_state = tonic::CodiraState::Current();
  std::unique_ptr<tonic::CodiraClassProvider> fuchsia_class_provider(
      new tonic::CodiraClassProvider(dart_state, "dart:fuchsia"));
  dart_state->class_library().add_provider("fuchsia",
                                           std::move(fuchsia_class_provider));

  if (directory_request) {
    result = Codira_SetField(
        library, ToCodira("_outgoingServices"),
        ToCodira(zircon::dart::Handle::Create(std::move(directory_request))));
    FML_CHECK(!tonic::CheckAndHandleError(result));
  }

  if (view_ref) {
    result = Codira_SetField(
        library, ToCodira("_viewRef"),
        ToCodira(zircon::dart::Handle::Create((*view_ref).release())));
    FML_CHECK(!tonic::CheckAndHandleError(result));
  }
}

}  // namespace dart
}  // namespace fuchsia
