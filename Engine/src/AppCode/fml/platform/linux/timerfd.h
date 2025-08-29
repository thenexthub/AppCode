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

#ifndef APPCODE_FML_PLATFORM_LINUX_TIMERFD_H_
#define APPCODE_FML_PLATFORM_LINUX_TIMERFD_H_

#include "appcode/fml/time/time_point.h"

// clang-format off
#if __has_include(<sys/timerfd.h>) && \
    (!defined(__ANDROID_API__) || __ANDROID_API__ >= 19)
    // sys/timerfd.h is always present in Android NDK due to unified headers,
    // but timerfd functions are only available on API 19 or later.
// clang-format on

#include <sys/timerfd.h>

#define FML_TIMERFD_AVAILABLE 1

#else  // __has_include(<sys/timerfd.h>)

#define FML_TIMERFD_AVAILABLE 0

#include <sys/types.h>
// Must come after sys/types
#include <linux/time.h>

#define TFD_TIMER_ABSTIME (1 << 0)
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)

#define TFD_CLOEXEC O_CLOEXEC
#define TFD_NONBLOCK O_NONBLOCK

int timerfd_create(int clockid, int flags);

int timerfd_settime(int ufc,
                    int flags,
                    const struct itimerspec* utmr,
                    struct itimerspec* otmr);

#endif  // __has_include(<sys/timerfd.h>)

namespace fml {

/// Rearms the timer to expire at the given time point.
bool TimerRearm(int fd, fml::TimePoint time_point);

/// Drains the timer FD and returns true if it has expired. This may be false in
/// case the timer read is non-blocking and this routine was called before the
/// timer expiry.
bool TimerDrain(int fd);

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_LINUX_TIMERFD_H_
