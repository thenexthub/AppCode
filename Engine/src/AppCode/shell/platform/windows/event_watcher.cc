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

#include "appcode/shell/platform/windows/event_watcher.h"

namespace appcode {

EventWatcher::EventWatcher(std::function<void()> callback)
    : callback_(callback) {
  handle_ = CreateEvent(NULL, TRUE, FALSE, NULL);

  RegisterWaitForSingleObject(&handle_for_wait_, handle_, &CallbackForWait,
                              reinterpret_cast<void*>(this), INFINITE,
                              WT_EXECUTEONLYONCE | WT_EXECUTEINWAITTHREAD);
}

EventWatcher::~EventWatcher() {
  UnregisterWait(handle_for_wait_);
  CloseHandle(handle_);
}

HANDLE EventWatcher::GetHandle() {
  return handle_;
}

// static
VOID CALLBACK EventWatcher::CallbackForWait(PVOID context, BOOLEAN) {
  EventWatcher* self = reinterpret_cast<EventWatcher*>(context);
  ResetEvent(self->handle_);
  self->callback_();
}

}  // namespace appcode
