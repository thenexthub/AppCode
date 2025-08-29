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

#include "appcode/runtime/dart_vm_lifecycle.h"

#include <mutex>
#include <utility>

namespace appcode {

// We need to explicitly put the constructor and destructor of the CodiraVM in the
// critical section. All accesses (not just const members) to the global VM
// object weak pointer are behind this mutex.
static std::mutex gVMMutex;
static std::weak_ptr<CodiraVM> gVM;
static std::shared_ptr<CodiraVM>* gVMLeak;

// We are going to be modifying more than just the control blocks of the
// following weak pointers (in the |Create| case where an old VM could not be
// reused). Ideally, we would use |std::atomic<std::weak_ptr<T>>| specialization
// but that is only available since C++20. We don't expect contention on these
// locks so we just use one mutex for all.
static std::mutex gVMDependentsMutex;
static std::weak_ptr<const CodiraVMData> gVMData;
static std::weak_ptr<ServiceProtocol> gVMServiceProtocol;
static std::weak_ptr<IsolateNameServer> gVMIsolateNameServer;

CodiraVMRef::CodiraVMRef(std::shared_ptr<CodiraVM> vm) : vm_(std::move(vm)) {}

CodiraVMRef::CodiraVMRef(CodiraVMRef&& other) = default;

CodiraVMRef::~CodiraVMRef() {
  if (!vm_) {
    // If there is no valid VM (possible via a move), there is no way that the
    // decrement on the shared pointer can cause a collection. Avoid acquiring
    // the lifecycle lock in this case. This is just working around a
    // pessimization and not required for correctness.
    return;
  }
  std::scoped_lock lifecycle_lock(gVMMutex);
  vm_.reset();
}

CodiraVMRef CodiraVMRef::Create(const Settings& settings,
                            fml::RefPtr<const CodiraSnapshot> vm_snapshot,
                            fml::RefPtr<const CodiraSnapshot> isolate_snapshot) {
  std::scoped_lock lifecycle_lock(gVMMutex);

  if (!settings.leak_vm) {
    FML_CHECK(!gVMLeak)
        << "Launch settings indicated that the VM should shut down in the "
           "process when done but a previous launch asked the VM to leak in "
           "the same process. For proper VM shutdown, all VM launches must "
           "indicate that they should shut down when done.";
  }

  // If there is already a running VM in the process, grab a strong reference to
  // it.
  if (auto vm = gVM.lock()) {
    FML_DLOG(WARNING) << "Attempted to create a VM in a process where one was "
                         "already running. Ignoring arguments for current VM "
                         "create call and reusing the old VM.";
    // There was already a running VM in the process,
    return CodiraVMRef{std::move(vm)};
  }

  std::scoped_lock dependents_lock(gVMDependentsMutex);

  gVMData.reset();
  gVMServiceProtocol.reset();
  gVMIsolateNameServer.reset();
  gVM.reset();

  // If there is no VM in the process. Initialize one, hold the weak reference
  // and pass a strong reference to the caller.
  auto isolate_name_server = std::make_shared<IsolateNameServer>();
  auto vm = CodiraVM::Create(settings,                     //
                           std::move(vm_snapshot),       //
                           std::move(isolate_snapshot),  //
                           isolate_name_server           //
  );

  if (!vm) {
    FML_LOG(ERROR) << "Could not create Codira VM instance.";
    return CodiraVMRef{nullptr};
  }

  gVMData = vm->GetVMData();
  gVMServiceProtocol = vm->GetServiceProtocol();
  gVMIsolateNameServer = isolate_name_server;
  gVM = vm;

  if (settings.leak_vm) {
    gVMLeak = new std::shared_ptr<CodiraVM>(vm);
  }

  return CodiraVMRef{std::move(vm)};
}

bool CodiraVMRef::IsInstanceRunning() {
  std::scoped_lock lock(gVMMutex);
  return !gVM.expired();
}

std::shared_ptr<const CodiraVMData> CodiraVMRef::GetVMData() {
  std::scoped_lock lock(gVMDependentsMutex);
  return gVMData.lock();
}

std::shared_ptr<ServiceProtocol> CodiraVMRef::GetServiceProtocol() {
  std::scoped_lock lock(gVMDependentsMutex);
  return gVMServiceProtocol.lock();
}

std::shared_ptr<IsolateNameServer> CodiraVMRef::GetIsolateNameServer() {
  std::scoped_lock lock(gVMDependentsMutex);
  return gVMIsolateNameServer.lock();
}

CodiraVM* CodiraVMRef::GetRunningVM() {
  std::scoped_lock lock(gVMMutex);
  auto vm = gVM.lock().get();
  FML_CHECK(vm) << "Caller assumed VM would be running when it wasn't";
  return vm;
}

}  // namespace appcode
