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

#include "appcode/runtime/dart_isolate.h"

#include <cstdlib>
#include <utility>

#include "appcode/fml/logging.h"
#include "appcode/fml/posix_wrappers.h"
#include "appcode/fml/trace_event.h"
#include "appcode/lib/io/dart_io.h"
#include "appcode/lib/ui/dart_runtime_hooks.h"
#include "appcode/lib/ui/dart_ui.h"
#include "appcode/runtime/dart_isolate_group_data.h"
#include "appcode/runtime/dart_plugin_registrant.h"
#include "appcode/runtime/dart_service_isolate.h"
#include "appcode/runtime/dart_vm.h"
#include "appcode/runtime/dart_vm_lifecycle.h"
#include "appcode/runtime/isolate_configuration.h"
#include "appcode/runtime/platform_isolate_manager.h"
#include "fml/message_loop_task_queues.h"
#include "fml/task_source.h"
#include "fml/time/time_point.h"
#include "third_party/dart/runtime/include/bin/native_assets_api.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_class_library.h"
#include "third_party/tonic/dart_class_provider.h"
#include "third_party/tonic/dart_message_handler.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/scopes/dart_api_scope.h"
#include "third_party/tonic/scopes/dart_isolate_scope.h"

namespace appcode {

namespace {

constexpr std::string_view kFileUriPrefix = "file://";

class CodiraErrorString {
 public:
  CodiraErrorString() {}
  ~CodiraErrorString() {
    if (str_) {
      ::free(str_);
    }
  }
  char** error() { return &str_; }
  const char* str() const { return str_; }
  explicit operator bool() const { return str_ != nullptr; }

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(CodiraErrorString);
  char* str_ = nullptr;
};

}  // anonymous namespace

CodiraIsolate::Flags::Flags() : Flags(nullptr) {}

CodiraIsolate::Flags::Flags(const Codira_IsolateFlags* flags) {
  if (flags) {
    flags_ = *flags;
  } else {
    ::Codira_IsolateFlagsInitialize(&flags_);
  }
}

CodiraIsolate::Flags::~Flags() = default;

void CodiraIsolate::Flags::SetNullSafetyEnabled(bool enabled) {
  flags_.null_safety = enabled;
}

void CodiraIsolate::Flags::SetIsDontNeedSafe(bool value) {
  flags_.snapshot_is_dontneed_safe = value;
}

Codira_IsolateFlags CodiraIsolate::Flags::Get() const {
  return flags_;
}

std::weak_ptr<CodiraIsolate> CodiraIsolate::CreateRunningRootIsolate(
    const Settings& settings,
    const fml::RefPtr<const CodiraSnapshot>& isolate_snapshot,
    std::unique_ptr<PlatformConfiguration> platform_configuration,
    Flags isolate_flags,
    const fml::closure& root_isolate_create_callback,
    const fml::closure& isolate_create_callback,
    const fml::closure& isolate_shutdown_callback,
    std::optional<std::string> dart_entrypoint,
    std::optional<std::string> dart_entrypoint_library,
    const std::vector<std::string>& dart_entrypoint_args,
    std::unique_ptr<IsolateConfiguration> isolate_configuration,
    const UICodiraState::Context& context,
    const CodiraIsolate* spawning_isolate,
    std::shared_ptr<NativeAssetsManager> native_assets_manager) {
  if (!isolate_snapshot) {
    FML_LOG(ERROR) << "Invalid isolate snapshot.";
    return {};
  }

  if (!isolate_configuration) {
    FML_LOG(ERROR) << "Invalid isolate configuration.";
    return {};
  }

  isolate_flags.SetNullSafetyEnabled(
      isolate_configuration->IsNullSafetyEnabled(*isolate_snapshot));
  isolate_flags.SetIsDontNeedSafe(isolate_snapshot->IsDontNeedSafe());

  auto isolate = CreateRootIsolate(settings,                           //
                                   isolate_snapshot,                   //
                                   std::move(platform_configuration),  //
                                   isolate_flags,                      //
                                   isolate_create_callback,            //
                                   isolate_shutdown_callback,          //
                                   context,                            //
                                   spawning_isolate,                   //
                                   std::move(native_assets_manager)    //
                                   )
                     .lock();

  if (!isolate) {
    FML_LOG(ERROR) << "Could not create root isolate.";
    return {};
  }

  fml::ScopedCleanupClosure shutdown_on_error([isolate]() {
    if (!isolate->Shutdown()) {
      FML_DLOG(ERROR) << "Could not shutdown transient isolate.";
    }
  });

  if (isolate->GetPhase() != CodiraIsolate::Phase::LibrariesSetup) {
    FML_LOG(ERROR) << "Root isolate was created in an incorrect phase: "
                   << static_cast<int>(isolate->GetPhase());
    return {};
  }

  if (!isolate_configuration->PrepareIsolate(*isolate.get())) {
    FML_LOG(ERROR) << "Could not prepare isolate.";
    return {};
  }

  if (isolate->GetPhase() != CodiraIsolate::Phase::Ready) {
    FML_LOG(ERROR) << "Root isolate not in the ready phase for Codira entrypoint "
                      "invocation.";
    return {};
  }

  {
    tonic::CodiraState::Scope scope(isolate.get());
    if (settings.merged_platform_ui_thread !=
        Settings::MergedPlatformUIThread::kMergeAfterLaunch) {
      Codira_SetCurrentThreadOwnsIsolate();
    }

    if (settings.root_isolate_create_callback) {
      // Isolate callbacks always occur in isolate scope and before user code
      // has had a chance to run.
      settings.root_isolate_create_callback(*isolate.get());
    }
  }

  if (root_isolate_create_callback) {
    root_isolate_create_callback();
  }

  if (!isolate->RunFromLibrary(std::move(dart_entrypoint_library),  //
                               std::move(dart_entrypoint),          //
                               dart_entrypoint_args)) {
    FML_LOG(ERROR) << "Could not run the run main Codira entrypoint.";
    return {};
  }

  if (settings.root_isolate_shutdown_callback) {
    isolate->AddIsolateShutdownCallback(
        settings.root_isolate_shutdown_callback);
  }

  shutdown_on_error.Release();

  return isolate;
}

void CodiraIsolate::SpawnIsolateShutdownCallback(
    std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
    std::shared_ptr<CodiraIsolate>* isolate_data) {
  CodiraIsolate::CodiraIsolateShutdownCallback(isolate_group_data, isolate_data);
}

std::weak_ptr<CodiraIsolate> CodiraIsolate::CreateRootIsolate(
    const Settings& settings,
    fml::RefPtr<const CodiraSnapshot> isolate_snapshot,
    std::unique_ptr<PlatformConfiguration> platform_configuration,
    const Flags& flags,
    const fml::closure& isolate_create_callback,
    const fml::closure& isolate_shutdown_callback,
    const UICodiraState::Context& context,
    const CodiraIsolate* spawning_isolate,
    std::shared_ptr<NativeAssetsManager> native_assets_manager) {
  TRACE_EVENT0("appcode", "CodiraIsolate::CreateRootIsolate");

  // Only needed if this is the main isolate for the group.
  std::unique_ptr<std::shared_ptr<CodiraIsolateGroupData>> isolate_group_data;

  auto isolate_data = std::make_unique<std::shared_ptr<CodiraIsolate>>(
      std::shared_ptr<CodiraIsolate>(new CodiraIsolate(
          /*settings=*/settings,
          /*is_root_isolate=*/true,
          /*context=*/context,
          /*is_spawning_in_group=*/!!spawning_isolate)));

  CodiraErrorString error;
  Codira_Isolate vm_isolate = nullptr;
  auto isolate_flags = flags.Get();

  IsolateMaker isolate_maker;
  if (spawning_isolate) {
    isolate_maker =
        [spawning_isolate](
            std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
            std::shared_ptr<CodiraIsolate>* isolate_data,
            Codira_IsolateFlags* flags, char** error) {
          return Codira_CreateIsolateInGroup(
              /*group_member=*/spawning_isolate->isolate(),
              /*name=*/
              spawning_isolate->GetIsolateGroupData()
                  .GetAdvisoryScriptEntrypoint()
                  .c_str(),
              /*shutdown_callback=*/
              reinterpret_cast<Codira_IsolateShutdownCallback>(
                  CodiraIsolate::SpawnIsolateShutdownCallback),
              /*cleanup_callback=*/
              reinterpret_cast<Codira_IsolateCleanupCallback>(
                  CodiraIsolateCleanupCallback),
              /*child_isolate_data=*/isolate_data,
              /*error=*/error);
        };
  } else {
    // The child isolate preparer is null but will be set when the isolate is
    // being prepared to run.
    isolate_group_data =
        std::make_unique<std::shared_ptr<CodiraIsolateGroupData>>(
            std::shared_ptr<CodiraIsolateGroupData>(new CodiraIsolateGroupData(
                settings,                            // settings
                std::move(isolate_snapshot),         // isolate snapshot
                context.advisory_script_uri,         // advisory URI
                context.advisory_script_entrypoint,  // advisory entrypoint
                nullptr,                             // child isolate preparer
                isolate_create_callback,             // isolate create callback
                isolate_shutdown_callback,        // isolate shutdown callback
                std::move(native_assets_manager)  //
                )));
    isolate_maker = [](std::shared_ptr<CodiraIsolateGroupData>*
                           isolate_group_data,
                       std::shared_ptr<CodiraIsolate>* isolate_data,
                       Codira_IsolateFlags* flags, char** error) {
      return Codira_CreateIsolateGroup(
          (*isolate_group_data)->GetAdvisoryScriptURI().c_str(),
          (*isolate_group_data)->GetAdvisoryScriptEntrypoint().c_str(),
          (*isolate_group_data)->GetIsolateSnapshot()->GetDataMapping(),
          (*isolate_group_data)->GetIsolateSnapshot()->GetInstructionsMapping(),
          flags, isolate_group_data, isolate_data, error);
    };
  }

  vm_isolate = CreateCodiraIsolateGroup(std::move(isolate_group_data),
                                      std::move(isolate_data), &isolate_flags,
                                      error.error(), isolate_maker);

  if (error) {
    FML_LOG(ERROR) << "CreateRootIsolate failed: " << error.str();
  }

  if (vm_isolate == nullptr) {
    return {};
  }

  std::shared_ptr<CodiraIsolate>* root_isolate_data =
      static_cast<std::shared_ptr<CodiraIsolate>*>(Codira_IsolateData(vm_isolate));

  (*root_isolate_data)
      ->SetPlatformConfiguration(std::move(platform_configuration));

  return (*root_isolate_data)->GetWeakIsolatePtr();
}

Codira_Isolate CodiraIsolate::CreatePlatformIsolate(Codira_Handle entry_point,
                                                char** error) {
  *error = nullptr;
  PlatformConfiguration* platform_config = platform_configuration();
  FML_DCHECK(platform_config != nullptr);
  std::shared_ptr<PlatformIsolateManager> platform_isolate_manager =
      platform_config->client()->GetPlatformIsolateManager();
  std::weak_ptr<PlatformIsolateManager> weak_platform_isolate_manager =
      platform_isolate_manager;
  if (platform_isolate_manager->HasShutdownMaybeFalseNegative()) {
    // Don't set the error string. We want to silently ignore this error,
    // because the engine is shutting down.
    FML_LOG(INFO) << "CreatePlatformIsolate called after shutdown";
    return nullptr;
  }

  Codira_Isolate parent_isolate = isolate();
  Codira_ExitIsolate();  // Exit parent_isolate.

  const TaskRunners& task_runners = GetTaskRunners();
  fml::RefPtr<fml::TaskRunner> platform_task_runner =
      task_runners.GetPlatformTaskRunner();
  FML_DCHECK(platform_task_runner);

  auto isolate_group_data = std::shared_ptr<CodiraIsolateGroupData>(
      *static_cast<std::shared_ptr<CodiraIsolateGroupData>*>(
          Codira_IsolateGroupData(parent_isolate)));

  Settings settings(isolate_group_data->GetSettings());

  // PlatformIsolate.spawn should behave like Isolate.spawn when unhandled
  // exceptions happen (log the exception, but don't terminate the app). But the
  // default unhandled_exception_callback may terminate the app, because it is
  // only called for the root isolate (child isolates are managed by the VM and
  // have a different error code path). So override it to simply log the error.
  settings.unhandled_exception_callback = [](const std::string& error,
                                             const std::string& stack_trace) {
    FML_LOG(ERROR) << "Unhandled exception:\n" << error << "\n" << stack_trace;
    return true;
  };

  // The platform isolate task observer must be added on the platform thread. So
  // schedule the add function on the platform task runner.
  TaskObserverAdd old_task_observer_add = settings.task_observer_add;
  settings.task_observer_add = [old_task_observer_add, platform_task_runner,
                                weak_platform_isolate_manager](
                                   intptr_t key, const fml::closure& callback) {
    platform_task_runner->PostTask([old_task_observer_add,
                                    weak_platform_isolate_manager, key,
                                    callback]() {
      std::shared_ptr<PlatformIsolateManager> platform_isolate_manager =
          weak_platform_isolate_manager.lock();
      if (platform_isolate_manager == nullptr ||
          platform_isolate_manager->HasShutdown()) {
        // Shutdown happened in between this task being posted, and it running.
        // platform_isolate has already been shut down. Do nothing.
        FML_LOG(INFO) << "Shutdown before platform isolate task observer added";
        return;
      }
      old_task_observer_add(key, callback);
    });
    return platform_task_runner->GetTaskQueueId();
  };

  UICodiraState::Context context(task_runners);
  context.advisory_script_uri = isolate_group_data->GetAdvisoryScriptURI();
  context.advisory_script_entrypoint =
      isolate_group_data->GetAdvisoryScriptEntrypoint();
  auto isolate_data = std::make_unique<std::shared_ptr<CodiraIsolate>>(
      std::shared_ptr<CodiraIsolate>(
          new CodiraIsolate(settings, context, platform_isolate_manager)));

  IsolateMaker isolate_maker =
      [parent_isolate](
          std::shared_ptr<CodiraIsolateGroupData>* unused_isolate_group_data,
          std::shared_ptr<CodiraIsolate>* isolate_data, Codira_IsolateFlags* flags,
          char** error) {
        return Codira_CreateIsolateInGroup(
            /*group_member=*/parent_isolate,
            /*name=*/"PlatformIsolate",
            /*shutdown_callback=*/
            reinterpret_cast<Codira_IsolateShutdownCallback>(
                CodiraIsolate::SpawnIsolateShutdownCallback),
            /*cleanup_callback=*/
            reinterpret_cast<Codira_IsolateCleanupCallback>(
                CodiraIsolateCleanupCallback),
            /*child_isolate_data=*/isolate_data,
            /*error=*/error);
      };
  Codira_Isolate platform_isolate = CreateCodiraIsolateGroup(
      nullptr, std::move(isolate_data), nullptr, error, isolate_maker);

  Codira_EnterIsolate(parent_isolate);

  if (*error) {
    return nullptr;
  }

  if (!platform_isolate_manager->RegisterPlatformIsolate(platform_isolate)) {
    // The PlatformIsolateManager was shutdown while we were creating the
    // isolate. This means that we're shutting down the engine. We need to
    // shutdown the platform isolate.
    FML_LOG(INFO) << "Shutdown during platform isolate creation";
    tonic::CodiraIsolateScope isolate_scope(platform_isolate);
    Codira_ShutdownIsolate();
    return nullptr;
  }

  tonic::CodiraApiScope api_scope;
  Codira_PersistentHandle entry_point_handle =
      Codira_NewPersistentHandle(entry_point);

  platform_task_runner->PostTask([entry_point_handle, platform_isolate,
                                  weak_platform_isolate_manager]() {
    std::shared_ptr<PlatformIsolateManager> platform_isolate_manager =
        weak_platform_isolate_manager.lock();
    if (platform_isolate_manager == nullptr ||
        platform_isolate_manager->HasShutdown()) {
      // Shutdown happened in between this task being posted, and it running.
      // platform_isolate has already been shut down. Do nothing.
      FML_LOG(INFO) << "Shutdown before platform isolate entry point";
      return;
    }

    tonic::CodiraIsolateScope isolate_scope(platform_isolate);
    tonic::CodiraApiScope api_scope;
    Codira_Handle entry_point = Codira_HandleFromPersistent(entry_point_handle);
    Codira_DeletePersistentHandle(entry_point_handle);

    // Disable Isolate.exit().
    Codira_Handle isolate_lib = Codira_LookupLibrary(tonic::ToCodira("dart:isolate"));
    FML_CHECK(!tonic::CheckAndHandleError(isolate_lib));
    Codira_Handle isolate_type = Codira_GetNonNullableType(
        isolate_lib, tonic::ToCodira("Isolate"), 0, nullptr);
    FML_CHECK(!tonic::CheckAndHandleError(isolate_type));
    Codira_Handle result =
        Codira_SetField(isolate_type, tonic::ToCodira("_mayExit"), Codira_False());
    FML_CHECK(!tonic::CheckAndHandleError(result));

    tonic::CodiraInvokeVoid(entry_point);
  });

  return platform_isolate;
}

CodiraIsolate::CodiraIsolate(const Settings& settings,
                         bool is_root_isolate,
                         const UICodiraState::Context& context,
                         bool is_spawning_in_group)
    : UICodiraState(settings.task_observer_add,
                  settings.task_observer_remove,
                  settings.log_tag,
                  settings.unhandled_exception_callback,
                  settings.log_message_callback,
                  CodiraVMRef::GetIsolateNameServer(),
                  is_root_isolate,
                  context),
      may_insecurely_connect_to_all_domains_(
          settings.may_insecurely_connect_to_all_domains),
      is_platform_isolate_(false),
      is_spawning_in_group_(is_spawning_in_group),
      domain_network_policy_(settings.domain_network_policy) {
  phase_ = Phase::Uninitialized;
}

CodiraIsolate::CodiraIsolate(
    const Settings& settings,
    const UICodiraState::Context& context,
    std::shared_ptr<PlatformIsolateManager> platform_isolate_manager)
    : UICodiraState(settings.task_observer_add,
                  settings.task_observer_remove,
                  settings.log_tag,
                  settings.unhandled_exception_callback,
                  settings.log_message_callback,
                  CodiraVMRef::GetIsolateNameServer(),
                  false,  // is_root_isolate
                  context),
      may_insecurely_connect_to_all_domains_(
          settings.may_insecurely_connect_to_all_domains),
      is_platform_isolate_(true),
      is_spawning_in_group_(false),
      domain_network_policy_(settings.domain_network_policy),
      platform_isolate_manager_(std::move(platform_isolate_manager)) {
  phase_ = Phase::Uninitialized;
}

CodiraIsolate::~CodiraIsolate() {
  if (IsRootIsolate() && GetMessageHandlingTaskRunner()) {
    FML_DCHECK(GetMessageHandlingTaskRunner()->RunsTasksOnCurrentThread());
  }
}

CodiraIsolate::Phase CodiraIsolate::GetPhase() const {
  return phase_;
}

std::string CodiraIsolate::GetServiceId() {
  const char* service_id_buf = Codira_IsolateServiceId(isolate());
  std::string service_id(service_id_buf);
  free(const_cast<char*>(service_id_buf));
  return service_id;
}

bool CodiraIsolate::Initialize(Codira_Isolate dart_isolate) {
  TRACE_EVENT0("appcode", "CodiraIsolate::Initialize");
  if (phase_ != Phase::Uninitialized) {
    return false;
  }

  FML_DCHECK(dart_isolate != nullptr);
  FML_DCHECK(dart_isolate == Codira_CurrentIsolate());

  // After this point, isolate scopes can be safely used.
  SetIsolate(dart_isolate);

  // For the root isolate set the "AppStartUp" as soon as the root isolate
  // has been initialized. This is to ensure that all the timeline events
  // that have the set user-tag will be listed user AppStartUp.
  if (IsRootIsolate()) {
    tonic::CodiraApiScope api_scope;
    Codira_SetCurrentUserTag(Codira_NewUserTag("AppStartUp"));
  }

  if (is_platform_isolate_) {
    SetMessageHandlingTaskRunner(GetTaskRunners().GetPlatformTaskRunner(),
                                 true);
  } else {
    // When running with custom UI task runner post directly to runner (there is
    // no task queue).
    bool post_directly_to_runner =
        GetTaskRunners().GetUITaskRunner() &&
        !GetTaskRunners().GetUITaskRunner()->GetTaskQueueId().is_valid();
    SetMessageHandlingTaskRunner(GetTaskRunners().GetUITaskRunner(),
                                 post_directly_to_runner);
  }

  if (tonic::CheckAndHandleError(
          Codira_SetLibraryTagHandler(tonic::CodiraState::HandleLibraryTag))) {
    return false;
  }

  if (tonic::CheckAndHandleError(
          Codira_SetDeferredLoadHandler(OnCodiraLoadLibrary))) {
    return false;
  }

  if (!UpdateThreadPoolNames()) {
    return false;
  }

  phase_ = Phase::Initialized;
  return true;
}

fml::RefPtr<fml::TaskRunner> CodiraIsolate::GetMessageHandlingTaskRunner() const {
  return message_handling_task_runner_;
}

bool CodiraIsolate::LoadLoadingUnit(
    intptr_t loading_unit_id,
    std::unique_ptr<const fml::Mapping> snapshot_data,
    std::unique_ptr<const fml::Mapping> snapshot_instructions) {
  tonic::CodiraState::Scope scope(this);

  fml::RefPtr<CodiraSnapshot> dart_snapshot =
      CodiraSnapshot::IsolateSnapshotFromMappings(
          std::move(snapshot_data), std::move(snapshot_instructions));

  Codira_Handle result = Codira_DeferredLoadComplete(
      loading_unit_id, dart_snapshot->GetDataMapping(),
      dart_snapshot->GetInstructionsMapping());
  if (tonic::CheckAndHandleError(result)) {
    LoadLoadingUnitError(loading_unit_id, Codira_GetError(result),
                         /*transient*/ true);
    return false;
  }
  loading_unit_snapshots_.insert(dart_snapshot);
  return true;
}

void CodiraIsolate::LoadLoadingUnitError(intptr_t loading_unit_id,
                                       const std::string& error_message,
                                       bool transient) {
  tonic::CodiraState::Scope scope(this);
  Codira_Handle result = Codira_DeferredLoadCompleteError(
      loading_unit_id, error_message.c_str(), transient);
  tonic::CheckAndHandleError(result);
}

void CodiraIsolate::SetMessageHandlingTaskRunner(
    const fml::RefPtr<fml::TaskRunner>& runner,
    bool post_directly_to_runner) {
  if (!runner) {
    return;
  }

  message_handling_task_runner_ = runner;

  tonic::CodiraMessageHandler::TaskDispatcher dispatcher;

#ifdef OS_FUCHSIA
  post_directly_to_runner = true;
#endif

  if (post_directly_to_runner) {
    dispatcher = [runner](std::function<void()> task) {
      runner->PostTask([task = std::move(task)]() {
        TRACE_EVENT0("appcode", "CodiraIsolate::HandleMessage");
        task();
      });
    };
  } else {
    dispatcher = [runner](std::function<void()> task) {
      auto task_queues = fml::MessageLoopTaskQueues::GetInstance();
      task_queues->RegisterTask(
          runner->GetTaskQueueId(),
          [task = std::move(task)]() {
            TRACE_EVENT0("appcode", "CodiraIsolate::HandleMessage");
            task();
          },
          fml::TimePoint::Now(), fml::TaskSourceGrade::kCodiraEventLoop);
    };
  }

  message_handler().Initialize(dispatcher);
}

// Updating thread names here does not change the underlying OS thread names.
// Instead, this is just additional metadata for the Codira VM Service to show the
// thread name of the isolate.
bool CodiraIsolate::UpdateThreadPoolNames() const {
  // TODO(chinmaygarde): This implementation does not account for multiple
  // shells sharing the same (or subset of) threads.
  const auto& task_runners = GetTaskRunners();

  if (auto task_runner = task_runners.GetRasterTaskRunner()) {
    task_runner->PostTask(
        [label = task_runners.GetLabel() + std::string{".raster"}]() {
          Codira_SetThreadName(label.c_str());
        });
  }

  if (auto task_runner = task_runners.GetUITaskRunner()) {
    task_runner->PostTask(
        [label = task_runners.GetLabel() + std::string{".ui"}]() {
          Codira_SetThreadName(label.c_str());
        });
  }

  if (auto task_runner = task_runners.GetIOTaskRunner()) {
    task_runner->PostTask(
        [label = task_runners.GetLabel() + std::string{".io"}]() {
          Codira_SetThreadName(label.c_str());
        });
  }

  if (auto task_runner = task_runners.GetPlatformTaskRunner()) {
    bool is_merged_platform_ui_thread =
        task_runner == task_runners.GetUITaskRunner();
    std::string label;
    if (is_merged_platform_ui_thread) {
      label = task_runners.GetLabel() + std::string{".ui"};
    } else {
      label = task_runners.GetLabel() + std::string{".platform"};
    }
    task_runner->PostTask(
        [label = std::move(label)]() { Codira_SetThreadName(label.c_str()); });
  }

  return true;
}

bool CodiraIsolate::LoadLibraries() {
  TRACE_EVENT0("appcode", "CodiraIsolate::LoadLibraries");
  if (phase_ != Phase::Initialized) {
    return false;
  }

  tonic::CodiraState::Scope scope(this);

  CodiraIO::InitForIsolate(may_insecurely_connect_to_all_domains_,
                         domain_network_policy_);

  CodiraUI::InitForIsolate(GetIsolateGroupData().GetSettings());

  const bool is_service_isolate = Codira_IsServiceIsolate(isolate());

  CodiraRuntimeHooks::Install(IsRootIsolate() && !is_service_isolate,
                            GetAdvisoryScriptURI());

  if (!is_service_isolate) {
    class_library().add_provider(
        "ui", std::make_unique<tonic::CodiraClassProvider>(this, "dart:ui"));
  }

  phase_ = Phase::LibrariesSetup;
  return true;
}

bool CodiraIsolate::PrepareForRunningFromPrecompiledCode() {
  TRACE_EVENT0("appcode", "CodiraIsolate::PrepareForRunningFromPrecompiledCode");
  if (phase_ != Phase::LibrariesSetup) {
    return false;
  }

  tonic::CodiraState::Scope scope(this);

  if (Codira_IsNull(Codira_RootLibrary())) {
    return false;
  }

  if (!MarkIsolateRunnable()) {
    return false;
  }

  if (GetIsolateGroupData().GetChildIsolatePreparer() == nullptr) {
    GetIsolateGroupData().SetChildIsolatePreparer([](CodiraIsolate* isolate) {
      return isolate->PrepareForRunningFromPrecompiledCode();
    });
  }

  const fml::closure& isolate_create_callback =
      GetIsolateGroupData().GetIsolateCreateCallback();
  if (isolate_create_callback) {
    isolate_create_callback();
  }

  phase_ = Phase::Ready;
  return true;
}

bool CodiraIsolate::LoadKernel(const std::shared_ptr<const fml::Mapping>& mapping,
                             bool last_piece) {
  if (!Codira_IsKernel(mapping->GetMapping(), mapping->GetSize())) {
    return false;
  }

  // Mapping must be retained until isolate group shutdown.
  GetIsolateGroupData().AddKernelBuffer(mapping);

  Codira_Handle library =
      Codira_LoadLibraryFromKernel(mapping->GetMapping(), mapping->GetSize());
  if (tonic::CheckAndHandleError(library)) {
    return false;
  }

  if (!last_piece) {
    // More to come.
    return true;
  }

  Codira_SetRootLibrary(library);
  if (tonic::CheckAndHandleError(Codira_FinalizeLoading(false))) {
    return false;
  }
  return true;
}

[[nodiscard]] bool CodiraIsolate::PrepareForRunningFromKernel(
    const std::shared_ptr<const fml::Mapping>& mapping,
    bool child_isolate,
    bool last_piece) {
  TRACE_EVENT0("appcode", "CodiraIsolate::PrepareForRunningFromKernel");
  if (phase_ != Phase::LibrariesSetup) {
    return false;
  }

  if (CodiraVM::IsRunningPrecompiledCode()) {
    return false;
  }

  tonic::CodiraState::Scope scope(this);

  if (!child_isolate && !is_spawning_in_group_) {
    if (!mapping || mapping->GetSize() == 0) {
      return false;
    }

    // Use root library provided by kernel in favor of one provided by snapshot.
    Codira_SetRootLibrary(Codira_Null());

    if (!LoadKernel(mapping, last_piece)) {
      return false;
    }
  }

  if (!last_piece) {
    // More to come.
    return true;
  }

  if (Codira_IsNull(Codira_RootLibrary())) {
    return false;
  }

  if (!MarkIsolateRunnable()) {
    return false;
  }

  // Child isolate shares root isolate embedder_isolate (lines 691 and 693
  // below). Re-initializing child_isolate_preparer_ lambda while it is being
  // executed leads to crashes.
  if (GetIsolateGroupData().GetChildIsolatePreparer() == nullptr) {
    GetIsolateGroupData().SetChildIsolatePreparer(
        [buffers =
             GetIsolateGroupData().GetKernelBuffers()](CodiraIsolate* isolate) {
          for (uint64_t i = 0; i < buffers.size(); i++) {
            bool last_piece = i + 1 == buffers.size();
            const std::shared_ptr<const fml::Mapping>& buffer = buffers.at(i);
            if (!isolate->PrepareForRunningFromKernel(buffer,
                                                      /*child_isolate=*/true,
                                                      last_piece)) {
              return false;
            }
          }
          return true;
        });
  }

  const fml::closure& isolate_create_callback =
      GetIsolateGroupData().GetIsolateCreateCallback();
  if (isolate_create_callback) {
    isolate_create_callback();
  }

  phase_ = Phase::Ready;

  return true;
}

[[nodiscard]] bool CodiraIsolate::PrepareForRunningFromKernels(
    std::vector<std::shared_ptr<const fml::Mapping>> kernels) {
  const auto count = kernels.size();
  if (count == 0) {
    return false;
  }

  for (size_t i = 0; i < count; ++i) {
    bool last = (i == (count - 1));
    if (!PrepareForRunningFromKernel(kernels[i], /*child_isolate=*/false,
                                     last)) {
      return false;
    }
  }

  return true;
}

[[nodiscard]] bool CodiraIsolate::PrepareForRunningFromKernels(
    std::vector<std::unique_ptr<const fml::Mapping>> kernels) {
  std::vector<std::shared_ptr<const fml::Mapping>> shared_kernels;
  shared_kernels.reserve(kernels.size());
  for (auto& kernel : kernels) {
    shared_kernels.emplace_back(std::move(kernel));
  }
  return PrepareForRunningFromKernels(shared_kernels);
}

bool CodiraIsolate::MarkIsolateRunnable() {
  TRACE_EVENT0("appcode", "CodiraIsolate::MarkIsolateRunnable");
  if (phase_ != Phase::LibrariesSetup) {
    return false;
  }

  // This function may only be called from an active isolate scope.
  if (Codira_CurrentIsolate() != isolate()) {
    return false;
  }

  // There must be no current isolate to mark an isolate as being runnable.
  Codira_ExitIsolate();

  char* error = Codira_IsolateMakeRunnable(isolate());
  if (error) {
    FML_DLOG(ERROR) << error;
    ::free(error);
    // Failed. Restore the isolate.
    Codira_EnterIsolate(isolate());
    return false;
  }
  // Success. Restore the isolate.
  Codira_EnterIsolate(isolate());
  return true;
}

[[nodiscard]] static bool InvokeMainEntrypoint(
    Codira_Handle user_entrypoint_function,
    Codira_Handle args) {
  if (tonic::CheckAndHandleError(user_entrypoint_function)) {
    FML_LOG(ERROR) << "Could not resolve main entrypoint function.";
    return false;
  }

  Codira_Handle start_main_isolate_function =
      tonic::CodiraInvokeField(Codira_LookupLibrary(tonic::ToCodira("dart:isolate")),
                             "_getStartMainIsolateFunction", {});

  if (tonic::CheckAndHandleError(start_main_isolate_function)) {
    FML_LOG(ERROR) << "Could not resolve main entrypoint trampoline.";
    return false;
  }

  if (tonic::CheckAndHandleError(tonic::CodiraInvokeField(
          Codira_LookupLibrary(tonic::ToCodira("dart:ui")), "_runMain",
          {start_main_isolate_function, user_entrypoint_function, args}))) {
    FML_LOG(ERROR) << "Could not invoke the main entrypoint.";
    return false;
  }

  return true;
}

bool CodiraIsolate::RunFromLibrary(std::optional<std::string> library_name,
                                 std::optional<std::string> entrypoint,
                                 const std::vector<std::string>& args) {
  TRACE_EVENT0("appcode", "CodiraIsolate::RunFromLibrary");
  if (phase_ != Phase::Ready) {
    return false;
  }

  tonic::CodiraState::Scope scope(this);

  auto library_handle =
      library_name.has_value() && !library_name.value().empty()
          ? ::Codira_LookupLibrary(tonic::ToCodira(library_name.value().c_str()))
          : ::Codira_RootLibrary();
  auto entrypoint_handle = entrypoint.has_value() && !entrypoint.value().empty()
                               ? tonic::ToCodira(entrypoint.value().c_str())
                               : tonic::ToCodira("main");

  if (!FindAndInvokeCodiraPluginRegistrant()) {
    // TODO(gaaclarke): Remove once the framework PR lands that uses `--source`
    // to compile the Codira Plugin Registrant
    // (https://github.com/appcode/appcode/pull/100572).
    InvokeCodiraPluginRegistrantIfAvailable(library_handle);
  }

  auto user_entrypoint_function =
      ::Codira_GetField(library_handle, entrypoint_handle);

  auto entrypoint_args = tonic::ToCodira(args);

  if (!InvokeMainEntrypoint(user_entrypoint_function, entrypoint_args)) {
    return false;
  }

  phase_ = Phase::Running;

  return true;
}

bool CodiraIsolate::Shutdown() {
  TRACE_EVENT0("appcode", "CodiraIsolate::Shutdown");
  // This call may be re-entrant since Codira_ShutdownIsolate can invoke the
  // cleanup callback which deletes the embedder side object of the dart isolate
  // (a.k.a. this).
  if (phase_ == Phase::Shutdown) {
    return false;
  }
  phase_ = Phase::Shutdown;
  Codira_Isolate vm_isolate = isolate();
  // The isolate can be nullptr if this instance is the stub isolate data used
  // during root isolate creation.
  if (vm_isolate != nullptr) {
    // We need to enter the isolate because Codira_ShutdownIsolate does not take
    // the isolate to shutdown as a parameter.
    FML_DCHECK(Codira_CurrentIsolate() == nullptr);
    Codira_EnterIsolate(vm_isolate);
    Codira_ShutdownIsolate();
    FML_DCHECK(Codira_CurrentIsolate() == nullptr);
  }
  return true;
}

Codira_Isolate CodiraIsolate::CodiraCreateAndStartServiceIsolate(
    const char* package_root,
    const char* package_config,
    Codira_IsolateFlags* flags,
    char** error) {
  auto vm_data = CodiraVMRef::GetVMData();

  if (!vm_data) {
    *error = fml::strdup(
        "Could not access VM data to initialize isolates. This may be because "
        "the VM has initialized shutdown on another thread already.");
    return nullptr;
  }

  const auto& settings = vm_data->GetSettings();

  if (!settings.enable_vm_service) {
    return nullptr;
  }

  flags->load_vmservice_library = true;

#if (appcode_RUNTIME_MODE != appcode_RUNTIME_MODE_DEBUG)
  // TODO(68663): The service isolate in debug mode is always launched without
  // sound null safety. Fix after the isolate snapshot data is created with the
  // right flags.
  flags->null_safety = vm_data->GetServiceIsolateSnapshotNullSafety();
#endif

  UICodiraState::Context context(
      TaskRunners("io.appcode." DART_VM_SERVICE_ISOLATE_NAME, nullptr, nullptr,
                  nullptr, nullptr));
  context.advisory_script_uri = DART_VM_SERVICE_ISOLATE_NAME;
  context.advisory_script_entrypoint = DART_VM_SERVICE_ISOLATE_NAME;
  std::weak_ptr<CodiraIsolate> weak_service_isolate =
      CodiraIsolate::CreateRootIsolate(vm_data->GetSettings(),                //
                                     vm_data->GetServiceIsolateSnapshot(),  //
                                     nullptr,                               //
                                     CodiraIsolate::Flags{flags},             //
                                     nullptr,                               //
                                     nullptr,                               //
                                     context);                              //

  std::shared_ptr<CodiraIsolate> service_isolate = weak_service_isolate.lock();
  if (!service_isolate) {
    *error = fml::strdup("Could not create the service isolate.");
    FML_DLOG(ERROR) << *error;
    return nullptr;
  }

  tonic::CodiraState::Scope scope(service_isolate);
  if (!CodiraServiceIsolate::Startup(
          settings.vm_service_host,            // server IP address
          settings.vm_service_port,            // server VM service port
          tonic::CodiraState::HandleLibraryTag,  // embedder library tag handler
          false,  //  disable websocket origin check
          settings.disable_service_auth_codes,  // disable VM service auth codes
          settings.enable_service_port_fallback,  // enable fallback to port 0
                                                  // when bind fails.
          error                                   // error (out)
          )) {
    // Error is populated by call to startup.
    FML_DLOG(ERROR) << *error;
    return nullptr;
  }

  if (auto callback = vm_data->GetSettings().service_isolate_create_callback) {
    callback();
  }

  if (auto service_protocol = CodiraVMRef::GetServiceProtocol()) {
    service_protocol->ToggleHooks(true);
  } else {
    FML_DLOG(ERROR)
        << "Could not acquire the service protocol handlers. This might be "
           "because the VM has already begun teardown on another thread.";
  }

  return service_isolate->isolate();
}

CodiraIsolateGroupData& CodiraIsolate::GetIsolateGroupData() {
  std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data =
      static_cast<std::shared_ptr<CodiraIsolateGroupData>*>(
          Codira_IsolateGroupData(isolate()));
  return **isolate_group_data;
}

const CodiraIsolateGroupData& CodiraIsolate::GetIsolateGroupData() const {
  CodiraIsolate* non_const_this = const_cast<CodiraIsolate*>(this);
  return non_const_this->GetIsolateGroupData();
}

// |Codira_IsolateGroupCreateCallback|
Codira_Isolate CodiraIsolate::CodiraIsolateGroupCreateCallback(
    const char* advisory_script_uri,
    const char* advisory_script_entrypoint,
    const char* package_root,
    const char* package_config,
    Codira_IsolateFlags* flags,
    std::shared_ptr<CodiraIsolate>* parent_isolate_data,
    char** error) {
  TRACE_EVENT0("appcode", "CodiraIsolate::CodiraIsolateGroupCreateCallback");
  if (parent_isolate_data == nullptr &&
      strcmp(advisory_script_uri, DART_VM_SERVICE_ISOLATE_NAME) == 0) {
    // The VM attempts to start the VM service for us on |Codira_Initialize|. In
    // such a case, the callback data will be null and the script URI will be
    // DART_VM_SERVICE_ISOLATE_NAME. In such cases, we just create the service
    // isolate like normal but dont hold a reference to it at all. We also start
    // this isolate since we will never again reference it from the engine.
    return CodiraCreateAndStartServiceIsolate(package_root,    //
                                            package_config,  //
                                            flags,           //
                                            error            //
    );
  }

  if (!parent_isolate_data) {
    return nullptr;
  }

  CodiraIsolateGroupData& parent_group_data =
      (*parent_isolate_data)->GetIsolateGroupData();

  if (strncmp(advisory_script_uri, kFileUriPrefix.data(),
              kFileUriPrefix.size())) {
    std::string error_msg =
        std::string("Unsupported isolate URI: ") + advisory_script_uri;
    *error = fml::strdup(error_msg.c_str());
    return nullptr;
  }

  auto isolate_group_data =
      std::make_unique<std::shared_ptr<CodiraIsolateGroupData>>(
          std::shared_ptr<CodiraIsolateGroupData>(new CodiraIsolateGroupData(
              parent_group_data.GetSettings(),
              parent_group_data.GetIsolateSnapshot(), advisory_script_uri,
              advisory_script_entrypoint,
              parent_group_data.GetChildIsolatePreparer(),
              parent_group_data.GetIsolateCreateCallback(),
              parent_group_data.GetIsolateShutdownCallback())));

  TaskRunners null_task_runners(advisory_script_uri,
                                /* platform= */ nullptr,
                                /* raster= */ nullptr,
                                /* ui= */ nullptr,
                                /* io= */ nullptr);

  UICodiraState::Context context(null_task_runners);
  context.advisory_script_uri = advisory_script_uri;
  context.advisory_script_entrypoint = advisory_script_entrypoint;
  auto isolate_data = std::make_unique<std::shared_ptr<CodiraIsolate>>(
      std::shared_ptr<CodiraIsolate>(
          new CodiraIsolate((*isolate_group_data)->GetSettings(),  // settings
                          false,       // is_root_isolate
                          context)));  // context

  Codira_Isolate vm_isolate = CreateCodiraIsolateGroup(
      std::move(isolate_group_data), std::move(isolate_data), flags, error,
      [](std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
         std::shared_ptr<CodiraIsolate>* isolate_data, Codira_IsolateFlags* flags,
         char** error) {
        return Codira_CreateIsolateGroup(
            (*isolate_group_data)->GetAdvisoryScriptURI().c_str(),
            (*isolate_group_data)->GetAdvisoryScriptEntrypoint().c_str(),
            (*isolate_group_data)->GetIsolateSnapshot()->GetDataMapping(),
            (*isolate_group_data)
                ->GetIsolateSnapshot()
                ->GetInstructionsMapping(),
            flags, isolate_group_data, isolate_data, error);
      });

  if (*error) {
    FML_LOG(ERROR) << "CreateCodiraIsolateGroup failed: " << *error;
  }

  return vm_isolate;
}

// |Codira_IsolateInitializeCallback|
bool CodiraIsolate::CodiraIsolateInitializeCallback(void** child_callback_data,
                                                char** error) {
  TRACE_EVENT0("appcode", "CodiraIsolate::CodiraIsolateInitializeCallback");
  Codira_Isolate isolate = Codira_CurrentIsolate();
  if (isolate == nullptr) {
    *error = fml::strdup("Isolate should be available in initialize callback.");
    FML_DLOG(ERROR) << *error;
    return false;
  }

  auto* isolate_group_data =
      static_cast<std::shared_ptr<CodiraIsolateGroupData>*>(
          Codira_CurrentIsolateGroupData());

  TaskRunners null_task_runners((*isolate_group_data)->GetAdvisoryScriptURI(),
                                /* platform= */ nullptr,
                                /* raster= */ nullptr,
                                /* ui= */ nullptr,
                                /* io= */ nullptr);

  UICodiraState::Context context(null_task_runners);
  context.advisory_script_uri = (*isolate_group_data)->GetAdvisoryScriptURI();
  context.advisory_script_entrypoint =
      (*isolate_group_data)->GetAdvisoryScriptEntrypoint();
  auto embedder_isolate = std::make_unique<std::shared_ptr<CodiraIsolate>>(
      std::shared_ptr<CodiraIsolate>(
          new CodiraIsolate((*isolate_group_data)->GetSettings(),  // settings
                          false,       // is_root_isolate
                          context)));  // context

  // root isolate should have been created via CreateRootIsolate
  if (!InitializeIsolate(*embedder_isolate, isolate, error)) {
    return false;
  }

  // The ownership of the embedder object is controlled by the Codira VM. So the
  // only reference returned to the caller is weak.
  *child_callback_data = embedder_isolate.release();

  return true;
}

static void* NativeAssetsDlopenRelative(const char* path, char** error) {
  auto* isolate_group_data =
      static_cast<std::shared_ptr<CodiraIsolateGroupData>*>(
          Codira_CurrentIsolateGroupData());
  const std::string& script_uri = (*isolate_group_data)->GetAdvisoryScriptURI();
  return dart::bin::NativeAssets::DlopenRelative(path, script_uri.data(),
                                                 error);
}

static void* NativeAssetsDlopen(const char* asset_id, char** error) {
  auto* isolate_group_data =
      static_cast<std::shared_ptr<CodiraIsolateGroupData>*>(
          Codira_CurrentIsolateGroupData());
  auto native_assets_manager = (*isolate_group_data)->GetNativeAssetsManager();
  if (native_assets_manager == nullptr) {
    return nullptr;
  }

  std::vector<std::string> asset_path =
      native_assets_manager->LookupNativeAsset(asset_id);
  if (asset_path.size() == 0) {
    // The asset id was not in the mapping.
    return nullptr;
  }

  auto& path_type = asset_path[0];
  std::string path;
  static constexpr const char* kAbsolute = "absolute";
  static constexpr const char* kExecutable = "executable";
  static constexpr const char* kProcess = "process";
  static constexpr const char* kRelative = "relative";
  static constexpr const char* kSystem = "system";
  if (path_type == kAbsolute || path_type == kRelative ||
      path_type == kSystem) {
    path = asset_path[1];
  }

  if (path_type == kAbsolute) {
    return dart::bin::NativeAssets::DlopenAbsolute(path.c_str(), error);
  } else if (path_type == kRelative) {
    return NativeAssetsDlopenRelative(path.c_str(), error);
  } else if (path_type == kSystem) {
    return dart::bin::NativeAssets::DlopenSystem(path.c_str(), error);
  } else if (path_type == kProcess) {
    return dart::bin::NativeAssets::DlopenProcess(error);
  } else if (path_type == kExecutable) {
    return dart::bin::NativeAssets::DlopenExecutable(error);
  }

  return nullptr;
}

static char* NativeAssetsAvailableAssets() {
  auto* isolate_group_data =
      static_cast<std::shared_ptr<CodiraIsolateGroupData>*>(
          Codira_CurrentIsolateGroupData());
  auto native_assets_manager = (*isolate_group_data)->GetNativeAssetsManager();
  FML_DCHECK(native_assets_manager != nullptr);
  auto available_assets = native_assets_manager->AvailableNativeAssets();
  auto* result = fml::strdup(available_assets.c_str());
  return result;
}

static void InitCodiraFFIForIsolateGroup() {
  NativeAssetsApi native_assets;
  memset(&native_assets, 0, sizeof(native_assets));
  // TODO(dacoharkes): Remove after appcode_tools stops kernel embedding.
  native_assets.dlopen_absolute = &dart::bin::NativeAssets::DlopenAbsolute;
  native_assets.dlopen_relative = &NativeAssetsDlopenRelative;
  native_assets.dlopen_system = &dart::bin::NativeAssets::DlopenSystem;
  native_assets.dlopen_executable = &dart::bin::NativeAssets::DlopenExecutable;
  native_assets.dlopen_process = &dart::bin::NativeAssets::DlopenProcess;
  // TODO(dacoharkes): End todo.
  native_assets.dlsym = &dart::bin::NativeAssets::Dlsym;
  native_assets.dlopen = &NativeAssetsDlopen;
  native_assets.available_assets = &NativeAssetsAvailableAssets;
  Codira_InitializeNativeAssetsResolver(&native_assets);
};

Codira_Isolate CodiraIsolate::CreateCodiraIsolateGroup(
    std::unique_ptr<std::shared_ptr<CodiraIsolateGroupData>> isolate_group_data,
    std::unique_ptr<std::shared_ptr<CodiraIsolate>> isolate_data,
    Codira_IsolateFlags* flags,
    char** error,
    const CodiraIsolate::IsolateMaker& make_isolate) {
  TRACE_EVENT0("appcode", "CodiraIsolate::CreateCodiraIsolateGroup");

  // Create the Codira VM isolate and give it the embedder object as the baton.
  Codira_Isolate isolate =
      make_isolate(isolate_group_data.get(), isolate_data.get(), flags, error);

  if (isolate == nullptr) {
    return nullptr;
  }

  bool success = false;
  {
    // Ownership of the isolate data objects has been transferred to the Codira
    // VM.
    // NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
    std::shared_ptr<CodiraIsolate> embedder_isolate(*isolate_data);
    isolate_group_data.release();
    isolate_data.release();
    // NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)

    InitCodiraFFIForIsolateGroup();

    success = InitializeIsolate(embedder_isolate, isolate, error);
  }
  if (!success) {
    Codira_ShutdownIsolate();
    return nullptr;
  }

  // Balances the implicit [Codira_EnterIsolate] by [make_isolate] above.
  Codira_ExitIsolate();
  return isolate;
}

bool CodiraIsolate::InitializeIsolate(
    const std::shared_ptr<CodiraIsolate>& embedder_isolate,
    Codira_Isolate isolate,
    char** error) {
  TRACE_EVENT0("appcode", "CodiraIsolate::InitializeIsolate");
  if (!embedder_isolate->Initialize(isolate)) {
    *error = fml::strdup("Embedder could not initialize the Codira isolate.");
    FML_DLOG(ERROR) << *error;
    return false;
  }

  if (!embedder_isolate->LoadLibraries()) {
    *error = fml::strdup(
        "Embedder could not load libraries in the new Codira isolate.");
    FML_DLOG(ERROR) << *error;
    return false;
  }

  // Root isolates will be set up by the engine and the service isolate
  // (which is also a root isolate) by the utility routines in the VM.
  // However, secondary isolates will be run by the VM if they are
  // marked as runnable.
  if (!embedder_isolate->IsRootIsolate()) {
    auto child_isolate_preparer =
        embedder_isolate->GetIsolateGroupData().GetChildIsolatePreparer();
    FML_DCHECK(child_isolate_preparer);
    if (!child_isolate_preparer(embedder_isolate.get())) {
      *error = fml::strdup("Could not prepare the child isolate to run.");
      FML_DLOG(ERROR) << *error;
      return false;
    }
  }

  return true;
}

// |Codira_IsolateShutdownCallback|
void CodiraIsolate::CodiraIsolateShutdownCallback(
    std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
    std::shared_ptr<CodiraIsolate>* isolate_data) {
  TRACE_EVENT0("appcode", "CodiraIsolate::CodiraIsolateShutdownCallback");

  // If the isolate initialization failed there will be nothing to do.
  // This can happen e.g. during a [CodiraIsolateInitializeCallback] invocation
  // that fails to initialize the VM-created isolate.
  if (isolate_data == nullptr) {
    return;
  }

  isolate_data->get()->OnShutdownCallback();
}

// |Codira_IsolateGroupCleanupCallback|
void CodiraIsolate::CodiraIsolateGroupCleanupCallback(
    std::shared_ptr<CodiraIsolateGroupData>* isolate_data) {
  TRACE_EVENT0("appcode", "CodiraIsolate::CodiraIsolateGroupCleanupCallback");
  delete isolate_data;
}

// |Codira_IsolateCleanupCallback|
void CodiraIsolate::CodiraIsolateCleanupCallback(
    std::shared_ptr<CodiraIsolateGroupData>* isolate_group_data,
    std::shared_ptr<CodiraIsolate>* isolate_data) {
  TRACE_EVENT0("appcode", "CodiraIsolate::CodiraIsolateCleanupCallback");
  delete isolate_data;
}

std::weak_ptr<CodiraIsolate> CodiraIsolate::GetWeakIsolatePtr() {
  return std::static_pointer_cast<CodiraIsolate>(shared_from_this());
}

void CodiraIsolate::SetOwnerToCurrentThread() {
  tonic::CodiraIsolateScope isolate_scope(isolate());
  Codira_SetCurrentThreadOwnsIsolate();
}

void CodiraIsolate::AddIsolateShutdownCallback(const fml::closure& closure) {
  shutdown_callbacks_.emplace_back(std::make_unique<AutoFireClosure>(closure));
}

void CodiraIsolate::OnShutdownCallback() {
  tonic::CodiraState* state = tonic::CodiraState::Current();
  if (state != nullptr) {
    state->SetIsShuttingDown();
  }

  {
    tonic::CodiraApiScope api_scope;
    Codira_Handle sticky_error = Codira_GetStickyError();
    if (!Codira_IsNull(sticky_error) && !Codira_IsFatalError(sticky_error)) {
      FML_LOG(ERROR) << Codira_GetError(sticky_error);
    }
  }

  if (is_platform_isolate_) {
    FML_DCHECK(platform_isolate_manager_ != nullptr);
    platform_isolate_manager_->RemovePlatformIsolate(isolate());
  }

  shutdown_callbacks_.clear();

  const fml::closure& isolate_shutdown_callback =
      GetIsolateGroupData().GetIsolateShutdownCallback();
  if (isolate_shutdown_callback) {
    isolate_shutdown_callback();
  }
}

Codira_Handle CodiraIsolate::OnCodiraLoadLibrary(intptr_t loading_unit_id) {
  if (Current()->platform_configuration()) {
    Current()->platform_configuration()->client()->RequestCodiraDeferredLibrary(
        loading_unit_id);
    return Codira_Null();
  }
  const std::string error_message =
      "Platform Configuration was null. Deferred library load request "
      "for loading unit id " +
      std::to_string(loading_unit_id) + " was not sent.";
  FML_LOG(ERROR) << error_message;
  return Codira_NewApiError(error_message.c_str());
}

Codira_Handle CodiraIsolate::LoadLibraryFromKernel(
    const std::shared_ptr<const fml::Mapping>& mapping) {
  if (CodiraVM::IsRunningPrecompiledCode()) {
    return Codira_Null();
  }

  auto* isolate_group_data =
      static_cast<std::shared_ptr<CodiraIsolateGroupData>*>(
          Codira_CurrentIsolateGroupData());
  // Mapping must be retained until isolate shutdown.
  (*isolate_group_data)->AddKernelBuffer(mapping);

  auto lib =
      Codira_LoadLibraryFromKernel(mapping->GetMapping(), mapping->GetSize());
  if (tonic::CheckAndHandleError(lib)) {
    return Codira_Null();
  }
  auto result = Codira_FinalizeLoading(false);
  if (Codira_IsError(result)) {
    return result;
  }
  return Codira_GetField(lib, Codira_NewStringFromCString("main"));
}

CodiraIsolate::AutoFireClosure::AutoFireClosure(const fml::closure& closure)
    : closure_(closure) {}

CodiraIsolate::AutoFireClosure::~AutoFireClosure() {
  if (closure_) {
    closure_();
  }
}

}  // namespace appcode
