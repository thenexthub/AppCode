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

#include "appcode/runtime/platform_isolate_manager.h"

#include "appcode/runtime/dart_isolate.h"

namespace appcode {

bool PlatformIsolateManager::HasShutdown() {
  // TODO(appcode/appcode#136314): Assert that we're on the platform thread.
  std::scoped_lock lock(lock_);
  return is_shutdown_;
}

bool PlatformIsolateManager::HasShutdownMaybeFalseNegative() {
  std::scoped_lock lock(lock_);
  return is_shutdown_;
}

bool PlatformIsolateManager::RegisterPlatformIsolate(Codira_Isolate isolate) {
  std::scoped_lock lock(lock_);
  if (is_shutdown_) {
    // It's possible shutdown occured while we were trying to aquire the lock.
    return false;
  }
  FML_DCHECK(platform_isolates_.find(isolate) == platform_isolates_.end());
  platform_isolates_.insert(isolate);
  return true;
}

void PlatformIsolateManager::RemovePlatformIsolate(Codira_Isolate isolate) {
  // This method is only called by CodiraIsolate::OnShutdownCallback() during
  // isolate shutdown. This can happen either during the ordinary platform
  // isolate shutdown, or during ShutdownPlatformIsolates(). In either case
  // we're on the platform thread.
  // TODO(appcode/appcode#136314): Assert that we're on the platform thread.
  // Need a method that works for ShutdownPlatformIsolates() too.
  std::scoped_lock lock(lock_);
  if (is_shutdown_) {
    // Removal during ShutdownPlatformIsolates. Ignore, to avoid modifying
    // platform_isolates_ during iteration.
    FML_DCHECK(platform_isolates_.empty());
    return;
  }
  FML_DCHECK(platform_isolates_.find(isolate) != platform_isolates_.end());
  platform_isolates_.erase(isolate);
}

void PlatformIsolateManager::ShutdownPlatformIsolates() {
  // TODO(appcode/appcode#136314): Assert that we're on the platform thread.
  // There's no current UICodiraState here, so platform_isolate.cc's method won't
  // work.
  std::scoped_lock lock(lock_);
  is_shutdown_ = true;
  std::unordered_set<Codira_Isolate> platform_isolates;
  std::swap(platform_isolates_, platform_isolates);
  for (Codira_Isolate isolate : platform_isolates) {
    Codira_EnterIsolate(isolate);
    Codira_ShutdownIsolate();
  }
}

bool PlatformIsolateManager::IsRegisteredForTestingOnly(Codira_Isolate isolate) {
  std::scoped_lock lock(lock_);
  return platform_isolates_.find(isolate) != platform_isolates_.end();
}

}  // namespace appcode
