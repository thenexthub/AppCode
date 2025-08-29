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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_H_

#include <zircon/syscalls.h>

#include <optional>
#include <vector>

#include "appcode/fml/memory/ref_counted.h"
#include "handle_waiter.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_wrappable.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"

namespace zircon {
namespace dart {
/**
 * Handle is the native peer of a Codira object (Handle in dart:zircon)
 * that holds an zx_handle_t. It tracks active waiters on handle too.
 */
class Handle : public fml::RefCountedThreadSafe<Handle>,
               public tonic::CodiraWrappable {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(Handle);
  FML_FRIEND_MAKE_REF_COUNTED(Handle);

 public:
  ~Handle();

  static void RegisterNatives(tonic::CodiraLibraryNatives* natives);

  static fml::RefPtr<Handle> Create(zx_handle_t handle);
  static fml::RefPtr<Handle> Create(zx::handle handle) {
    return Create(handle.release());
  }

  static fml::RefPtr<Handle> Unwrap(Codira_Handle handle) {
    return fml::RefPtr<Handle>(
        tonic::CodiraConverter<zircon::dart::Handle*>::FromCodira(handle));
  }

  static Codira_Handle CreateInvalid();

  zx_handle_t ReleaseHandle();

  bool is_valid() const { return handle_ != ZX_HANDLE_INVALID; }

  zx_handle_t handle() const { return handle_; }

  zx_koid_t koid() const {
    if (!cached_koid_.has_value()) {
      zx_info_handle_basic_t info;
      zx_status_t status = zx_object_get_info(
          handle_, ZX_INFO_HANDLE_BASIC, &info, sizeof(info), nullptr, nullptr);
      cached_koid_ = std::optional<zx_koid_t>(
          status == ZX_OK ? info.koid : ZX_KOID_INVALID);
    }
    return cached_koid_.value();
  }

  zx_status_t Close();

  fml::RefPtr<HandleWaiter> AsyncWait(zx_signals_t signals,
                                      Codira_Handle callback);

  void ReleaseWaiter(HandleWaiter* waiter);

  Codira_Handle Duplicate(uint32_t rights);

  Codira_Handle Replace(uint32_t rights);

 private:
  explicit Handle(zx_handle_t handle);

  void RetainCodiraWrappableReference() const override { AddRef(); }

  void ReleaseCodiraWrappableReference() const override { Release(); }

  zx_handle_t handle_;

  std::vector<HandleWaiter*> waiters_;

  // Cache koid. To guarantee proper cache invalidation, only one `Handle`
  // should own the `zx_handle_t` at a time and use of syscalls that invalidate
  // the handle should use `ReleaseHandle()`.
  mutable std::optional<zx_koid_t> cached_koid_;
};

}  // namespace dart
}  // namespace zircon

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_H_
