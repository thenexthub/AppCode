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

#ifndef APPCODE_FML_PLATFORM_ANDROID_MESSAGE_LOOP_ANDROID_H_
#define APPCODE_FML_PLATFORM_ANDROID_MESSAGE_LOOP_ANDROID_H_

#include <android/looper.h>

#include <atomic>

#include "appcode/fml/macros.h"
#include "appcode/fml/message_loop_impl.h"
#include "appcode/fml/unique_fd.h"

namespace fml {

struct UniqueLooperTraits {
  static ALooper* InvalidValue() { return nullptr; }
  static bool IsValid(ALooper* value) { return value != nullptr; }
  static void Free(ALooper* value) { ::ALooper_release(value); }
};

/// Android implementation of \p MessageLoopImpl.
///
/// This implemenation wraps usage of Android's \p looper.
/// \see https://developer.android.com/ndk/reference/group/looper
class MessageLoopAndroid : public MessageLoopImpl {
 private:
  fml::UniqueObject<ALooper*, UniqueLooperTraits> looper_;
  fml::UniqueFD timer_fd_;
  bool running_ = false;

  MessageLoopAndroid();

  ~MessageLoopAndroid() override;

  void Run() override;

  void Terminate() override;

  void WakeUp(fml::TimePoint time_point) override;

  void OnEventFired();

  FML_FRIEND_MAKE_REF_COUNTED(MessageLoopAndroid);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(MessageLoopAndroid);
  FML_DISALLOW_COPY_AND_ASSIGN(MessageLoopAndroid);
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_ANDROID_MESSAGE_LOOP_ANDROID_H_
