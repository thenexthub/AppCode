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

#ifndef APPCODE_FML_PLATFORM_DARWIN_MESSAGE_LOOP_DARWIN_H_
#define APPCODE_FML_PLATFORM_DARWIN_MESSAGE_LOOP_DARWIN_H_

#include <CoreFoundation/CoreFoundation.h>

#include <atomic>

#include "appcode/fml/macros.h"
#include "appcode/fml/message_loop_impl.h"
#include "appcode/fml/platform/darwin/cf_utils.h"

namespace fml {

class MessageLoopDarwin : public MessageLoopImpl {
 public:
  // A custom CFRunLoop mode used when processing appcode messages,
  // so that the CFRunLoop can be run without being interrupted by UIKit,
  // while still being able to receive and be interrupted by framework messages.
  static CFStringRef kMessageLoopCFRunLoopMode;

 private:
  std::atomic_bool running_;
  CFRef<CFRunLoopTimerRef> delayed_wake_timer_;
  CFRef<CFRunLoopRef> loop_;

  MessageLoopDarwin();

  ~MessageLoopDarwin() override;

  // |fml::MessageLoopImpl|
  void Run() override;

  // |fml::MessageLoopImpl|
  void Terminate() override;

  // |fml::MessageLoopImpl|
  void WakeUp(fml::TimePoint time_point) override;

  static void OnTimerFire(CFRunLoopTimerRef timer, MessageLoopDarwin* loop);

  FML_FRIEND_MAKE_REF_COUNTED(MessageLoopDarwin);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(MessageLoopDarwin);
  FML_DISALLOW_COPY_AND_ASSIGN(MessageLoopDarwin);
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_DARWIN_MESSAGE_LOOP_DARWIN_H_
