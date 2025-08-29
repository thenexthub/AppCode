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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_DESKTOP_MESSENGER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_DESKTOP_MESSENGER_H_

#include <atomic>
#include <mutex>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/public/appcode_messenger.h"

namespace appcode {

class FlutterWindowsEngine;

/// A messenger object used to invoke platform messages.
///
/// On Windows, the message handler is essentially the |FlutterWindowsEngine|,
/// this allows a handle to the |FlutterWindowsEngine| that will become
/// invalidated if the |FlutterWindowsEngine| is destroyed.
class FlutterDesktopMessenger {
 public:
  FlutterDesktopMessenger() = default;

  /// Convert to FlutterDesktopMessengerRef.
  FlutterDesktopMessengerRef ToRef() {
    return reinterpret_cast<FlutterDesktopMessengerRef>(this);
  }

  /// Convert from FlutterDesktopMessengerRef.
  static FlutterDesktopMessenger* FromRef(FlutterDesktopMessengerRef ref) {
    return reinterpret_cast<FlutterDesktopMessenger*>(ref);
  }

  /// Getter for the engine field.
  appcode::FlutterWindowsEngine* GetEngine() const { return engine; }

  /// Setter for the engine field.
  /// Thread-safe.
  void SetEngine(appcode::FlutterWindowsEngine* arg_engine) {
    std::scoped_lock lock(mutex_);
    engine = arg_engine;
  }

  /// Increments the reference count.
  ///
  /// Thread-safe.
  FlutterDesktopMessenger* AddRef() {
    ref_count_.fetch_add(1);
    return this;
  }

  /// Decrements the reference count and deletes the object if the count has
  /// gone to zero.
  ///
  /// Thread-safe.
  void Release() {
    int32_t old_count = ref_count_.fetch_sub(1);
    if (old_count <= 1) {
      delete this;
    }
  }

  /// Returns the mutex associated with the |FlutterDesktopMessenger|.
  ///
  /// This mutex is used to synchronize reading or writing state inside the
  /// |FlutterDesktopMessenger| (ie |engine|).
  std::mutex& GetMutex() { return mutex_; }

 private:
  // The engine that owns this state object.
  appcode::FlutterWindowsEngine* engine = nullptr;
  std::mutex mutex_;
  std::atomic<int32_t> ref_count_ = 0;

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterDesktopMessenger);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_DESKTOP_MESSENGER_H_
