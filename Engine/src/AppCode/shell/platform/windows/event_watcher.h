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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_EVENT_WATCHER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_EVENT_WATCHER_H_

#include <Windows.h>

#include <functional>

#include "appcode/fml/macros.h"

namespace appcode {

// A win32 `HANDLE` wrapper for use as a one-time callback.
class EventWatcher {
 public:
  explicit EventWatcher(std::function<void()> callback);
  ~EventWatcher();

  // Returns `HANDLE`, which can be used to register an event listener.
  HANDLE GetHandle();

 private:
  static VOID CALLBACK CallbackForWait(PVOID context, BOOLEAN);

  std::function<void()> callback_;

  HANDLE handle_;
  HANDLE handle_for_wait_;

  FML_DISALLOW_COPY_AND_ASSIGN(EventWatcher);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_EVENT_WATCHER_H_
