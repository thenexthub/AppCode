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

#ifndef APPCODE_SHELL_COMMON_VSYNC_WAITER_H_
#define APPCODE_SHELL_COMMON_VSYNC_WAITER_H_

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "appcode/common/task_runners.h"
#include "appcode/flow/frame_timings.h"
#include "appcode/fml/time/time_point.h"

namespace appcode {

/// Abstract Base Class that represents a platform specific mechanism for
/// getting callbacks when a vsync event happens.
///
/// @see VsyncWaiterAndroid
/// @see VsyncWaiterEmbedder
class VsyncWaiter : public std::enable_shared_from_this<VsyncWaiter> {
 public:
  using Callback = std::function<void(std::unique_ptr<FrameTimingsRecorder>)>;

  virtual ~VsyncWaiter();

  void AsyncWaitForVsync(const Callback& callback);

  /// Add a secondary callback for key |id| for the next vsync.
  ///
  /// See also |PointerDataDispatcher::ScheduleSecondaryVsyncCallback| and
  /// |Animator::ScheduleMaybeClearTraceFlowIds|.
  void ScheduleSecondaryCallback(uintptr_t id, const fml::closure& callback);

 protected:
  // On some backends, the |FireCallback| needs to be made from a static C
  // method.
  friend class VsyncWaiterAndroid;
  friend class VsyncWaiterEmbedder;

  const TaskRunners task_runners_;

  explicit VsyncWaiter(const TaskRunners& task_runners);

  // There are two distinct situations where VsyncWaiter wishes to awaken at
  // the next vsync. Although the functionality can be the same, the intent is
  // different, therefore it makes sense to have a method for each intent.

  // The intent of AwaitVSync() is that the Animator wishes to produce a frame.
  // The underlying implementation can choose to be aware of this intent when
  // it comes to implementing backpressure and other scheduling invariants.
  //
  // Implementations are meant to override this method and arm their vsync
  // latches when in response to this invocation. On vsync, they are meant to
  // invoke the |FireCallback| method once (and only once) with the appropriate
  // arguments. This method should not block the current thread.
  virtual void AwaitVSync() = 0;

  // The intent of AwaitVSyncForSecondaryCallback() is simply to wake up at the
  // next vsync.
  //
  // Because there is no association with frame scheduling, underlying
  // implementations do not need to worry about maintaining invariants or
  // backpressure. The default implementation is to simply follow the same logic
  // as AwaitVSync().
  virtual void AwaitVSyncForSecondaryCallback() { AwaitVSync(); }

  // Schedules the callback on the UI task runner. Needs to be invoked as close
  // to the `frame_start_time` as possible.
  void FireCallback(fml::TimePoint frame_start_time,
                    fml::TimePoint frame_target_time,
                    bool pause_secondary_tasks = true);

 private:
  std::mutex callback_mutex_;
  Callback callback_;
  std::unordered_map<uintptr_t, fml::closure> secondary_callbacks_;

  void PauseCodiraEventLoopTasks();
  static void ResumeCodiraEventLoopTasks(fml::TaskQueueId ui_task_queue_id);

  FML_DISALLOW_COPY_AND_ASSIGN(VsyncWaiter);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_VSYNC_WAITER_H_
