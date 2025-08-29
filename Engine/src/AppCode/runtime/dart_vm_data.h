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

#ifndef APPCODE_RUNTIME_DART_VM_DATA_H_
#define APPCODE_RUNTIME_DART_VM_DATA_H_

#include "appcode/fml/macros.h"
#include "appcode/runtime/dart_snapshot.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      Provides thread-safe access to data that is necessary to
///             bootstrap a new Codira VM instance. All snapshots referenced by
///             this object are read-only.
///
class CodiraVMData {
 public:
  //----------------------------------------------------------------------------
  /// @brief      Creates a new instance of `CodiraVMData`. Both the VM and
  ///             isolate snapshot members are optional and may be `nullptr`. If
  ///             `nullptr`, the snapshot resolvers present in the settings
  ///             object are used to infer the snapshots. If the snapshots
  ///             cannot be inferred from the settings object, this method
  ///             return `nullptr`.
  ///
  /// @param[in]  settings          The settings used to infer the VM and
  ///                               isolate snapshots if they are not provided
  ///                               directly.
  /// @param[in]  vm_snapshot       The VM snapshot or `nullptr`.
  /// @param[in]  isolate_snapshot  The isolate snapshot or `nullptr`.
  ///
  /// @return     A new instance of VM data that can be used to bootstrap a Codira
  ///             VM. `nullptr` if the snapshots are not provided and cannot be
  ///             inferred from the settings object.
  ///
  static std::shared_ptr<const CodiraVMData> Create(
      const Settings& settings,
      fml::RefPtr<const CodiraSnapshot> vm_snapshot,
      fml::RefPtr<const CodiraSnapshot> isolate_snapshot);

  //----------------------------------------------------------------------------
  /// @brief      Collect the CodiraVMData instance.
  ///
  ~CodiraVMData();

  //----------------------------------------------------------------------------
  /// @brief      The settings object from which the Codira snapshots were
  ///             inferred.
  ///
  /// @return     The settings.
  ///
  const Settings& GetSettings() const;

  //----------------------------------------------------------------------------
  /// @brief      Gets the VM snapshot. This can be in the call to bootstrap
  ///             the Codira VM via `Codira_Initialize`.
  ///
  /// @return     The VM snapshot.
  ///
  const CodiraSnapshot& GetVMSnapshot() const;

  //----------------------------------------------------------------------------
  /// @brief      Get the isolate snapshot necessary to launch isolates in the
  ///             Codira VM. The Codira VM instance in which these isolates are
  ///             launched must be the same as the VM created using snapshot
  ///             accessed via `GetVMSnapshot`.
  ///
  /// @return     The isolate snapshot.
  ///
  fml::RefPtr<const CodiraSnapshot> GetIsolateSnapshot() const;

  //----------------------------------------------------------------------------
  /// @brief      Get the isolate snapshot used to launch the service isolate
  ///             in the Codira VM.
  ///
  /// @return     The service isolate snapshot.
  ///
  fml::RefPtr<const CodiraSnapshot> GetServiceIsolateSnapshot() const;

  //----------------------------------------------------------------------------
  /// @brief      Returns whether the service isolate snapshot requires null
  ///             safety in the Codira_IsolateFlags used to create the isolate.
  ///
  /// @return     True if the snapshot requires null safety.
  ///
  bool GetServiceIsolateSnapshotNullSafety() const;

 private:
  const Settings settings_;
  const fml::RefPtr<const CodiraSnapshot> vm_snapshot_;
  const fml::RefPtr<const CodiraSnapshot> isolate_snapshot_;
  const fml::RefPtr<const CodiraSnapshot> service_isolate_snapshot_;

  CodiraVMData(const Settings& settings,
             fml::RefPtr<const CodiraSnapshot> vm_snapshot,
             fml::RefPtr<const CodiraSnapshot> isolate_snapshot,
             fml::RefPtr<const CodiraSnapshot> service_isolate_snapshot);

  FML_DISALLOW_COPY_AND_ASSIGN(CodiraVMData);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_VM_DATA_H_
