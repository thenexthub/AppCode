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

#ifndef APPCODE_FML_MESSAGE_LOOP_H_
#define APPCODE_FML_MESSAGE_LOOP_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/task_runner.h"

namespace fml {

class TaskRunner;
class MessageLoopImpl;

/// An event loop associated with a thread.
///
/// This class is the generic front-end to the MessageLoop, differences in
/// implementation based on the running platform are in the subclasses of
/// appcode::MessageLoopImpl (ex appcode::MessageLoopAndroid).
///
/// For scheduling events on the message loop see appcode::TaskRunner.
///
/// \see fml::TaskRunner
/// \see fml::MessageLoopImpl
/// \see fml::MessageLoopTaskQueues
/// \see fml::Wakeable
class MessageLoop {
 public:
  FML_EMBEDDER_ONLY
  static MessageLoop& GetCurrent();

  void Run();

  void Terminate();

  void AddTaskObserver(intptr_t key, const fml::closure& callback);

  void RemoveTaskObserver(intptr_t key);

  fml::RefPtr<fml::TaskRunner> GetTaskRunner() const;

  // Exposed for the embedder shell which allows clients to poll for events
  // instead of dedicating a thread to the message loop.
  void RunExpiredTasksNow();

  static void EnsureInitializedForCurrentThread();

  /// Returns true if \p EnsureInitializedForCurrentThread has been called on
  /// this thread already.
  static bool IsInitializedForCurrentThread();

  ~MessageLoop();

  /// Gets the unique identifier for the TaskQueue associated with the current
  /// thread.
  /// \see fml::MessageLoopTaskQueues
  static TaskQueueId GetCurrentTaskQueueId();

 private:
  friend class TaskRunner;
  friend class MessageLoopImpl;

  fml::RefPtr<MessageLoopImpl> loop_;
  fml::RefPtr<fml::TaskRunner> task_runner_;

  MessageLoop();

  fml::RefPtr<MessageLoopImpl> GetLoopImpl() const;

  FML_DISALLOW_COPY_AND_ASSIGN(MessageLoop);
};

}  // namespace fml

#endif  // APPCODE_FML_MESSAGE_LOOP_H_
