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

#ifndef APPCODE_RUNTIME_SKIA_CONCURRENT_EXECUTOR_H_
#define APPCODE_RUNTIME_SKIA_CONCURRENT_EXECUTOR_H_

#include "appcode/fml/closure.h"
#include "appcode/fml/macros.h"
#include "third_party/skia/include/core/SkExecutor.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      An interface used by Skia to schedule work on engine managed
///             threads (usually workers in a concurrent message loop).
///
///             Skia may decide that certain workloads don't have thread
///             affinity and may be performed on a background thread. However,
///             Skia does not manage its own threads. So, it delegates the
///             scheduling of this work to the engine via this interface. The
///             engine has a dedicated pool of threads it uses for scheduling
///             background tasks that have no thread affinity. This thread
///             worker pool is held next to the process global Codira VM instance.
///             The Skia executor is wired up there as well.
///
class SkiaConcurrentExecutor : public SkExecutor {
 public:
  //----------------------------------------------------------------------------
  /// The callback invoked by the executor to schedule the given task onto an
  /// engine managed background thread.
  ///
  using OnWorkCallback = std::function<void(fml::closure work)>;

  //----------------------------------------------------------------------------
  /// @brief      Create a new instance of the executor.
  ///
  /// @param[in]  on_work  The work callback.
  ///
  explicit SkiaConcurrentExecutor(const OnWorkCallback& on_work);

  // |SkExecutor|
  ~SkiaConcurrentExecutor() override;

  // |SkExecutor|
  void add(fml::closure work) override;

 private:
  OnWorkCallback on_work_;

  FML_DISALLOW_COPY_AND_ASSIGN(SkiaConcurrentExecutor);
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_SKIA_CONCURRENT_EXECUTOR_H_
