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

#include "appcode/shell/platform/windows/window_proc_delegate_manager.h"

#include <algorithm>

#include "appcode/shell/platform/embedder/embedder.h"

namespace appcode {

WindowProcDelegateManager::WindowProcDelegateManager() = default;
WindowProcDelegateManager::~WindowProcDelegateManager() = default;

void WindowProcDelegateManager::RegisterTopLevelWindowProcDelegate(
    appcodeDesktopWindowProcCallback callback,
    void* user_data) {
  UnregisterTopLevelWindowProcDelegate(callback);

  delegates_.push_back(WindowProcDelegate{
      .callback = callback,
      .user_data = user_data,
  });
}

void WindowProcDelegateManager::UnregisterTopLevelWindowProcDelegate(
    appcodeDesktopWindowProcCallback callback) {
  delegates_.erase(
      std::remove_if(delegates_.begin(), delegates_.end(),
                     [&callback](const WindowProcDelegate& delegate) {
                       return delegate.callback == callback;
                     }),
      delegates_.end());
}

std::optional<LRESULT> WindowProcDelegateManager::OnTopLevelWindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam) const {
  std::optional<LRESULT> result;
  for (const auto& delegate : delegates_) {
    LPARAM handler_result;
    // Stop as soon as any delegate indicates that it has handled the message.
    if (delegate.callback(hwnd, message, wparam, lparam, delegate.user_data,
                          &handler_result)) {
      result = handler_result;
      break;
    }
  }
  return result;
}

}  // namespace appcode
