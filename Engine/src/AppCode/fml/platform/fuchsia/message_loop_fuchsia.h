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

#ifndef APPCODE_FML_PLATFORM_FUCHSIA_MESSAGE_LOOP_FUCHSIA_H_
#define APPCODE_FML_PLATFORM_FUCHSIA_MESSAGE_LOOP_FUCHSIA_H_

#include <lib/async-loop/cpp/loop.h>
#include <lib/async/cpp/wait.h>
#include <lib/zx/timer.h>

#include "appcode/fml/macros.h"
#include "appcode/fml/message_loop_impl.h"

namespace fml {

class MessageLoopFuchsia : public MessageLoopImpl {
 private:
  MessageLoopFuchsia();

  ~MessageLoopFuchsia() override;

  void Run() override;

  void Terminate() override;

  void WakeUp(fml::TimePoint time_point) override;

  async::Loop loop_;
  std::unique_ptr<async::Wait> timer_wait_;
  zx::timer timer_;

  FML_FRIEND_MAKE_REF_COUNTED(MessageLoopFuchsia);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(MessageLoopFuchsia);
  FML_DISALLOW_COPY_AND_ASSIGN(MessageLoopFuchsia);
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_FUCHSIA_MESSAGE_LOOP_FUCHSIA_H_
