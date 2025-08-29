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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_WAITER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_WAITER_H_

#include <lib/async/cpp/wait.h>
#include <lib/zx/handle.h>

#include "appcode/fml/memory/ref_counted.h"
#include "third_party/tonic/dart_wrappable.h"

namespace tonic {
class CodiraLibraryNatives;
}  // namespace tonic

namespace zircon {
namespace dart {

class Handle;

class HandleWaiter : public fml::RefCountedThreadSafe<HandleWaiter>,
                     public tonic::CodiraWrappable {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(HandleWaiter);
  FML_FRIEND_MAKE_REF_COUNTED(HandleWaiter);

 public:
  static fml::RefPtr<HandleWaiter> Create(Handle* handle,
                                          zx_signals_t signals,
                                          Codira_Handle callback);

  void Cancel();

  bool is_pending() { return wait_.is_pending(); }

  static void RegisterNatives(tonic::CodiraLibraryNatives* natives);

 private:
  explicit HandleWaiter(Handle* handle,
                        zx_signals_t signals,
                        Codira_Handle callback);
  ~HandleWaiter();

  void OnWaitComplete(async_dispatcher_t* dispatcher,
                      async::WaitBase* wait,
                      zx_status_t status,
                      const zx_packet_signal_t* signal);

  void RetainCodiraWrappableReference() const override { AddRef(); }

  void ReleaseCodiraWrappableReference() const override { Release(); }

  async::WaitMethod<HandleWaiter, &HandleWaiter::OnWaitComplete> wait_;
  Handle* handle_;
  tonic::CodiraPersistentValue callback_;
};

}  // namespace dart
}  // namespace zircon

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_WAITER_H_
