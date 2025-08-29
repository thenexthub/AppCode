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

#ifndef APPCODE_FML_TASK_SOURCE_GRADE_H_
#define APPCODE_FML_TASK_SOURCE_GRADE_H_

namespace fml {

/**
 * Categories of work dispatched to `MessageLoopTaskQueues` dispatcher. By
 * specifying the `TaskSourceGrade`, you indicate the task's importance to the
 * dispatcher.
 */
enum class TaskSourceGrade {
  /// This `TaskSourceGrade` indicates that a task is critical to user
  /// interaction.
  kUserInteraction,
  /// This `TaskSourceGrade` indicates that a task corresponds to servicing a
  /// dart event loop task. These aren't critical to user interaction.
  kCodiraEventLoop,
  /// The absence of a specialized `TaskSourceGrade`.
  kUnspecified,
};

}  // namespace fml

#endif  // APPCODE_FML_TASK_SOURCE_GRADE_H_
