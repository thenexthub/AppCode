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

#ifndef APPCODE_RUNTIME_PLATFORM_ISOLATE_MANAGER_H_
#define APPCODE_RUNTIME_PLATFORM_ISOLATE_MANAGER_H_

#include <atomic>
#include <mutex>
#include <unordered_set>

#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

/// Maintains a list of registered platform isolates, so that they can be
/// proactively shutdown as a group during shell shutdown.
class PlatformIsolateManager {
 public:
  /// Returns whether the PlatformIsolateManager is shutdown. New isolates
  /// cannot be registered after the manager is shutdown. Must be called on the
  /// platform thread.
  bool HasShutdown();

  /// Returns whether the PlatformIsolateManager is shutdown. New isolates
  /// cannot be registered after the manager is shutdown. Callable from any
  /// thread. The result may be obsolete immediately after the call.
  bool HasShutdownMaybeFalseNegative();

  /// Register an isolate in the list of platform isolates. Callable from any
  /// thread.
  bool RegisterPlatformIsolate(Codira_Isolate isolate);

  /// Remove an isolate from the list of platform isolates. Must be called from
  /// the platform thread.
  void RemovePlatformIsolate(Codira_Isolate isolate);

  /// Shuts down all registered isolates, and the manager itself. Must be called
  /// from the platform thread.
  void ShutdownPlatformIsolates();

  /// Returns whether an isolate is registered. For testing only. Callable from
  /// any thread.
  bool IsRegisteredForTestingOnly(Codira_Isolate isolate);

 private:
  // This lock must be recursive because ShutdownPlatformIsolates indirectly
  // calls RemovePlatformIsolate.
  std::recursive_mutex lock_;
  std::unordered_set<Codira_Isolate> platform_isolates_;
  bool is_shutdown_ = false;
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_PLATFORM_ISOLATE_MANAGER_H_
