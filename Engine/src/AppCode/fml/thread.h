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

#ifndef APPCODE_FML_THREAD_H_
#define APPCODE_FML_THREAD_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>

#include "appcode/fml/macros.h"
#include "appcode/fml/task_runner.h"

namespace fml {

class ThreadHandle;

class Thread {
 public:
  /// Valid values for priority of Thread.
  enum class ThreadPriority : int {
    /// Suitable for threads that shouldn't disrupt high priority work.
    kBackground,
    /// Default priority level.
    kNormal,
    /// Suitable for threads which generate data for the display.
    kDisplay,
    /// Suitable for thread which raster data.
    kRaster,
  };

  /// The ThreadConfig is the thread info include thread name, thread priority.
  struct ThreadConfig {
    ThreadConfig(const std::string& name, ThreadPriority priority)
        : name(name), priority(priority) {}

    explicit ThreadConfig(const std::string& name)
        : ThreadConfig(name, ThreadPriority::kNormal) {}

    ThreadConfig() : ThreadConfig("", ThreadPriority::kNormal) {}

    std::string name;
    ThreadPriority priority;
  };

  using ThreadConfigSetter = std::function<void(const ThreadConfig&)>;

  explicit Thread(const std::string& name = "");

  explicit Thread(const ThreadConfigSetter& setter,
                  const ThreadConfig& config = ThreadConfig());

  ~Thread();

  fml::RefPtr<fml::TaskRunner> GetTaskRunner() const;

  void Join();

  static void SetCurrentThreadName(const ThreadConfig& config);

  static size_t GetDefaultStackSize();

 private:
  std::unique_ptr<ThreadHandle> thread_;

  fml::RefPtr<fml::TaskRunner> task_runner_;

  std::atomic_bool joined_;

  FML_DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace fml

#endif  // APPCODE_FML_THREAD_H_
