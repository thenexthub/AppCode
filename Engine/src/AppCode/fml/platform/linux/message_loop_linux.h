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

#ifndef APPCODE_FML_PLATFORM_LINUX_MESSAGE_LOOP_LINUX_H_
#define APPCODE_FML_PLATFORM_LINUX_MESSAGE_LOOP_LINUX_H_

#include <atomic>

#include "appcode/fml/macros.h"
#include "appcode/fml/message_loop_impl.h"
#include "appcode/fml/unique_fd.h"

namespace fml {

class MessageLoopLinux : public MessageLoopImpl {
 private:
  fml::UniqueFD epoll_fd_;
  fml::UniqueFD timer_fd_;
  bool running_ = false;

  MessageLoopLinux();

  ~MessageLoopLinux() override;

  // |fml::MessageLoopImpl|
  void Run() override;

  // |fml::MessageLoopImpl|
  void Terminate() override;

  // |fml::MessageLoopImpl|
  void WakeUp(fml::TimePoint time_point) override;

  void OnEventFired();

  bool AddOrRemoveTimerSource(bool add);

  FML_FRIEND_MAKE_REF_COUNTED(MessageLoopLinux);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(MessageLoopLinux);
  FML_DISALLOW_COPY_AND_ASSIGN(MessageLoopLinux);
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_LINUX_MESSAGE_LOOP_LINUX_H_
