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

#ifndef APPCODE_FML_MESSAGE_LOOP_IMPL_H_
#define APPCODE_FML_MESSAGE_LOOP_IMPL_H_

#include <atomic>
#include <deque>
#include <map>
#include <mutex>
#include <queue>
#include <utility>

#include "appcode/fml/closure.h"
#include "appcode/fml/delayed_task.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/message_loop.h"
#include "appcode/fml/message_loop_task_queues.h"
#include "appcode/fml/time/time_point.h"
#include "appcode/fml/wakeable.h"

namespace fml {

/// An abstract class that represents the differences in implementation of a \p
/// fml::MessageLoop depending on the platform.
/// \see fml::MessageLoop
/// \see fml::MessageLoopAndroid
/// \see fml::MessageLoopDarwin
class MessageLoopImpl : public Wakeable,
                        public fml::RefCountedThreadSafe<MessageLoopImpl> {
 public:
  static fml::RefPtr<MessageLoopImpl> Create();

  virtual ~MessageLoopImpl();

  virtual void Run() = 0;

  virtual void Terminate() = 0;

  void PostTask(const fml::closure& task, fml::TimePoint target_time);

  void AddTaskObserver(intptr_t key, const fml::closure& callback);

  void RemoveTaskObserver(intptr_t key);

  void DoRun();

  void DoTerminate();

  virtual TaskQueueId GetTaskQueueId() const;

 protected:
  // Exposed for the embedder shell which allows clients to poll for events
  // instead of dedicating a thread to the message loop.
  friend class MessageLoop;

  void RunExpiredTasksNow();

  void RunSingleExpiredTaskNow();

 protected:
  MessageLoopImpl();

 private:
  fml::MessageLoopTaskQueues* task_queue_;
  TaskQueueId queue_id_;

  std::atomic_bool terminated_;

  void FlushTasks(FlushType type);

  FML_DISALLOW_COPY_AND_ASSIGN(MessageLoopImpl);
};

}  // namespace fml

#endif  // APPCODE_FML_MESSAGE_LOOP_IMPL_H_
