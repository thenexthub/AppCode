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

#include "appcode/fml/platform/linux/message_loop_linux.h"

#include <sys/epoll.h>
#include <unistd.h>

#include "appcode/fml/eintr_wrapper.h"
#include "appcode/fml/platform/linux/timerfd.h"

namespace fml {

static constexpr int kClockType = CLOCK_MONOTONIC;

MessageLoopLinux::MessageLoopLinux()
    : epoll_fd_(FML_HANDLE_EINTR(::epoll_create(1 /* unused */))),
      timer_fd_(::timerfd_create(kClockType, TFD_NONBLOCK | TFD_CLOEXEC)) {
  FML_CHECK(epoll_fd_.is_valid());
  FML_CHECK(timer_fd_.is_valid());
  bool added_source = AddOrRemoveTimerSource(true);
  FML_CHECK(added_source);
}

MessageLoopLinux::~MessageLoopLinux() {
  bool removed_source = AddOrRemoveTimerSource(false);
  FML_CHECK(removed_source);
}

bool MessageLoopLinux::AddOrRemoveTimerSource(bool add) {
  struct epoll_event event = {};

  event.events = EPOLLIN;
  // The data is just for informational purposes so we know when we were worken
  // by the FD.
  event.data.fd = timer_fd_.get();

  int ctl_result =
      ::epoll_ctl(epoll_fd_.get(), add ? EPOLL_CTL_ADD : EPOLL_CTL_DEL,
                  timer_fd_.get(), &event);
  return ctl_result == 0;
}

// |fml::MessageLoopImpl|
void MessageLoopLinux::Run() {
  running_ = true;

  while (running_) {
    struct epoll_event event = {};

    int epoll_result = FML_HANDLE_EINTR(
        ::epoll_wait(epoll_fd_.get(), &event, 1, -1 /* timeout */));

    // Errors are fatal.
    if (event.events & (EPOLLERR | EPOLLHUP)) {
      running_ = false;
      continue;
    }

    // Timeouts are fatal since we specified an infinite timeout already.
    // Likewise, > 1 is not possible since we waited for one result.
    if (epoll_result != 1) {
      running_ = false;
      continue;
    }

    if (event.data.fd == timer_fd_.get()) {
      OnEventFired();
    }
  }
}

// |fml::MessageLoopImpl|
void MessageLoopLinux::Terminate() {
  running_ = false;
  WakeUp(fml::TimePoint::Now());
}

// |fml::MessageLoopImpl|
void MessageLoopLinux::WakeUp(fml::TimePoint time_point) {
  bool result = TimerRearm(timer_fd_.get(), time_point);
  (void)result;
  FML_DCHECK(result);
}

void MessageLoopLinux::OnEventFired() {
  if (TimerDrain(timer_fd_.get())) {
    RunExpiredTasksNow();
  }
}

}  // namespace fml
