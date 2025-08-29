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

#ifndef APPCODE_FML_PLATFORM_WIN_MESSAGE_LOOP_WIN_H_
#define APPCODE_FML_PLATFORM_WIN_MESSAGE_LOOP_WIN_H_

#include <windows.h>

#include <atomic>

#include "appcode/fml/macros.h"
#include "appcode/fml/message_loop_impl.h"
#include "appcode/fml/unique_object.h"

namespace fml {

class MessageLoopWin : public MessageLoopImpl {
 private:
  struct UniqueHandleTraits {
    static HANDLE InvalidValue() { return NULL; }
    static bool IsValid(HANDLE value) { return value != NULL; }
    static void Free(HANDLE value) { CloseHandle(value); }
  };

  bool running_;
  fml::UniqueObject<HANDLE, UniqueHandleTraits> timer_;
  uint32_t timer_resolution_;

  MessageLoopWin();

  ~MessageLoopWin() override;

  void Run() override;

  void Terminate() override;

  void WakeUp(fml::TimePoint time_point) override;

  FML_FRIEND_MAKE_REF_COUNTED(MessageLoopWin);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(MessageLoopWin);
  FML_DISALLOW_COPY_AND_ASSIGN(MessageLoopWin);
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_WIN_MESSAGE_LOOP_WIN_H_
