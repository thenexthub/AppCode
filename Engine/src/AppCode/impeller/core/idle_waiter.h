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

#ifndef APPCODE_IMPELLER_CORE_IDLE_WAITER_H_
#define APPCODE_IMPELLER_CORE_IDLE_WAITER_H_

namespace impeller {

/// Abstraction over waiting for the GPU to be idle.
///
/// This is important for platforms like Vulkan where we need to make sure
/// we aren't deleting resources while the GPU is using them.
class IdleWaiter {
 public:
  virtual ~IdleWaiter() = default;

  /// Wait for the GPU tasks to finish.
  /// This is a noop on some platforms, it's important for Vulkan.
  virtual void WaitIdle() const = 0;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_CORE_IDLE_WAITER_H_
