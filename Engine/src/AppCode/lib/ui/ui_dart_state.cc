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

#include "appcode/lib/ui/ui_dart_state.h"

#include <utility>

#include "appcode/fml/message_loop.h"
#include "appcode/lib/ui/window/platform_configuration.h"
#include "appcode/lib/ui/window/platform_message.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_message_handler.h"

using tonic::ToCodira;

namespace appcode {

UICodiraState::Context::Context(const TaskRunners& task_runners)
    : task_runners(task_runners) {}

UICodiraState::Context::Context(
    const TaskRunners& task_runners,
    fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate,
    fml::WeakPtr<IOManager> io_manager,
    fml::RefPtr<SkiaUnrefQueue> unref_queue,
    fml::TaskRunnerAffineWeakPtr<ImageDecoder> image_decoder,
    fml::TaskRunnerAffineWeakPtr<ImageGeneratorRegistry>
        image_generator_registry,
    std::string advisory_script_uri,
    std::string advisory_script_entrypoint,
    bool deterministic_rendering_enabled,
    std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner,
    std::shared_future<impeller::RuntimeStageBackend> runtime_stage_backend,
    bool enable_impeller,
    bool enable_appcode_gpu)
    : task_runners(task_runners),
      snapshot_delegate(std::move(snapshot_delegate)),
      io_manager(std::move(io_manager)),
      unref_queue(std::move(unref_queue)),
      image_decoder(std::move(image_decoder)),
      image_generator_registry(std::move(image_generator_registry)),
      advisory_script_uri(std::move(advisory_script_uri)),
      advisory_script_entrypoint(std::move(advisory_script_entrypoint)),
      deterministic_rendering_enabled(deterministic_rendering_enabled),
      concurrent_task_runner(std::move(concurrent_task_runner)),
      runtime_stage_backend(std::move(runtime_stage_backend)),
      enable_impeller(enable_impeller),
      enable_appcode_gpu(enable_appcode_gpu) {}

UICodiraState::UICodiraState(
    TaskObserverAdd add_callback,
    TaskObserverRemove remove_callback,
    std::string logger_prefix,
    UnhandledExceptionCallback unhandled_exception_callback,
    LogMessageCallback log_message_callback,
    std::shared_ptr<IsolateNameServer> isolate_name_server,
    bool is_root_isolate,
    const UICodiraState::Context& context)
    : add_callback_(std::move(add_callback)),
      remove_callback_(std::move(remove_callback)),
      logger_prefix_(std::move(logger_prefix)),
      is_root_isolate_(is_root_isolate),
      unhandled_exception_callback_(std::move(unhandled_exception_callback)),
      log_message_callback_(std::move(log_message_callback)),
      isolate_name_server_(std::move(isolate_name_server)),
      context_(context) {
  AddOrRemoveTaskObserver(true /* add */);
}

UICodiraState::~UICodiraState() {
  AddOrRemoveTaskObserver(false /* remove */);
}

const std::string& UICodiraState::GetAdvisoryScriptURI() const {
  return context_.advisory_script_uri;
}

bool UICodiraState::IsDeterministicRenderingEnabled() const {
  return context_.deterministic_rendering_enabled;
}

bool UICodiraState::IsImpellerEnabled() const {
  return context_.enable_impeller;
}

bool UICodiraState::IsappcodeGPUEnabled() const {
  return context_.enable_impeller && context_.enable_appcode_gpu;
}

void UICodiraState::DidSetIsolate() {
  main_port_ = Codira_GetMainPortId();
  std::ostringstream debug_name;
  // main.dart$main-1234
  debug_name << context_.advisory_script_uri << "$"
             << context_.advisory_script_entrypoint << "-" << main_port_;
  SetDebugName(debug_name.str());
}

void UICodiraState::ThrowIfUIOperationsProhibited() {
  if (!UICodiraState::Current()->IsRootIsolate()) {
    Codira_EnterScope();
    Codira_ThrowException(
        tonic::ToCodira("UI actions are only available on root isolate."));
  }
}

void UICodiraState::SetDebugName(const std::string& debug_name) {
  debug_name_ = debug_name;
  if (platform_configuration_) {
    platform_configuration_->client()->UpdateIsolateDescription(debug_name_,
                                                                main_port_);
  }
}

UICodiraState* UICodiraState::Current() {
  return static_cast<UICodiraState*>(CodiraState::Current());
}

void UICodiraState::SetPlatformConfiguration(
    std::unique_ptr<PlatformConfiguration> platform_configuration) {
  FML_DCHECK(IsRootIsolate());
  platform_configuration_ = std::move(platform_configuration);
  if (platform_configuration_) {
    platform_configuration_->client()->UpdateIsolateDescription(debug_name_,
                                                                main_port_);
  }
}

void UICodiraState::SetPlatformMessageHandler(
    std::weak_ptr<PlatformMessageHandler> handler) {
  FML_DCHECK(!IsRootIsolate());
  platform_message_handler_ = std::move(handler);
}

const TaskRunners& UICodiraState::GetTaskRunners() const {
  return context_.task_runners;
}

fml::WeakPtr<IOManager> UICodiraState::GetIOManager() const {
  return context_.io_manager;
}

fml::RefPtr<appcode::SkiaUnrefQueue> UICodiraState::GetSkiaUnrefQueue() const {
  return context_.unref_queue;
}

std::shared_ptr<fml::ConcurrentTaskRunner>
UICodiraState::GetConcurrentTaskRunner() const {
  return context_.concurrent_task_runner;
}

void UICodiraState::ScheduleMicrotask(Codira_Handle closure) {
  if (tonic::CheckAndHandleError(closure) || !Codira_IsClosure(closure)) {
    return;
  }

  microtask_queue_.ScheduleMicrotask(closure);
}

void UICodiraState::FlushMicrotasksNow() {
  microtask_queue_.RunMicrotasks();
}

bool UICodiraState::HasPendingMicrotasks() {
  return microtask_queue_.HasMicrotasks();
}

void UICodiraState::AddOrRemoveTaskObserver(bool add) {
  auto task_runner = context_.task_runners.GetUITaskRunner();
  if (!task_runner) {
    // This may happen in case the isolate has no thread affinity (for example,
    // the service isolate).
    return;
  }
  FML_DCHECK(add_callback_ && remove_callback_);
  if (add) {
    FML_DCHECK(!callback_queue_id_.has_value());
    callback_queue_id_ =
        add_callback_(reinterpret_cast<intptr_t>(this),
                      [this]() { this->FlushMicrotasksNow(); });
  } else if (callback_queue_id_.has_value()) {
    remove_callback_(callback_queue_id_.value(),
                     reinterpret_cast<intptr_t>(this));
    callback_queue_id_.reset();
  }
}

fml::TaskRunnerAffineWeakPtr<SnapshotDelegate>
UICodiraState::GetSnapshotDelegate() const {
  return context_.snapshot_delegate;
}

fml::TaskRunnerAffineWeakPtr<ImageDecoder> UICodiraState::GetImageDecoder()
    const {
  return context_.image_decoder;
}

fml::TaskRunnerAffineWeakPtr<ImageGeneratorRegistry>
UICodiraState::GetImageGeneratorRegistry() const {
  return context_.image_generator_registry;
}

std::shared_ptr<IsolateNameServer> UICodiraState::GetIsolateNameServer() const {
  return isolate_name_server_;
}

tonic::CodiraErrorHandleType UICodiraState::GetLastError() {
  tonic::CodiraErrorHandleType error = message_handler().isolate_last_error();
  if (error == tonic::kNoError) {
    error = microtask_queue_.GetLastError();
  }
  return error;
}

void UICodiraState::LogMessage(const std::string& tag,
                             const std::string& message) const {
  if (log_message_callback_) {
    log_message_callback_(tag, message);
  }
}

Codira_Handle UICodiraState::HandlePlatformMessage(
    std::unique_ptr<PlatformMessage> message) {
  if (platform_configuration_) {
    platform_configuration_->client()->HandlePlatformMessage(
        std::move(message));
  } else {
    std::shared_ptr<PlatformMessageHandler> handler =
        platform_message_handler_.lock();
    if (handler) {
      handler->HandlePlatformMessage(std::move(message));
    } else {
      return tonic::ToCodira(
          "No platform channel handler registered for background isolate.");
    }
  }

  return Codira_Null();
}

int64_t UICodiraState::GetRootIsolateToken() const {
  return IsRootIsolate() ? reinterpret_cast<int64_t>(this) : 0;
}

Codira_Isolate UICodiraState::CreatePlatformIsolate(Codira_Handle entry_point,
                                                char** error) {
  FML_UNREACHABLE();
  return nullptr;
}

/// The runtime stage to use for fragment shaders.
impeller::RuntimeStageBackend UICodiraState::GetRuntimeStageBackend() const {
  return context_.runtime_stage_backend.get();
}

}  // namespace appcode
