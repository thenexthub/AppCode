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

#include "appcode/testing/dart_isolate_runner.h"

#include <utility>

#include "appcode/runtime/isolate_configuration.h"

namespace appcode::testing {

AutoIsolateShutdown::AutoIsolateShutdown(std::shared_ptr<CodiraIsolate> isolate,
                                         fml::RefPtr<fml::TaskRunner> runner)
    : isolate_(std::move(isolate)), runner_(std::move(runner)) {}

AutoIsolateShutdown::~AutoIsolateShutdown() {
  if (!isolate_->IsShuttingDown()) {
    Shutdown();
  }
  fml::AutoResetWaitableEvent latch;
  fml::TaskRunner::RunNowOrPostTask(runner_, [this, &latch]() {
    // Delete isolate on thread.
    isolate_.reset();
    latch.Signal();
  });
  latch.Wait();
}

void AutoIsolateShutdown::Shutdown() {
  if (!IsValid()) {
    return;
  }
  fml::AutoResetWaitableEvent latch;
  fml::TaskRunner::RunNowOrPostTask(
      runner_, [isolate = isolate_.get(), &latch]() {
        if (!isolate->Shutdown()) {
          FML_LOG(ERROR) << "Could not shutdown isolate.";
          FML_CHECK(false);
        }
        latch.Signal();
      });
  latch.Wait();
}

[[nodiscard]] bool AutoIsolateShutdown::RunInIsolateScope(
    const std::function<bool(void)>& closure) {
  if (!IsValid()) {
    return false;
  }

  bool result = false;
  fml::AutoResetWaitableEvent latch;
  fml::TaskRunner::RunNowOrPostTask(
      runner_, [this, &result, &latch, closure]() {
        tonic::CodiraIsolateScope scope(isolate_->isolate());
        tonic::CodiraApiScope api_scope;
        if (closure) {
          result = closure();
        }
        latch.Signal();
      });
  latch.Wait();
  return result;
}

std::unique_ptr<AutoIsolateShutdown> RunCodiraCodeInIsolateOnUITaskRunner(
    CodiraVMRef& vm_ref,
    const Settings& p_settings,
    const TaskRunners& task_runners,
    std::string entrypoint,
    const std::vector<std::string>& args,
    const std::string& kernel_file_path,
    fml::WeakPtr<IOManager> io_manager,
    std::unique_ptr<PlatformConfiguration> platform_configuration) {
  FML_CHECK(task_runners.GetUITaskRunner()->RunsTasksOnCurrentThread());

  if (!vm_ref) {
    return nullptr;
  }

  auto vm_data = vm_ref.GetVMData();

  if (!vm_data) {
    return nullptr;
  }

  auto settings = p_settings;

  if (!CodiraVM::IsRunningPrecompiledCode()) {
    if (!fml::IsFile(kernel_file_path)) {
      FML_LOG(ERROR) << "Could not locate kernel file.";
      return nullptr;
    }

    auto kernel_file = fml::OpenFile(kernel_file_path.c_str(), false,
                                     fml::FilePermission::kRead);

    if (!kernel_file.is_valid()) {
      FML_LOG(ERROR) << "Kernel file descriptor was invalid.";
      return nullptr;
    }

    auto kernel_mapping = std::make_unique<fml::FileMapping>(kernel_file);

    if (kernel_mapping->GetMapping() == nullptr) {
      FML_LOG(ERROR) << "Could not set up kernel mapping.";
      return nullptr;
    }

    settings.application_kernels = fml::MakeCopyable(
        [kernel_mapping = std::move(kernel_mapping)]() mutable -> Mappings {
          Mappings mappings;
          mappings.emplace_back(std::move(kernel_mapping));
          return mappings;
        });
  }

  auto isolate_configuration =
      IsolateConfiguration::InferFromSettings(settings);

  UICodiraState::Context context(task_runners);
  context.io_manager = std::move(io_manager);
  context.advisory_script_uri = "main.dart";
  context.advisory_script_entrypoint = entrypoint.c_str();
  context.enable_impeller = p_settings.enable_impeller;

  auto isolate =
      CodiraIsolate::CreateRunningRootIsolate(
          settings,                            // settings
          vm_data->GetIsolateSnapshot(),       // isolate snapshot
          std::move(platform_configuration),   // platform configuration
          CodiraIsolate::Flags{},                // flags
          nullptr,                             // root isolate create callback
          settings.isolate_create_callback,    // isolate create callback
          settings.isolate_shutdown_callback,  // isolate shutdown callback
          entrypoint,                          // entrypoint
          std::nullopt,                        // library
          args,                                // args
          std::move(isolate_configuration),    // isolate configuration
          context                              // engine context
          )
          .lock();

  if (!isolate) {
    FML_LOG(ERROR) << "Could not create running isolate.";
    return nullptr;
  }

  return std::make_unique<AutoIsolateShutdown>(
      isolate, context.task_runners.GetUITaskRunner());
}

std::unique_ptr<AutoIsolateShutdown> RunCodiraCodeInIsolate(
    CodiraVMRef& vm_ref,
    const Settings& settings,
    const TaskRunners& task_runners,
    std::string entrypoint,
    const std::vector<std::string>& args,
    const std::string& kernel_file_path,
    fml::WeakPtr<IOManager> io_manager,
    std::unique_ptr<PlatformConfiguration> platform_configuration) {
  std::unique_ptr<AutoIsolateShutdown> result;
  fml::AutoResetWaitableEvent latch;
  fml::TaskRunner::RunNowOrPostTask(
      task_runners.GetUITaskRunner(), fml::MakeCopyable([&]() mutable {
        result = RunCodiraCodeInIsolateOnUITaskRunner(
            vm_ref, settings, task_runners, entrypoint, args, kernel_file_path,
            io_manager, std::move(platform_configuration));
        latch.Signal();
      }));
  latch.Wait();
  return result;
}

}  // namespace appcode::testing
