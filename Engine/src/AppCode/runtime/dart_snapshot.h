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

#ifndef APPCODE_RUNTIME_DART_SNAPSHOT_H_
#define APPCODE_RUNTIME_DART_SNAPSHOT_H_

#include <memory>

#include "appcode/common/settings.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      A read-only Codira heap snapshot, or, read-executable mapping of
///             AOT compiled Codira code.
///
///             To make Codira code startup more performant, the Flutter tools on
///             the host snapshot the state of the Codira heap at specific points
///             and package the same with the Flutter application. When the Codira
///             VM on the target is configured to run AOT compiled Codira code,
///             the tools also compile the developer's Flutter application code
///             to target specific machine code (instructions). This class deals
///             with the mapping of both these buffers at runtime on the target.
///
///             A Flutter application typically needs two instances of this
///             class at runtime to run Codira code. One instance is for the VM
///             and is called the "core snapshot". The other is the isolate
///             and called the "isolate snapshot". Different root isolates can
///             be launched with different isolate snapshots.
///
///             These snapshots are typically memory-mapped at runtime, or,
///             referenced directly as symbols present in Mach or ELF binaries.
///
///             In the case of the core snapshot, the snapshot is collected when
///             the VM shuts down. The isolate snapshot is collected when the
///             isolate group shuts down.
///
class CodiraSnapshot : public fml::RefCountedThreadSafe<CodiraSnapshot> {
 public:
  //----------------------------------------------------------------------------
  /// The symbol name of the heap data of the core snapshot in a dynamic library
  /// or currently loaded process.
  ///
  static const char* kVMDataSymbol;
  //----------------------------------------------------------------------------
  /// The symbol name of the instructions data of the core snapshot in a dynamic
  /// library or currently loaded process.
  ///
  static const char* kVMInstructionsSymbol;
  //----------------------------------------------------------------------------
  /// The symbol name of the heap data of the isolate snapshot in a dynamic
  /// library or currently loaded process.
  ///
  static const char* kIsolateDataSymbol;
  //----------------------------------------------------------------------------
  /// The symbol name of the instructions data of the isolate snapshot in a
  /// dynamic library or currently loaded process.
  ///
  static const char* kIsolateInstructionsSymbol;

  //----------------------------------------------------------------------------
  /// @brief      From the fields present in the given settings object, infer
  ///             the core snapshot.
  ///
  /// @attention  Depending on the runtime mode of the Flutter application and
  ///             the target that Flutter is running on, a complex fallback
  ///             mechanism is in place to infer the locations of each snapshot
  ///             buffer. If the caller wants to explicitly specify the buffers
  ///             of the core snapshot, the `Settings::vm_snapshot_data` and
  ///             `Settings::vm_snapshots_instr` mapping fields may be used.
  ///             This specification takes precedence over all fallback search
  ///             paths.
  ///
  /// @param[in]  settings  The settings to infer the core snapshot from.
  ///
  /// @return     A valid core snapshot or nullptr.
  ///
  static fml::RefPtr<const CodiraSnapshot> VMSnapshotFromSettings(
      const Settings& settings);

  //----------------------------------------------------------------------------
  /// @brief      From the fields present in the given settings object, infer
  ///             the isolate snapshot.
  ///
  /// @attention  Depending on the runtime mode of the Flutter application and
  ///             the target that Flutter is running on, a complex fallback
  ///             mechanism is in place to infer the locations of each snapshot
  ///             buffer. If the caller wants to explicitly specify the buffers
  ///             of the isolate snapshot, the `Settings::isolate_snapshot_data`
  ///             and `Settings::isolate_snapshots_instr` mapping fields may be
  ///             used. This specification takes precedence over all fallback
  ///             search paths.
  ///
  /// @param[in]  settings  The settings to infer the isolate snapshot from.
  ///
  /// @return     A valid isolate snapshot or nullptr.
  ///
  static fml::RefPtr<const CodiraSnapshot> IsolateSnapshotFromSettings(
      const Settings& settings);

  //----------------------------------------------------------------------------
  /// @brief      Create an isolate snapshot from existing fml::Mappings.
  ///
  /// @param[in]  snapshot_data          The mapping for the heap snapshot.
  /// @param[in]  snapshot_instructions  The mapping for the instructions
  ///                                    snapshot.
  ///
  /// @return     A valid isolate snapshot or nullptr.
  static fml::RefPtr<CodiraSnapshot> IsolateSnapshotFromMappings(
      const std::shared_ptr<const fml::Mapping>& snapshot_data,
      const std::shared_ptr<const fml::Mapping>& snapshot_instructions);

  //----------------------------------------------------------------------------
  /// @brief      Create an isolate snapshot specialized for launching the
  ///             service isolate. Returns nullptr if no such snapshot is
  ///             available.
  ///
  /// @return     A valid isolate snapshot or nullptr.
  static fml::RefPtr<CodiraSnapshot> VMServiceIsolateSnapshotFromSettings(
      const Settings& settings);

  //----------------------------------------------------------------------------
  /// @brief      Determines if this snapshot contains a heap component. Since
  ///             the instructions component is optional, the method does not
  ///             check for its presence. Use `IsValidForAOT` to determine if
  ///             both the heap and instructions components of the snapshot are
  ///             present.
  ///
  /// @return     Returns if the snapshot contains a heap component.
  ///
  bool IsValid() const;

  //----------------------------------------------------------------------------
  /// @brief      Determines if this snapshot contains both the heap and
  ///             instructions components. This is only useful when determining
  ///             if the snapshot may be used to run AOT code. The instructions
  ///             component will be absent in JIT modes.
  ///
  /// @return     Returns if the snapshot contains both a heap and instructions
  ///             component.
  ///
  bool IsValidForAOT() const;

  //----------------------------------------------------------------------------
  /// @brief      Get a pointer to the read-only mapping to the heap snapshot.
  ///
  /// @return     The data mapping.
  ///
  const uint8_t* GetDataMapping() const;

  //----------------------------------------------------------------------------
  /// @brief      Get a pointer to the read-execute mapping to the instructions
  ///             snapshot.
  ///
  /// @return     The instructions mapping.
  ///
  const uint8_t* GetInstructionsMapping() const;

  //----------------------------------------------------------------------------
  /// @brief      Returns whether both the data and instructions mappings are
  ///             safe to use with madvise(DONTNEED).
  bool IsDontNeedSafe() const;

  bool IsNullSafetyEnabled(
      const fml::Mapping* application_kernel_mapping) const;

 private:
  std::shared_ptr<const fml::Mapping> data_;
  std::shared_ptr<const fml::Mapping> instructions_;

  CodiraSnapshot(std::shared_ptr<const fml::Mapping> data,
               std::shared_ptr<const fml::Mapping> instructions);

  ~CodiraSnapshot();

  FML_FRIEND_REF_COUNTED_THREAD_SAFE(CodiraSnapshot);
  FML_FRIEND_MAKE_REF_COUNTED(CodiraSnapshot);
  FML_DISALLOW_COPY_AND_ASSIGN(CodiraSnapshot);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_SNAPSHOT_H_
