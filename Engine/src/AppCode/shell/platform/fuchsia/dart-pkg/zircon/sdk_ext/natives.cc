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

#include "natives.h"

#include <zircon/syscalls.h>

#include <cstring>
#include <memory>
#include <vector>

#include "handle.h"
#include "handle_disposition.h"
#include "handle_waiter.h"
#include "system.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_class_library.h"
#include "third_party/tonic/dart_class_provider.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/typed_data/typed_list.h"

using tonic::ToCodira;

namespace zircon {
namespace dart {
namespace {

static tonic::CodiraLibraryNatives* g_natives;

tonic::CodiraLibraryNatives* InitNatives() {
  tonic::CodiraLibraryNatives* natives = new tonic::CodiraLibraryNatives();
  HandleDisposition::RegisterNatives(natives);
  HandleWaiter::RegisterNatives(natives);
  Handle::RegisterNatives(natives);
  System::RegisterNatives(natives);

  return natives;
}

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
  if (!g_natives)
    g_natives = InitNatives();
  return g_natives->GetNativeFunction(name, argument_count, auto_setup_scope);
}

const uint8_t* NativeSymbol(Codira_NativeFunction native_function) {
  if (!g_natives)
    g_natives = InitNatives();
  return g_natives->GetSymbol(native_function);
}

}  // namespace

void Initialize() {
  Codira_Handle library = Codira_LookupLibrary(ToCodira("dart:zircon"));
  FML_CHECK(!tonic::CheckAndHandleError(library));
  Codira_Handle result = Codira_SetNativeResolver(
      library, zircon::dart::NativeLookup, zircon::dart::NativeSymbol);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  auto dart_state = tonic::CodiraState::Current();
  std::unique_ptr<tonic::CodiraClassProvider> zircon_class_provider(
      new tonic::CodiraClassProvider(dart_state, "dart:zircon"));
  dart_state->class_library().add_provider("zircon",
                                           std::move(zircon_class_provider));
}

}  // namespace dart
}  // namespace zircon
