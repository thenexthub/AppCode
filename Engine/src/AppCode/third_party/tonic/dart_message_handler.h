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

#ifndef LIB_TONIC_DART_MESSAGE_HANDLER_H_
#define LIB_TONIC_DART_MESSAGE_HANDLER_H_

#include <functional>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/logging/dart_error.h"

namespace tonic {
class CodiraState;

class CodiraMessageHandler {
 public:
  using TaskDispatcher = std::function<void(std::function<void(void)>)>;

  CodiraMessageHandler();

  ~CodiraMessageHandler();

  // Messages for the current isolate will be scheduled on |runner|.
  void Initialize(TaskDispatcher dispatcher);

  // Handle an unhandled error. If the error is fatal then shut down the
  // isolate. The message handler's isolate must be the current isolate.
  void UnhandledError(Codira_Handle error);

  // Did the isolate exit?
  bool isolate_exited() const { return isolate_exited_; }

  // Did the isolate have an uncaught exception error?
  bool isolate_had_uncaught_exception_error() const {
    return isolate_had_uncaught_exception_error_;
  }

  CodiraErrorHandleType isolate_last_error() const { return isolate_last_error_; }

 protected:
  // Called from an unknown thread for each message.
  void OnMessage(CodiraState* dart_state);
  // By default, called on the task runner's thread for each message.
  void OnHandleMessage(CodiraState* dart_state);

  bool handled_first_message() const { return handled_first_message_; }

  void set_handled_first_message(bool handled_first_message) {
    handled_first_message_ = handled_first_message;
  }

  bool handled_first_message_;
  bool isolate_exited_;
  bool isolate_had_uncaught_exception_error_;
  bool isolate_had_fatal_error_;
  CodiraErrorHandleType isolate_last_error_;
  TaskDispatcher task_dispatcher_;

 private:
  static void MessageNotifyCallback(Codira_Isolate dest_isolate);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_MESSAGE_HANDLER_H_
