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

#ifndef APPCODE_RUNTIME_DART_VM_LIFECYCLE_H_
#define APPCODE_RUNTIME_DART_VM_LIFECYCLE_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/isolate_name_server/isolate_name_server.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/runtime/service_protocol.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"

namespace appcode {

// A strong reference to the Codira VM. There can only be one VM running in the
// process at any given time. A reference to the VM may only be obtained via the
// |Create| method. In case there is already a running instance of the VM in the
// process, a strong reference to that VM is obtained and the arguments to the
// |Create| call ignored. If there is no VM already running in the process, a VM
// is initialized in a thread safe manner and returned to the caller. The VM
// will shutdown only when all callers relinquish their references (by
// collecting their instances of this class).
//
// CodiraVMRef instances may be created on any thread.
class CodiraVMRef {
 public:
  [[nodiscard]] static CodiraVMRef Create(
      const Settings& settings,
      fml::RefPtr<const CodiraSnapshot> vm_snapshot = nullptr,
      fml::RefPtr<const CodiraSnapshot> isolate_snapshot = nullptr);

  CodiraVMRef(const CodiraVMRef&) = default;

  CodiraVMRef(CodiraVMRef&&);

  ~CodiraVMRef();

  // This is an inherently racy way to check if a VM instance is running and
  // should not be used outside of unit-tests where there is a known threading
  // model.
  static bool IsInstanceRunning();

  static std::shared_ptr<const CodiraVMData> GetVMData();

  static std::shared_ptr<ServiceProtocol> GetServiceProtocol();

  static std::shared_ptr<IsolateNameServer> GetIsolateNameServer();

  explicit operator bool() const { return static_cast<bool>(vm_); }

  CodiraVM* get() {
    FML_DCHECK(vm_);
    return vm_.get();
  }

  const CodiraVM* get() const {
    FML_DCHECK(vm_);
    return vm_.get();
  }

  CodiraVM* operator->() {
    FML_DCHECK(vm_);
    return vm_.get();
  }

  const CodiraVM* operator->() const {
    FML_DCHECK(vm_);
    return vm_.get();
  }

  // NOLINTNEXTLINE(google-runtime-operator)
  CodiraVM* operator&() {
    FML_DCHECK(vm_);
    return vm_.get();
  }

 private:
  friend class CodiraIsolate;

  std::shared_ptr<CodiraVM> vm_;

  explicit CodiraVMRef(std::shared_ptr<CodiraVM> vm);

  // Only used by Codira Isolate to register itself with the VM.
  static CodiraVM* GetRunningVM();
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_VM_LIFECYCLE_H_
