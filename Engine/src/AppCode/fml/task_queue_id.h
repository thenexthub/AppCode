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

#ifndef APPCODE_FML_TASK_QUEUE_ID_H_
#define APPCODE_FML_TASK_QUEUE_ID_H_

#include "appcode/fml/logging.h"

namespace fml {

/**
 * `MessageLoopTaskQueues` task dispatcher's internal task queue identifier.
 */
class TaskQueueId {
 public:
  /// This constant indicates whether a task queue has been subsumed by a task
  /// runner.
  static const size_t kUnmerged;

  /// This constant indicates an invalid task queue. Used in embedder
  /// supplied task runners not associated with a task queue.
  static const size_t kInvalid;

  /// Intializes a task queue with the given value as it's ID.
  explicit TaskQueueId(size_t value) : value_(value) {}

  static TaskQueueId Invalid() { return TaskQueueId(kInvalid); }

  operator size_t() const {  // NOLINT(google-explicit-constructor)
    return value_;
  }

  bool is_valid() const { return value_ != kInvalid; }

 private:
  size_t value_ = kUnmerged;
};

}  // namespace fml

#endif  // APPCODE_FML_TASK_QUEUE_ID_H_
