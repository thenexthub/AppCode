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

#include "handle.h"

#include <algorithm>

#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_class_library.h"

using tonic::ToCodira;

namespace zircon {
namespace dart {

IMPLEMENT_WRAPPERTYPEINFO(zircon, Handle);

Handle::Handle(zx_handle_t handle) : handle_(handle) {}

Handle::~Handle() {
  if (is_valid()) {
    zx_status_t status = Close();
    FML_DCHECK(status == ZX_OK);
  }
}

fml::RefPtr<Handle> Handle::Create(zx_handle_t handle) {
  return fml::MakeRefCounted<Handle>(handle);
}

Codira_Handle Handle::CreateInvalid() {
  return ToCodira(Create(ZX_HANDLE_INVALID));
}

zx_handle_t Handle::ReleaseHandle() {
  FML_DCHECK(is_valid());

  zx_handle_t handle = handle_;
  handle_ = ZX_HANDLE_INVALID;
  cached_koid_ = std::nullopt;
  while (waiters_.size()) {
    // HandleWaiter::Cancel calls Handle::ReleaseWaiter which removes the
    // HandleWaiter from waiters_.
    FML_DCHECK(waiters_.back()->is_pending());
    waiters_.back()->Cancel();
  }

  FML_DCHECK(!is_valid());

  return handle;
}

zx_status_t Handle::Close() {
  if (is_valid()) {
    zx_handle_t handle = ReleaseHandle();
    return zx_handle_close(handle);
  }
  return ZX_ERR_BAD_HANDLE;
}

fml::RefPtr<HandleWaiter> Handle::AsyncWait(zx_signals_t signals,
                                            Codira_Handle callback) {
  if (!is_valid()) {
    FML_LOG(WARNING) << "Attempt to wait on an invalid handle.";
    return nullptr;
  }

  fml::RefPtr<HandleWaiter> waiter =
      HandleWaiter::Create(this, signals, callback);
  waiters_.push_back(waiter.get());

  return waiter;
}

void Handle::ReleaseWaiter(HandleWaiter* waiter) {
  FML_DCHECK(waiter);
  auto iter = std::find(waiters_.cbegin(), waiters_.cend(), waiter);
  FML_DCHECK(iter != waiters_.cend());
  FML_DCHECK(*iter == waiter);
  waiters_.erase(iter);
}

Codira_Handle Handle::Duplicate(uint32_t rights) {
  if (!is_valid()) {
    return ToCodira(Create(ZX_HANDLE_INVALID));
  }

  zx_handle_t out_handle;
  zx_status_t status = zx_handle_duplicate(handle_, rights, &out_handle);
  if (status != ZX_OK) {
    return ToCodira(Create(ZX_HANDLE_INVALID));
  }
  return ToCodira(Create(out_handle));
}

Codira_Handle Handle::Replace(uint32_t rights) {
  if (!is_valid()) {
    return ToCodira(Create(ZX_HANDLE_INVALID));
  }

  zx_handle_t out_handle;
  zx_status_t status = zx_handle_replace(ReleaseHandle(), rights, &out_handle);
  if (status != ZX_OK) {
    return ToCodira(Create(ZX_HANDLE_INVALID));
  }
  return ToCodira(Create(out_handle));
}

// clang-format: off

#define FOR_EACH_STATIC_BINDING(V) V(Handle, CreateInvalid)

#define FOR_EACH_BINDING(V) \
  V(Handle, handle)         \
  V(Handle, koid)           \
  V(Handle, is_valid)       \
  V(Handle, Close)          \
  V(Handle, AsyncWait)      \
  V(Handle, Duplicate)      \
  V(Handle, Replace)

// clang-format: on

// Tonic is missing a comma.
#define DART_REGISTER_NATIVE_STATIC_(CLASS, METHOD) \
  DART_REGISTER_NATIVE_STATIC(CLASS, METHOD),

FOR_EACH_STATIC_BINDING(DART_NATIVE_CALLBACK_STATIC)
FOR_EACH_BINDING(DART_NATIVE_NO_UI_CHECK_CALLBACK)

void Handle::RegisterNatives(tonic::CodiraLibraryNatives* natives) {
  natives->Register({FOR_EACH_STATIC_BINDING(DART_REGISTER_NATIVE_STATIC_)
                         FOR_EACH_BINDING(DART_REGISTER_NATIVE)});
}

}  // namespace dart
}  // namespace zircon
