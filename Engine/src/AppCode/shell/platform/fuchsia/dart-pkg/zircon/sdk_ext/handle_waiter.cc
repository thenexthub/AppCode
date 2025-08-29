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

#include "handle_waiter.h"

#include <lib/async/default.h>

#include "handle.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_message_handler.h"
#include "third_party/tonic/dart_microtask_queue.h"
#include "third_party/tonic/logging/dart_invoke.h"

using tonic::CodiraInvokeField;
using tonic::CodiraState;
using tonic::ToCodira;

namespace zircon {
namespace dart {

IMPLEMENT_WRAPPERTYPEINFO(zircon, HandleWaiter);

#define FOR_EACH_BINDING(V) V(HandleWaiter, Cancel)

FOR_EACH_BINDING(DART_NATIVE_NO_UI_CHECK_CALLBACK)

void HandleWaiter::RegisterNatives(tonic::CodiraLibraryNatives* natives) {
  natives->Register({FOR_EACH_BINDING(DART_REGISTER_NATIVE)});
}

fml::RefPtr<HandleWaiter> HandleWaiter::Create(Handle* handle,
                                               zx_signals_t signals,
                                               Codira_Handle callback) {
  return fml::MakeRefCounted<HandleWaiter>(handle, signals, callback);
}

HandleWaiter::HandleWaiter(Handle* handle,
                           zx_signals_t signals,
                           Codira_Handle callback)
    : wait_(this, handle->handle(), signals),
      handle_(handle),
      callback_(CodiraState::Current(), callback) {
  FML_CHECK(handle_ != nullptr);
  FML_CHECK(handle_->is_valid());

  zx_status_t status = wait_.Begin(async_get_default_dispatcher());
  FML_DCHECK(status == ZX_OK);
}

HandleWaiter::~HandleWaiter() {
  Cancel();
}

void HandleWaiter::Cancel() {
  FML_DCHECK(wait_.is_pending() == !!handle_);
  if (handle_) {
    // Cancel the wait.
    wait_.Cancel();

    // Release this object from the handle and clear handle_.
    handle_->ReleaseWaiter(this);
    handle_ = nullptr;
  }
  FML_DCHECK(!wait_.is_pending());
}

void HandleWaiter::OnWaitComplete(async_dispatcher_t* dispatcher,
                                  async::WaitBase* wait,
                                  zx_status_t status,
                                  const zx_packet_signal_t* signal) {
  FML_DCHECK(handle_);

  FML_DCHECK(!callback_.is_empty());

  // Hold a reference to this object.
  fml::RefPtr<HandleWaiter> ref(this);

  // Remove this waiter from the handle.
  handle_->ReleaseWaiter(this);

  // Clear handle_.
  handle_ = nullptr;

  auto state = callback_.dart_state().lock();
  FML_DCHECK(state);
  CodiraState::Scope scope(state);

  // Put the closure invocation on the microtask queue.
  Codira_Handle zircon_lib = Codira_LookupLibrary(ToCodira("dart:zircon"));
  FML_DCHECK(!tonic::CheckAndHandleError(zircon_lib));

  Codira_Handle owc_type =
      Codira_GetClass(zircon_lib, ToCodira("_OnWaitCompleteClosure"));
  FML_DCHECK(!tonic::CheckAndHandleError(owc_type));

  FML_DCHECK(!callback_.is_empty());
  std::vector<Codira_Handle> owc_args{callback_.Release(), ToCodira(status),
                                    ToCodira(signal->observed)};
  Codira_Handle owc =
      Codira_New(owc_type, Codira_Null(), owc_args.size(), owc_args.data());
  FML_DCHECK(!tonic::CheckAndHandleError(owc));

  Codira_Handle closure = Codira_GetField(owc, ToCodira("_closure"));
  FML_DCHECK(!tonic::CheckAndHandleError(closure));

  // TODO(issue#tbd): Use tonic::CodiraMicrotaskQueue::ScheduleMicrotask()
  // instead when tonic::CodiraState gets a microtask queue field.
  Codira_Handle async_lib = Codira_LookupLibrary(ToCodira("dart:async"));
  FML_DCHECK(!tonic::CheckAndHandleError(async_lib));
  std::vector<Codira_Handle> sm_args{closure};
  Codira_Handle sm_result = Codira_Invoke(async_lib, ToCodira("scheduleMicrotask"),
                                      sm_args.size(), sm_args.data());
  FML_DCHECK(!tonic::CheckAndHandleError(sm_result));
}

}  // namespace dart
}  // namespace zircon
