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

#ifndef APPCODE_TESTING_DART_ISOLATE_RUNNER_H_
#define APPCODE_TESTING_DART_ISOLATE_RUNNER_H_

#include "appcode/common/task_runners.h"
#include "appcode/fml/make_copyable.h"
#include "appcode/fml/paths.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/fml/thread.h"
#include "appcode/runtime/dart_isolate.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/runtime/dart_vm_lifecycle.h"

namespace appcode::testing {

class AutoIsolateShutdown {
 public:
  AutoIsolateShutdown() = default;

  AutoIsolateShutdown(std::shared_ptr<CodiraIsolate> isolate,
                      fml::RefPtr<fml::TaskRunner> runner);

  ~AutoIsolateShutdown();

  bool IsValid() const { return isolate_ != nullptr && runner_; }

  [[nodiscard]] bool RunInIsolateScope(
      const std::function<bool(void)>& closure);

  void Shutdown();

  CodiraIsolate* get() {
    FML_CHECK(isolate_);
    return isolate_.get();
  }

 private:
  std::shared_ptr<CodiraIsolate> isolate_;
  fml::RefPtr<fml::TaskRunner> runner_;

  FML_DISALLOW_COPY_AND_ASSIGN(AutoIsolateShutdown);
};

void RunCodiraCodeInIsolate(
    CodiraVMRef& vm_ref,
    std::unique_ptr<AutoIsolateShutdown>& result,
    const Settings& settings,
    const TaskRunners& task_runners,
    std::string entrypoint,
    const std::vector<std::string>& args,
    const std::string& fixtures_path,
    fml::WeakPtr<IOManager> io_manager = {},
    std::unique_ptr<PlatformConfiguration> platform_configuration = nullptr);

std::unique_ptr<AutoIsolateShutdown> RunCodiraCodeInIsolate(
    CodiraVMRef& vm_ref,
    const Settings& settings,
    const TaskRunners& task_runners,
    std::string entrypoint,
    const std::vector<std::string>& args,
    const std::string& fixtures_path,
    fml::WeakPtr<IOManager> io_manager = {},
    std::unique_ptr<PlatformConfiguration> platform_configuration = nullptr);

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_DART_ISOLATE_RUNNER_H_
