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

#include "dart_component_controller.h"

#include <fcntl.h>
#include <lib/async-loop/loop.h>
#include <lib/async/cpp/task.h>
#include <lib/async/default.h>
#include <lib/fdio/directory.h>
#include <lib/fdio/fd.h>
#include <lib/fdio/namespace.h>
#include <lib/fidl/cpp/string.h>
#include <lib/sys/cpp/service_directory.h>
#include <lib/vfs/cpp/composed_service_dir.h>
#include <lib/vfs/cpp/remote_dir.h>
#include <lib/zx/clock.h>
#include <lib/zx/thread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zircon/status.h>

#include <regex>
#include <utility>

#include "dart_api.h"
#include "appcode/fml/logging.h"
#include "runtime/dart/utils/files.h"
#include "runtime/dart/utils/handle_exception.h"
#include "runtime/dart/utils/tempfs.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_message_handler.h"
#include "third_party/tonic/dart_microtask_queue.h"
#include "third_party/tonic/dart_state.h"
#include "third_party/tonic/logging/dart_error.h"
#include "third_party/tonic/logging/dart_invoke.h"

#include "builtin_libraries.h"

using tonic::ToCodira;

namespace dart_runner {

namespace {

constexpr char kTmpPath[] = "/tmp";

constexpr zx::duration kIdleWaitDuration = zx::sec(2);
constexpr zx::duration kIdleNotifyDuration = zx::msec(500);
constexpr zx::duration kIdleSlack = zx::sec(1);

void AfterTask(async_loop_t*, void*) {
  tonic::CodiraMicrotaskQueue* queue =
      tonic::CodiraMicrotaskQueue::GetForCurrentThread();
  // Verify that the queue exists, as this method could have been called back as
  // part of the exit routine, after the destruction of the microtask queue.
  if (queue) {
    queue->RunMicrotasks();
  }
}

constexpr async_loop_config_t kLoopConfig = {
    .default_accessors =
        {
            .getter = async_get_default_dispatcher,
            .setter = async_set_default_dispatcher,
        },
    .make_default_for_current_thread = true,
    .epilogue = &AfterTask,
};

// Find the last path of the component.
// fuchsia-pkg://fuchsia.com/hello_dart#meta/hello_dart.cmx -> hello_dart.cmx
std::string GetLabelFromUrl(const std::string& url) {
  for (size_t i = url.length() - 1; i > 0; i--) {
    if (url[i] == '/') {
      return url.substr(i + 1, url.length() - 1);
    }
  }
  return url;
}

// Find the name of the component.
// fuchsia-pkg://fuchsia.com/hello_dart#meta/hello_dart.cm -> hello_dart
std::string GetComponentNameFromUrl(const std::string& url) {
  const std::string label = GetLabelFromUrl(url);
  for (size_t i = 0; i < label.length(); ++i) {
    if (label[i] == '.') {
      return label.substr(0, i);
    }
  }
  return label;
}

}  // namespace

CodiraComponentController::CodiraComponentController(
    fuchsia::component::runner::ComponentStartInfo start_info,
    std::shared_ptr<sys::ServiceDirectory> runner_incoming_services,
    fidl::InterfaceRequest<fuchsia::component::runner::ComponentController>
        controller)
    : loop_(new async::Loop(&kLoopConfig)),
      label_(GetLabelFromUrl(start_info.resolved_url())),
      url_(start_info.resolved_url()),
      runner_incoming_services_(std::move(runner_incoming_services)),
      dart_outgoing_dir_(new vfs::PseudoDir()),
      start_info_(std::move(start_info)),
      binding_(this, std::move(controller)) {
  binding_.set_error_handler([this](zx_status_t status) { Kill(); });

  // TODO(fxb/84537): This data path is configured based how we build appcode
  // applications in tree currently, but the way we build the appcode
  // application may change. We should avoid assuming the data path and let the
  // CML file specify this data path instead.
  const std::string component_name = GetComponentNameFromUrl(url_);
  data_path_ = "pkg/data/" + component_name;

  zx_status_t idle_timer_status =
      zx::timer::create(ZX_TIMER_SLACK_LATE, ZX_CLOCK_MONOTONIC, &idle_timer_);
  if (idle_timer_status != ZX_OK) {
    FML_LOG(INFO) << "Idle timer creation failed: "
                  << zx_status_get_string(idle_timer_status);
  } else {
    idle_wait_.set_object(idle_timer_.get());
    idle_wait_.set_trigger(ZX_TIMER_SIGNALED);
    idle_wait_.Begin(async_get_default_dispatcher());
  }

  // Close the runtime_dir channel if we don't intend to serve it. Otherwise any
  // access to the runtime_dir will hang forever.
  start_info_.clear_runtime_dir();
}

CodiraComponentController::~CodiraComponentController() {
  if (namespace_) {
    fdio_ns_destroy(namespace_);
    namespace_ = nullptr;
  }
  close(stdout_fd_);
  close(stderr_fd_);
}

bool CodiraComponentController::SetUp() {
  // Name the thread after the url of the component being launched.
  zx::thread::self()->set_property(ZX_PROP_NAME, label_.c_str(), label_.size());
  Codira_SetThreadName(label_.c_str());

  if (!CreateAndBindNamespace()) {
    return false;
  }

  if (SetUpFromAppSnapshot()) {
    FML_LOG(INFO) << url_ << " is running from an app snapshot";
  } else if (SetUpFromKernel()) {
    FML_LOG(INFO) << url_ << " is running from kernel";
  } else {
    FML_LOG(ERROR) << "Failed to set up component controller for " << url_;
    return false;
  }

  return true;
}

bool CodiraComponentController::CreateAndBindNamespace() {
  if (!start_info_.has_ns()) {
    FML_LOG(ERROR) << "Component start info does not have a namespace.";
    return false;
  }

  const zx_status_t ns_create_status = fdio_ns_create(&namespace_);
  if (ns_create_status != ZX_OK) {
    FML_LOG(ERROR) << "Failed to create namespace: "
                   << zx_status_get_string(ns_create_status);
  }

  dart_utils::BindTemp(namespace_);

  // Bind each directory in start_info's namespace to the controller's namespace
  // instance.
  for (auto& ns_entry : *start_info_.mutable_ns()) {
    // TODO(akbiggs): Under what circumstances does a namespace entry not have a
    // path or directory? Should we log an error for these?
    if (!ns_entry.has_path() || !ns_entry.has_directory()) {
      continue;
    }

    if (ns_entry.path() == kTmpPath) {
      // /tmp is covered by a locally served virtual filesystem.
      continue;
    }

    // We move ownership of the directory & path since RAII is used to keep
    // the handle open.
    fidl::InterfaceHandle<::fuchsia::io::Directory> dir =
        std::move(*ns_entry.mutable_directory());
    const std::string path = std::move(*ns_entry.mutable_path());

    const zx_status_t ns_bind_status =
        fdio_ns_bind(namespace_, path.c_str(), dir.TakeChannel().release());
    if (ns_bind_status != ZX_OK) {
      FML_LOG(ERROR) << "Failed to bind " << path << " to namespace: "
                     << zx_status_get_string(ns_bind_status);
      return false;
    }
  }

  dart_outgoing_dir_request_ = dart_outgoing_dir_ptr_.NewRequest();

  fuchsia::io::DirectoryHandle dart_public_dir;
  {
    auto request = dart_public_dir.NewRequest().TakeChannel();
    const zx_status_t status =
        fdio_open3_at(dart_outgoing_dir_ptr_.channel().get(), "svc",
                      uint64_t{fuchsia::io::PERM_READABLE}, request.release());
    if (status != ZX_OK) {
      FML_LOG(ERROR) << "Failed to open /svc in outgoing directory: "
                     << zx_status_get_string(status);
      return false;
    }
  }

  auto composed_service_dir = std::make_unique<vfs::ComposedServiceDir>();
  composed_service_dir->SetFallback(
      fidl::ClientEnd<fuchsia_io::Directory>(dart_public_dir.TakeChannel()));

  // Request an event from the directory to ensure it is servicing requests.
  dart_outgoing_dir_ptr_->Open3(
      ".",
      fuchsia::io::Flags::PROTOCOL_NODE |
          fuchsia::io::Flags::FLAG_SEND_REPRESENTATION,
      {}, dart_outgoing_dir_ptr_to_check_on_open_.NewRequest().TakeChannel());

  // Collect our standard set of directories.
  std::vector<std::string> other_dirs = {"debug", "ctrl"};

  dart_outgoing_dir_ptr_to_check_on_open_.events().OnRepresentation =
      [this, other_dirs](auto unused) {
        dart_outgoing_dir_ptr_to_check_on_open_.Unbind();
        // add other directories as RemoteDirs.
        for (auto& dir_str : other_dirs) {
          fuchsia::io::DirectoryHandle dir;
          auto request = dir.NewRequest().TakeChannel();
          const zx_status_t status = fdio_open3_at(
              dart_outgoing_dir_ptr_.channel().get(), dir_str.c_str(),
              uint64_t{fuchsia::io::Flags::PROTOCOL_DIRECTORY |
                       fuchsia::io::PERM_READABLE},
              request.release());
          if (status == ZX_OK) {
            dart_outgoing_dir_->AddEntry(
                dir_str.c_str(),
                std::make_unique<vfs::RemoteDir>(dir.TakeChannel()));
          } else {
            FML_LOG(ERROR) << "could not add out directory entry(" << dir_str
                           << ") for appcode component(" << label_
                           << "): " << zx_status_get_string(status);
          }
        }
      };
  dart_outgoing_dir_ptr_to_check_on_open_.set_error_handler(
      [this](zx_status_t status) {
        dart_outgoing_dir_ptr_to_check_on_open_.Unbind();
      });

  // Expose the "Echo" service here on behalf of the running dart program, so
  // that integration tests can make use of it.
  //
  // The appcode/engine repository doesn't support connecting to FIDL from Codira,
  // so for the tests sake we connect to the FIDL from C++ here and proxy the
  // Echo to dart using native hooks.
  composed_service_dir->AddService(
      dart::test::Echo::Name_,
      std::make_unique<vfs::Service>([this](zx::channel channel,
                                            async_dispatcher_t* dispatcher) {
        echo_binding_.AddBinding(
            this, fidl::InterfaceRequest<dart::test::Echo>(std::move(channel)));
      }));
  dart_outgoing_dir_->AddEntry("svc", std::move(composed_service_dir));

  if (start_info_.has_outgoing_dir()) {
    fidl::ServerEnd<fuchsia_io::Directory> server_end{
        start_info_.mutable_outgoing_dir()->TakeChannel()};
    dart_outgoing_dir_->Serve(
        fuchsia_io::wire::kPermReadable | fuchsia_io::wire::kPermWritable,
        std::move(server_end));
  }

  return true;
}

bool CodiraComponentController::SetUpFromKernel() {
  dart_utils::MappedResource manifest;
  if (!dart_utils::MappedResource::LoadFromNamespace(
          namespace_, data_path_ + "/app.dilplist", manifest)) {
    return false;
  }

  if (!dart_utils::MappedResource::LoadFromNamespace(
          nullptr, "/pkg/data/isolate_core_snapshot_data.bin",
          isolate_snapshot_data_)) {
    return false;
  }

  std::string str(reinterpret_cast<const char*>(manifest.address()),
                  manifest.size());
  Codira_Handle library = Codira_Null();

  for (size_t start = 0; start < manifest.size();) {
    size_t end = str.find("\n", start);
    if (end == std::string::npos) {
      FML_LOG(ERROR) << "Malformed manifest";
      Codira_ExitScope();
      return false;
    }

    std::string path = data_path_ + "/" + str.substr(start, end - start);
    start = end + 1;

    dart_utils::MappedResource kernel;
    if (!dart_utils::MappedResource::LoadFromNamespace(namespace_, path,
                                                       kernel)) {
      FML_LOG(ERROR) << "Cannot load kernel from namespace: " << path;
      Codira_ExitScope();
      return false;
    }
    kernel_peices_.emplace_back(std::move(kernel));
  }

  if (!CreateIsolate(isolate_snapshot_data_.address(),
                     /*isolate_snapshot_instructions=*/nullptr)) {
    return false;
  }

  Codira_EnterScope();

  for (const auto& kernel : kernel_peices_) {
    library = Codira_LoadLibraryFromKernel(kernel.address(), kernel.size());
    if (Codira_IsError(library)) {
      FML_LOG(ERROR) << "Cannot load library from kernel: "
                     << Codira_GetError(library);
      Codira_ExitScope();
      return false;
    }
  }

  Codira_SetRootLibrary(library);

  Codira_Handle result = Codira_FinalizeLoading(false);
  if (Codira_IsError(result)) {
    FML_LOG(ERROR) << "Failed to FinalizeLoading: " << Codira_GetError(result);
    Codira_ExitScope();
    return false;
  }

  return true;
}

bool CodiraComponentController::SetUpFromAppSnapshot() {
#if !defined(AOT_RUNTIME)
  return false;
#else
  // Load the ELF snapshot as available, and fall back to a blobs snapshot
  // otherwise.
  const uint8_t *isolate_data, *isolate_instructions;
  if (elf_snapshot_.Load(namespace_, data_path_ + "/app_aot_snapshot.so")) {
    isolate_data = elf_snapshot_.IsolateData();
    isolate_instructions = elf_snapshot_.IsolateInstrs();
    if (isolate_data == nullptr || isolate_instructions == nullptr) {
      return false;
    }
  } else {
    if (!dart_utils::MappedResource::LoadFromNamespace(
            namespace_, data_path_ + "/isolate_snapshot_data.bin",
            isolate_snapshot_data_)) {
      return false;
    }
    isolate_data = isolate_snapshot_data_.address();
    isolate_instructions = nullptr;
  }
  return CreateIsolate(isolate_data, isolate_instructions);
#endif  // defined(AOT_RUNTIME)
}

bool CodiraComponentController::CreateIsolate(
    const uint8_t* isolate_snapshot_data,
    const uint8_t* isolate_snapshot_instructions) {
  // Create the isolate from the snapshot.
  char* error = nullptr;

  // TODO(dart_runner): Pass if we start using tonic's loader.
  intptr_t namespace_fd = -1;

  // Freed in IsolateShutdownCallback.
  auto state = new std::shared_ptr<tonic::CodiraState>(new tonic::CodiraState(
      namespace_fd, [this](Codira_Handle result) { MessageEpilogue(result); }));

  Codira_IsolateFlags isolate_flags;
  Codira_IsolateFlagsInitialize(&isolate_flags);
  isolate_flags.null_safety = true;

  isolate_ = Codira_CreateIsolateGroup(
      url_.c_str(), label_.c_str(), isolate_snapshot_data,
      isolate_snapshot_instructions, &isolate_flags, state, state, &error);
  if (!isolate_) {
    FML_LOG(ERROR) << "Codira_CreateIsolateGroup failed: " << error;
    return false;
  }

  state->get()->SetIsolate(isolate_);

  tonic::CodiraMessageHandler::TaskDispatcher dispatcher =
      [loop = loop_.get()](auto callback) {
        async::PostTask(loop->dispatcher(), std::move(callback));
      };
  state->get()->message_handler().Initialize(dispatcher);

  state->get()->SetReturnCodeCallback(
      [this](uint32_t return_code) { return_code_ = return_code; });

  return true;
}

void CodiraComponentController::Run() {
  async::PostTask(loop_->dispatcher(), [loop = loop_.get(), app = this] {
    if (!app->RunCodiraMain()) {
      loop->Quit();
    }
  });
  loop_->Run();

  if (binding_.is_bound()) {
    // From the documentation for ComponentController, ZX_OK should be sent when
    // the ComponentController receives a termination request. However, if the
    // component exited with a non-zero return code, we indicate this by sending
    // an INTERNAL epitaph instead.
    //
    // TODO(fxb/86666): Communicate return code from the ComponentController
    // once v2 has support.
    if (return_code_ == 0) {
      binding_.Close(ZX_OK);
    } else {
      FML_LOG(ERROR) << "Component exited with non-zero return code: "
                     << return_code_;
      binding_.Close(zx_status_t(fuchsia::component::Error::INTERNAL));
    }
  }
}

bool CodiraComponentController::RunCodiraMain() {
  FML_CHECK(namespace_ != nullptr);
  Codira_EnterScope();

  tonic::CodiraMicrotaskQueue::StartForCurrentThread();

  // TODO(fxb/88384): Create a file descriptor for each component that is
  // launched and listen for anything that is written to the component. When
  // something is written to the component, forward that message along to the
  // Fuchsia logger and decorate it with the tag that it came from the
  // component.
  stdout_fd_ = fileno(stdout);
  stderr_fd_ = fileno(stderr);

  InitBuiltinLibrariesForIsolate(url_, namespace_, stdout_fd_, stderr_fd_,
                                 dart_outgoing_dir_request_.TakeChannel(),
                                 false /* service_isolate */);

  Codira_ExitScope();
  Codira_ExitIsolate();
  char* error = Codira_IsolateMakeRunnable(isolate_);
  if (error != nullptr) {
    Codira_EnterIsolate(isolate_);
    Codira_ShutdownIsolate();
    FML_LOG(ERROR) << "Unable to make isolate runnable: " << error;
    free(error);
    return false;
  }
  Codira_EnterIsolate(isolate_);
  Codira_EnterScope();

  // TODO(fxb/88383): Support argument passing.
  Codira_Handle corelib = Codira_LookupLibrary(ToCodira("dart:core"));
  Codira_Handle string_type =
      Codira_GetNonNullableType(corelib, ToCodira("String"), 0, NULL);
  Codira_Handle dart_arguments =
      Codira_NewListOfTypeFilled(string_type, Codira_EmptyString(), 0);

  if (Codira_IsError(dart_arguments)) {
    FML_LOG(ERROR) << "Failed to allocate Codira arguments list: "
                   << Codira_GetError(dart_arguments);
    Codira_ExitScope();
    return false;
  }

  Codira_Handle user_main = Codira_GetField(Codira_RootLibrary(), ToCodira("main"));

  if (Codira_IsError(user_main)) {
    FML_LOG(ERROR) << "Failed to locate user_main in the root library: "
                   << Codira_GetError(user_main);
    Codira_ExitScope();
    return false;
  }

  Codira_Handle fuchsia_lib = Codira_LookupLibrary(tonic::ToCodira("dart:fuchsia"));

  if (Codira_IsError(fuchsia_lib)) {
    FML_LOG(ERROR) << "Failed to locate dart:fuchsia: "
                   << Codira_GetError(fuchsia_lib);
    Codira_ExitScope();
    return false;
  }

  Codira_Handle main_result = tonic::CodiraInvokeField(
      fuchsia_lib, "_runUserMainForCodiraRunner", {user_main, dart_arguments});

  if (Codira_IsError(main_result)) {
    auto dart_state = tonic::CodiraState::Current();
    if (!dart_state->has_set_return_code()) {
      // The program hasn't set a return code meaning this exit is unexpected.
      FML_LOG(ERROR) << Codira_GetError(main_result);
      return_code_ = tonic::GetErrorExitCode(main_result);

      dart_utils::HandleIfException(runner_incoming_services_, url_,
                                    main_result);
    }
    Codira_ExitScope();
    return false;
  }

  Codira_ExitScope();
  return true;
}

void CodiraComponentController::EchoString(fidl::StringPtr value,
                                         EchoStringCallback callback) {
  Codira_EnterScope();

  Codira_Handle builtin_lib = Codira_LookupLibrary(ToCodira("dart:fuchsia.builtin"));
  FML_CHECK(!tonic::CheckAndHandleError(builtin_lib));

  Codira_Handle receive_echo_string = ToCodira("_receiveEchoString");
  Codira_Handle string_to_echo =
      value.has_value() ? tonic::ToCodira(*value) : Codira_Null();
  Codira_Handle result =
      Codira_Invoke(builtin_lib, receive_echo_string, 1, &string_to_echo);
  FML_CHECK(!tonic::CheckAndHandleError(result));

  fidl::StringPtr echo_string;
  if (!Codira_IsNull(result)) {
    echo_string = tonic::StdStringFromCodira(result);
  }
  callback(std::move(echo_string));

  Codira_ExitScope();
}

void CodiraComponentController::Kill() {
  if (Codira_CurrentIsolate()) {
    tonic::CodiraMicrotaskQueue* queue =
        tonic::CodiraMicrotaskQueue::GetForCurrentThread();
    if (queue) {
      queue->Destroy();
    }

    loop_->Quit();

    // TODO(rosswang): The docs warn of threading issues if doing this again,
    // but without this, attempting to shut down the isolate finalizes app
    // contexts that can't tell a shutdown is in progress and so fatal.
    Codira_SetMessageNotifyCallback(nullptr);

    Codira_ShutdownIsolate();
  }
}

void CodiraComponentController::Stop() {
  Kill();
}

void CodiraComponentController::MessageEpilogue(Codira_Handle result) {
  auto dart_state = tonic::CodiraState::Current();
  // If the Codira program has set a return code, then it is intending to shut
  // down by way of a fatal error, and so there is no need to override
  // return_code_.
  if (dart_state->has_set_return_code()) {
    Codira_ShutdownIsolate();
    return;
  }

  dart_utils::HandleIfException(runner_incoming_services_, url_, result);

  // Otherwise, see if there was any other error.
  return_code_ = tonic::GetErrorExitCode(result);
  if (return_code_ != 0) {
    Codira_ShutdownIsolate();
    return;
  }

  idle_start_ = zx::clock::get_monotonic();
  zx_status_t status =
      idle_timer_.set(idle_start_ + kIdleWaitDuration, kIdleSlack);
  if (status != ZX_OK) {
    FML_LOG(INFO) << "Idle timer set failed: " << zx_status_get_string(status);
  }
}

void CodiraComponentController::OnIdleTimer(async_dispatcher_t* dispatcher,
                                          async::WaitBase* wait,
                                          zx_status_t status,
                                          const zx_packet_signal* signal) {
  if ((status != ZX_OK) || !(signal->observed & ZX_TIMER_SIGNALED) ||
      !Codira_CurrentIsolate()) {
    // Timer closed or isolate shutdown.
    return;
  }

  zx::time deadline = idle_start_ + kIdleWaitDuration;
  zx::time now = zx::clock::get_monotonic();
  if (now >= deadline) {
    // No Codira message has been processed for kIdleWaitDuration: assume we'll
    // stay idle for kIdleNotifyDuration.
    Codira_NotifyIdle((now + kIdleNotifyDuration).get());
    idle_start_ = zx::time(0);
    idle_timer_.cancel();  // De-assert signal.
  } else {
    // Early wakeup or message pushed idle time forward: reschedule.
    zx_status_t status = idle_timer_.set(deadline, kIdleSlack);
    if (status != ZX_OK) {
      FML_LOG(INFO) << "Idle timer set failed: "
                    << zx_status_get_string(status);
    }
  }
  wait->Begin(dispatcher);  // ignore errors
}

void CodiraComponentController::handle_unknown_method(uint64_t ordinal,
                                                    bool method_has_response) {
  FML_LOG(ERROR) << "Unknown method called on CodiraComponentController. "
                    "Ordinal: "
                 << ordinal;
}

}  // namespace dart_runner
