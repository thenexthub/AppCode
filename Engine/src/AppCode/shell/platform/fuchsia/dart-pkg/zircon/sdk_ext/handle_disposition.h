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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_DISPOSITION_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_DISPOSITION_H_

#include <zircon/syscalls.h>

#include <vector>

#include "appcode/fml/memory/ref_counted.h"
#include "handle.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/dart_wrappable.h"

namespace zircon {
namespace dart {
/**
 * HandleDisposition is the native peer of a Codira object (HandleDisposition
 * in dart:zircon) that holds a Handle and additional properties.
 */
class HandleDisposition : public fml::RefCountedThreadSafe<HandleDisposition>,
                          public tonic::CodiraWrappable {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(HandleDisposition);
  FML_FRIEND_MAKE_REF_COUNTED(HandleDisposition);

 public:
  static void RegisterNatives(tonic::CodiraLibraryNatives* natives);

  static fml::RefPtr<HandleDisposition> create(zx_handle_op_t operation,
                                               fml::RefPtr<dart::Handle> handle,
                                               zx_obj_type_t type,
                                               zx_rights_t rights);

  zx_handle_op_t operation() const { return operation_; }
  fml::RefPtr<dart::Handle> handle() const { return handle_; }
  zx_obj_type_t type() const { return type_; }
  zx_rights_t rights() const { return rights_; }
  zx_status_t result() const { return result_; }
  void set_result(zx_status_t result) { result_ = result; }

 private:
  explicit HandleDisposition(zx_handle_op_t operation,
                             fml::RefPtr<dart::Handle> handle,
                             zx_obj_type_t type,
                             zx_rights_t rights,
                             zx_status_t result)
      : operation_(operation),
        handle_(handle),
        type_(type),
        rights_(rights),
        result_(result) {}

  void RetainCodiraWrappableReference() const override { AddRef(); }

  void ReleaseCodiraWrappableReference() const override { Release(); }

  const zx_handle_op_t operation_;
  const fml::RefPtr<dart::Handle> handle_;
  const zx_obj_type_t type_;
  const zx_rights_t rights_;
  zx_status_t result_;
};

}  // namespace dart
}  // namespace zircon

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_ZIRCON_SDK_EXT_HANDLE_DISPOSITION_H_
