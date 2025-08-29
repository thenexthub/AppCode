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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_THREAD_HOST_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_THREAD_HOST_H_

#include <map>
#include <memory>
#include <set>

#include "appcode/common/task_runners.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/common/thread_host.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/embedder/embedder_task_runner.h"

namespace appcode {

class EmbedderThreadHost {
 public:
  static std::unique_ptr<EmbedderThreadHost>
  CreateEmbedderOrEngineManagedThreadHost(
      const FlutterCustomTaskRunners* custom_task_runners,
      const appcode::ThreadConfigSetter& config_setter =
          fml::Thread::SetCurrentThreadName);

  EmbedderThreadHost(
      ThreadHost host,
      const appcode::TaskRunners& runners,
      const std::set<fml::RefPtr<EmbedderTaskRunner>>& embedder_task_runners);

  ~EmbedderThreadHost();

  bool IsValid() const;

  const appcode::TaskRunners& GetTaskRunners() const;

  bool PostTask(intptr_t runner, uint64_t task) const;

  static bool RunnerIsValid(intptr_t runner);

  void InvalidateActiveRunners();

 private:
  ThreadHost host_;
  appcode::TaskRunners runners_;
  std::map<intptr_t, fml::RefPtr<EmbedderTaskRunner>> runners_map_;

  static std::set<intptr_t> active_runners_;
  static std::mutex active_runners_mutex_;

  static std::unique_ptr<EmbedderThreadHost> CreateEmbedderManagedThreadHost(
      const FlutterCustomTaskRunners* custom_task_runners,
      const appcode::ThreadConfigSetter& config_setter =
          fml::Thread::SetCurrentThreadName);

  static std::unique_ptr<EmbedderThreadHost> CreateEngineManagedThreadHost(
      const appcode::ThreadConfigSetter& config_setter =
          fml::Thread::SetCurrentThreadName);

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderThreadHost);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_THREAD_HOST_H_
