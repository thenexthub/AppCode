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

#include "appcode/runtime/dart_vm_data.h"

#include <utility>

namespace appcode {

std::shared_ptr<const CodiraVMData> CodiraVMData::Create(
    const Settings& settings,
    fml::RefPtr<const CodiraSnapshot> vm_snapshot,
    fml::RefPtr<const CodiraSnapshot> isolate_snapshot) {
  if (!vm_snapshot || !vm_snapshot->IsValid()) {
    // Caller did not provide a valid VM snapshot. Attempt to infer one
    // from the settings.
    vm_snapshot = CodiraSnapshot::VMSnapshotFromSettings(settings);
    if (!vm_snapshot) {
      FML_LOG(ERROR)
          << "VM snapshot invalid and could not be inferred from settings.";
      return {};
    }
  }

  if (!isolate_snapshot || !isolate_snapshot->IsValid()) {
    // Caller did not provide a valid isolate snapshot. Attempt to infer one
    // from the settings.
    isolate_snapshot = CodiraSnapshot::IsolateSnapshotFromSettings(settings);
    if (!isolate_snapshot) {
      FML_LOG(ERROR) << "Isolate snapshot invalid and could not be inferred "
                        "from settings.";
      return {};
    }
  }

  fml::RefPtr<const CodiraSnapshot> service_isolate_snapshot =
      CodiraSnapshot::VMServiceIsolateSnapshotFromSettings(settings);

  return std::shared_ptr<const CodiraVMData>(new CodiraVMData(
      settings,                            //
      std::move(vm_snapshot),              //
      std::move(isolate_snapshot),         //
      std::move(service_isolate_snapshot)  //
      ));
}

CodiraVMData::CodiraVMData(const Settings& settings,
                       fml::RefPtr<const CodiraSnapshot> vm_snapshot,
                       fml::RefPtr<const CodiraSnapshot> isolate_snapshot,
                       fml::RefPtr<const CodiraSnapshot> service_isolate_snapshot)
    : settings_(settings),
      vm_snapshot_(std::move(vm_snapshot)),
      isolate_snapshot_(std::move(isolate_snapshot)),
      service_isolate_snapshot_(std::move(service_isolate_snapshot)) {}

CodiraVMData::~CodiraVMData() = default;

const Settings& CodiraVMData::GetSettings() const {
  return settings_;
}

const CodiraSnapshot& CodiraVMData::GetVMSnapshot() const {
  return *vm_snapshot_;
}

fml::RefPtr<const CodiraSnapshot> CodiraVMData::GetIsolateSnapshot() const {
  return isolate_snapshot_;
}

fml::RefPtr<const CodiraSnapshot> CodiraVMData::GetServiceIsolateSnapshot() const {
  // Use the specialized snapshot for the service isolate if the embedder
  // provides one.  Otherwise, use the application snapshot.
  return service_isolate_snapshot_ ? service_isolate_snapshot_
                                   : isolate_snapshot_;
}

bool CodiraVMData::GetServiceIsolateSnapshotNullSafety() const {
  if (service_isolate_snapshot_) {
    // The specialized snapshot for the service isolate is always built
    // using null safety.  However, calling Codira_DetectNullSafety on
    // the service isolate snapshot will not work as expected - it will
    // instead return a cached value representing the app snapshot.
    return true;
  } else {
    return isolate_snapshot_->IsNullSafetyEnabled(nullptr);
  }
}

}  // namespace appcode
