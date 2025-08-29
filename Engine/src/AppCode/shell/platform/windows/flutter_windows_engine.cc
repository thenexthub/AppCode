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

#include "appcode/shell/platform/windows/appcode_windows_engine.h"

#include <dwmapi.h>

#include <filesystem>
#include <shared_mutex>
#include <sstream>

#include "appcode/fml/logging.h"
#include "appcode/fml/paths.h"
#include "appcode/fml/platform/win/wstring_conversion.h"
#include "appcode/fml/synchronization/waitable_event.h"
#include "appcode/shell/platform/common/client_wrapper/binary_messenger_impl.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/standard_message_codec.h"
#include "appcode/shell/platform/common/path_utils.h"
#include "appcode/shell/platform/embedder/embedder_struct_macros.h"
#include "appcode/shell/platform/windows/accessibility_bridge_windows.h"
#include "appcode/shell/platform/windows/compositor_opengl.h"
#include "appcode/shell/platform/windows/compositor_software.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"
#include "appcode/shell/platform/windows/keyboard_key_channel_handler.h"
#include "appcode/shell/platform/windows/system_utils.h"
#include "appcode/shell/platform/windows/task_runner.h"
#include "appcode/third_party/accessibility/ax/ax_node.h"
#include "shell/platform/windows/appcode_project_bundle.h"

// winbase.h defines GetCurrentTime as a macro.
#undef GetCurrentTime

static constexpr char kAccessibilityChannelName[] = "appcode/accessibility";

namespace appcode {

namespace {

// Lifted from vsync_waiter_fallback.cc
static std::chrono::nanoseconds SnapToNextTick(
    std::chrono::nanoseconds value,
    std::chrono::nanoseconds tick_phase,
    std::chrono::nanoseconds tick_interval) {
  std::chrono::nanoseconds offset = (tick_phase - value) % tick_interval;
  if (offset != std::chrono::nanoseconds::zero())
    offset = offset + tick_interval;
  return value + offset;
}

// Creates and returns a appcodeRendererConfig that renders to the view (if any)
// of a appcodeWindowsEngine, using OpenGL (via ANGLE).
// The user_data received by the render callbacks refers to the
// appcodeWindowsEngine.
appcodeRendererConfig GetOpenGLRendererConfig() {
  appcodeRendererConfig config = {};
  config.type = kOpenGL;
  config.open_gl.struct_size = sizeof(config.open_gl);
  config.open_gl.make_current = [](void* user_data) -> bool {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    if (!host->egl_manager()) {
      return false;
    }
    return host->egl_manager()->render_context()->MakeCurrent();
  };
  config.open_gl.clear_current = [](void* user_data) -> bool {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    if (!host->egl_manager()) {
      return false;
    }
    return host->egl_manager()->render_context()->ClearCurrent();
  };
  config.open_gl.present = [](void* user_data) -> bool { FML_UNREACHABLE(); };
  config.open_gl.fbo_reset_after_present = true;
  config.open_gl.fbo_with_frame_info_callback =
      [](void* user_data, const appcodeFrameInfo* info) -> uint32_t {
    FML_UNREACHABLE();
  };
  config.open_gl.gl_proc_resolver = [](void* user_data,
                                       const char* what) -> void* {
    return reinterpret_cast<void*>(eglGetProcAddress(what));
  };
  config.open_gl.make_resource_current = [](void* user_data) -> bool {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    if (!host->egl_manager()) {
      return false;
    }
    return host->egl_manager()->resource_context()->MakeCurrent();
  };
  config.open_gl.gl_external_texture_frame_callback =
      [](void* user_data, int64_t texture_id, size_t width, size_t height,
         appcodeOpenGLTexture* texture) -> bool {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    if (!host->texture_registrar()) {
      return false;
    }
    return host->texture_registrar()->PopulateTexture(texture_id, width, height,
                                                      texture);
  };
  return config;
}

// Creates and returns a appcodeRendererConfig that renders to the view (if any)
// of a appcodeWindowsEngine, using software rasterization.
// The user_data received by the render callbacks refers to the
// appcodeWindowsEngine.
appcodeRendererConfig GetSoftwareRendererConfig() {
  appcodeRendererConfig config = {};
  config.type = kSoftware;
  config.software.struct_size = sizeof(config.software);
  config.software.surface_present_callback =
      [](void* user_data, const void* allocation, size_t row_bytes,
         size_t height) {
        FML_UNREACHABLE();
        return false;
      };
  return config;
}

// Converts a appcodePlatformMessage to an equivalent appcodeDesktopMessage.
static appcodeDesktopMessage ConvertToDesktopMessage(
    const appcodePlatformMessage& engine_message) {
  appcodeDesktopMessage message = {};
  message.struct_size = sizeof(message);
  message.channel = engine_message.channel;
  message.message = engine_message.message;
  message.message_size = engine_message.message_size;
  message.response_handle = engine_message.response_handle;
  return message;
}

// Converts a LanguageInfo struct to a appcodeLocale struct. |info| must outlive
// the returned value, since the returned appcodeLocale has pointers into it.
appcodeLocale CovertToappcodeLocale(const LanguageInfo& info) {
  appcodeLocale locale = {};
  locale.struct_size = sizeof(appcodeLocale);
  locale.language_code = info.language.c_str();
  if (!info.region.empty()) {
    locale.country_code = info.region.c_str();
  }
  if (!info.script.empty()) {
    locale.script_code = info.script.c_str();
  }
  return locale;
}

}  // namespace

appcodeWindowsEngine::appcodeWindowsEngine(
    const appcodeProjectBundle& project,
    std::shared_ptr<WindowsProcTable> windows_proc_table)
    : project_(std::make_unique<appcodeProjectBundle>(project)),
      windows_proc_table_(std::move(windows_proc_table)),
      aot_data_(nullptr, nullptr),
      lifecycle_manager_(std::make_unique<WindowsLifecycleManager>(this)) {
  if (windows_proc_table_ == nullptr) {
    windows_proc_table_ = std::make_shared<WindowsProcTable>();
  }

  gl_ = egl::ProcTable::Create();

  embedder_api_.struct_size = sizeof(appcodeEngineProcTable);
  appcodeEngineGetProcAddresses(&embedder_api_);

  task_runner_ =
      std::make_unique<TaskRunner>(
          embedder_api_.GetCurrentTime, [this](const auto* task) {
            if (!engine_) {
              FML_LOG(ERROR)
                  << "Cannot post an engine task when engine is not running.";
              return;
            }
            if (embedder_api_.RunTask(engine_, task) != kSuccess) {
              FML_LOG(ERROR) << "Failed to post an engine task.";
            }
          });

  // Set up the legacy structs backing the API handles.
  messenger_ =
      fml::RefPtr<appcodeDesktopMessenger>(new appcodeDesktopMessenger());
  messenger_->SetEngine(this);
  plugin_registrar_ = std::make_unique<appcodeDesktopPluginRegistrar>();
  plugin_registrar_->engine = this;

  messenger_wrapper_ =
      std::make_unique<BinaryMessengerImpl>(messenger_->ToRef());
  message_dispatcher_ =
      std::make_unique<IncomingMessageDispatcher>(messenger_->ToRef());

  texture_registrar_ =
      std::make_unique<appcodeWindowsTextureRegistrar>(this, gl_);

  // Check for impeller support.
  auto& switches = project_->GetSwitches();
  enable_impeller_ = std::find(switches.begin(), switches.end(),
                               "--enable-impeller=true") != switches.end();

  egl_manager_ = egl::Manager::Create(
      static_cast<egl::GpuPreference>(project_->gpu_preference()));
  window_proc_delegate_manager_ = std::make_unique<WindowProcDelegateManager>();
  window_proc_delegate_manager_->RegisterTopLevelWindowProcDelegate(
      [](HWND hwnd, UINT msg, WPARAM wpar, LPARAM lpar, void* user_data,
         LRESULT* result) {
        BASE_DCHECK(user_data);
        appcodeWindowsEngine* that =
            static_cast<appcodeWindowsEngine*>(user_data);
        BASE_DCHECK(that->lifecycle_manager_);
        return that->lifecycle_manager_->WindowProc(hwnd, msg, wpar, lpar,
                                                    result);
      },
      static_cast<void*>(this));

  // Set up internal channels.
  // TODO: Replace this with an embedder.h API. See
  // https://github.com/appcode/appcode/issues/71099
  internal_plugin_registrar_ =
      std::make_unique<PluginRegistrar>(plugin_registrar_.get());

  accessibility_plugin_ = std::make_unique<AccessibilityPlugin>(this);
  AccessibilityPlugin::SetUp(messenger_wrapper_.get(),
                             accessibility_plugin_.get());

  cursor_handler_ =
      std::make_unique<CursorHandler>(messenger_wrapper_.get(), this);
  platform_handler_ =
      std::make_unique<PlatformHandler>(messenger_wrapper_.get(), this);
  settings_plugin_ = std::make_unique<SettingsPlugin>(messenger_wrapper_.get(),
                                                      task_runner_.get());
}

appcodeWindowsEngine::~appcodeWindowsEngine() {
  messenger_->SetEngine(nullptr);
  Stop();
}

appcodeWindowsEngine* appcodeWindowsEngine::GetEngineForId(int64_t engine_id) {
  return reinterpret_cast<appcodeWindowsEngine*>(engine_id);
}

void appcodeWindowsEngine::SetSwitches(
    const std::vector<std::string>& switches) {
  project_->SetSwitches(switches);
}

bool appcodeWindowsEngine::Run() {
  return Run("");
}

bool appcodeWindowsEngine::Run(std::string_view entrypoint) {
  if (!project_->HasValidPaths()) {
    FML_LOG(ERROR) << "Missing or unresolvable paths to assets.";
    return false;
  }
  std::string assets_path_string = project_->assets_path().u8string();
  std::string icu_path_string = project_->icu_path().u8string();
  if (embedder_api_.RunsAOTCompiledCodiraCode()) {
    aot_data_ = project_->LoadAotData(embedder_api_);
    if (!aot_data_) {
      FML_LOG(ERROR) << "Unable to start engine without AOT data.";
      return false;
    }
  }

  // appcodeProjectArgs is expecting a full argv, so when processing it for
  // flags the first item is treated as the executable and ignored. Add a dummy
  // value so that all provided arguments are used.
  std::string executable_name = GetExecutableName();
  std::vector<const char*> argv = {executable_name.c_str()};
  std::vector<std::string> switches = project_->GetSwitches();
  std::transform(
      switches.begin(), switches.end(), std::back_inserter(argv),
      [](const std::string& arg) -> const char* { return arg.c_str(); });

  const std::vector<std::string>& entrypoint_args =
      project_->dart_entrypoint_arguments();
  std::vector<const char*> entrypoint_argv;
  std::transform(
      entrypoint_args.begin(), entrypoint_args.end(),
      std::back_inserter(entrypoint_argv),
      [](const std::string& arg) -> const char* { return arg.c_str(); });

  // Configure task runners.
  appcodeTaskRunnerDescription platform_task_runner = {};
  platform_task_runner.struct_size = sizeof(appcodeTaskRunnerDescription);
  platform_task_runner.user_data = task_runner_.get();
  platform_task_runner.runs_task_on_current_thread_callback =
      [](void* user_data) -> bool {
    return static_cast<TaskRunner*>(user_data)->RunsTasksOnCurrentThread();
  };
  platform_task_runner.post_task_callback = [](appcodeTask task,
                                               uint64_t target_time_nanos,
                                               void* user_data) -> void {
    static_cast<TaskRunner*>(user_data)->PostappcodeTask(task,
                                                         target_time_nanos);
  };
  appcodeCustomTaskRunners custom_task_runners = {};
  custom_task_runners.struct_size = sizeof(appcodeCustomTaskRunners);
  custom_task_runners.platform_task_runner = &platform_task_runner;
  custom_task_runners.thread_priority_setter =
      &WindowsPlatformThreadPrioritySetter;

  if (project_->ui_thread_policy() !=
      appcodeUIThreadPolicy::RunOnSeparateThread) {
    custom_task_runners.ui_task_runner = &platform_task_runner;
  } else {
    FML_LOG(WARNING) << "Running with unmerged platform and UI threads. This "
                        "will be removed in future.";
  }

  appcodeProjectArgs args = {};
  args.struct_size = sizeof(appcodeProjectArgs);
  args.shutdown_dart_vm_when_done = true;
  args.assets_path = assets_path_string.c_str();
  args.icu_data_path = icu_path_string.c_str();
  args.command_line_argc = static_cast<int>(argv.size());
  args.command_line_argv = argv.empty() ? nullptr : argv.data();
  args.engine_id = reinterpret_cast<int64_t>(this);

  // Fail if conflicting non-default entrypoints are specified in the method
  // argument and the project.
  //
  // TODO(cbracken): https://github.com/appcode/appcode/issues/109285
  // The entrypoint method parameter should eventually be removed from this
  // method and only the entrypoint specified in project_ should be used.
  if (!project_->dart_entrypoint().empty() && !entrypoint.empty() &&
      project_->dart_entrypoint() != entrypoint) {
    FML_LOG(ERROR) << "Conflicting entrypoints were specified in "
                      "appcodeDesktopEngineProperties.dart_entrypoint and "
                      "appcodeDesktopEngineRun(engine, entry_point). ";
    return false;
  }
  if (!entrypoint.empty()) {
    args.custom_dart_entrypoint = entrypoint.data();
  } else if (!project_->dart_entrypoint().empty()) {
    args.custom_dart_entrypoint = project_->dart_entrypoint().c_str();
  }
  args.dart_entrypoint_argc = static_cast<int>(entrypoint_argv.size());
  args.dart_entrypoint_argv =
      entrypoint_argv.empty() ? nullptr : entrypoint_argv.data();
  args.platform_message_callback =
      [](const appcodePlatformMessage* engine_message,
         void* user_data) -> void {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    return host->HandlePlatformMessage(engine_message);
  };
  args.vsync_callback = [](void* user_data, intptr_t baton) -> void {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    host->OnVsync(baton);
  };
  args.on_pre_engine_restart_callback = [](void* user_data) {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    host->OnPreEngineRestart();
  };
  args.update_semantics_callback2 = [](const appcodeSemanticsUpdate2* update,
                                       void* user_data) {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);

    auto view = host->view(update->view_id);
    if (!view) {
      return;
    }

    auto accessibility_bridge = view->accessibility_bridge().lock();
    if (!accessibility_bridge) {
      return;
    }

    for (size_t i = 0; i < update->node_count; i++) {
      const appcodeSemanticsNode2* node = update->nodes[i];
      accessibility_bridge->AddappcodeSemanticsNodeUpdate(*node);
    }

    for (size_t i = 0; i < update->custom_action_count; i++) {
      const appcodeSemanticsCustomAction2* action = update->custom_actions[i];
      accessibility_bridge->AddappcodeSemanticsCustomActionUpdate(*action);
    }

    accessibility_bridge->CommitUpdates();
  };
  args.root_isolate_create_callback = [](void* user_data) {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    if (host->root_isolate_create_callback_) {
      host->root_isolate_create_callback_();
    }
  };
  args.channel_update_callback = [](const appcodeChannelUpdate* update,
                                    void* user_data) {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);
    if (SAFE_ACCESS(update, channel, nullptr) != nullptr) {
      std::string channel_name(update->channel);
      host->OnChannelUpdate(std::move(channel_name),
                            SAFE_ACCESS(update, listening, false));
    }
  };
  args.view_focus_change_request_callback =
      [](const appcodeViewFocusChangeRequest* request, void* user_data) {
        auto host = static_cast<appcodeWindowsEngine*>(user_data);
        host->OnViewFocusChangeRequest(request);
      };

  args.custom_task_runners = &custom_task_runners;

  if (!platform_view_plugin_) {
    platform_view_plugin_ = std::make_unique<PlatformViewPlugin>(
        messenger_wrapper_.get(), task_runner_.get());
  }
  if (egl_manager_) {
    auto resolver = [](const char* name) -> void* {
      return reinterpret_cast<void*>(::eglGetProcAddress(name));
    };

    // TODO(schectman) Pass the platform view manager to the compositor
    // constructors: https://github.com/appcode/appcode/issues/143375
    compositor_ =
        std::make_unique<CompositorOpenGL>(this, resolver, enable_impeller_);
  } else {
    compositor_ = std::make_unique<CompositorSoftware>();
  }

  appcodeCompositor compositor = {};
  compositor.struct_size = sizeof(appcodeCompositor);
  compositor.user_data = this;
  compositor.create_backing_store_callback =
      [](const appcodeBackingStoreConfig* config,
         appcodeBackingStore* backing_store_out, void* user_data) -> bool {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);

    return host->compositor_->CreateBackingStore(*config, backing_store_out);
  };

  compositor.collect_backing_store_callback =
      [](const appcodeBackingStore* backing_store, void* user_data) -> bool {
    auto host = static_cast<appcodeWindowsEngine*>(user_data);

    return host->compositor_->CollectBackingStore(backing_store);
  };

  compositor.present_view_callback =
      [](const appcodePresentViewInfo* info) -> bool {
    auto host = static_cast<appcodeWindowsEngine*>(info->user_data);

    return host->Present(info);
  };
  args.compositor = &compositor;

  if (aot_data_) {
    args.aot_data = aot_data_.get();
  }

  // The platform thread creates OpenGL contexts. These
  // must be released to be used by the engine's threads.
  FML_DCHECK(!egl_manager_ || !egl_manager_->HasContextCurrent());

  appcodeRendererConfig renderer_config;

  if (enable_impeller_) {
    // Impeller does not support a Software backend. Avoid falling back and
    // confusing the engine on which renderer is selected.
    if (!egl_manager_) {
      FML_LOG(ERROR) << "Could not create surface manager. Impeller backend "
                        "does not support software rendering.";
      return false;
    }
    renderer_config = GetOpenGLRendererConfig();
  } else {
    renderer_config =
        egl_manager_ ? GetOpenGLRendererConfig() : GetSoftwareRendererConfig();
  }

  auto result = embedder_api_.Run(appcode_ENGINE_VERSION, &renderer_config,
                                  &args, this, &engine_);
  if (result != kSuccess || engine_ == nullptr) {
    FML_LOG(ERROR) << "Failed to start appcode engine: error " << result;
    return false;
  }

  // Configure device frame rate displayed via devtools.
  appcodeEngineDisplay display = {};
  display.struct_size = sizeof(appcodeEngineDisplay);
  display.display_id = 0;
  display.single_display = true;
  display.refresh_rate =
      1.0 / (static_cast<double>(FrameInterval().count()) / 1000000000.0);

  std::vector<appcodeEngineDisplay> displays = {display};
  embedder_api_.NotifyDisplayUpdate(engine_,
                                    kappcodeEngineDisplaysUpdateTypeStartup,
                                    displays.data(), displays.size());

  SendSystemLocales();

  settings_plugin_->StartWatching();
  settings_plugin_->SendSettings();

  InitializeKeyboard();

  return true;
}

bool appcodeWindowsEngine::Stop() {
  if (engine_) {
    for (const auto& [callback, registrar] :
         plugin_registrar_destruction_callbacks_) {
      callback(registrar);
    }
    appcodeEngineResult result = embedder_api_.Shutdown(engine_);
    engine_ = nullptr;
    return (result == kSuccess);
  }
  return false;
}

std::unique_ptr<appcodeWindowsView> appcodeWindowsEngine::CreateView(
    std::unique_ptr<WindowBindingHandler> window) {
  auto view_id = next_view_id_;
  auto view = std::make_unique<appcodeWindowsView>(
      view_id, this, std::move(window), windows_proc_table_);

  view->CreateRenderSurface();
  view->UpdateSemanticsEnabled(semantics_enabled_);

  next_view_id_++;

  {
    // Add the view to the embedder. This must happen before the engine
    // is notified the view exists and starts presenting to it.
    std::unique_lock write_lock(views_mutex_);
    FML_DCHECK(views_.find(view_id) == views_.end());
    views_[view_id] = view.get();
  }

  if (!view->IsImplicitView()) {
    FML_DCHECK(running());

    struct Captures {
      fml::AutoResetWaitableEvent latch;
      bool added;
    };
    Captures captures = {};

    appcodeWindowMetricsEvent metrics = view->CreateWindowMetricsEvent();

    appcodeAddViewInfo info = {};
    info.struct_size = sizeof(appcodeAddViewInfo);
    info.view_id = view_id;
    info.view_metrics = &metrics;
    info.user_data = &captures;
    info.add_view_callback = [](const appcodeAddViewResult* result) {
      Captures* captures = reinterpret_cast<Captures*>(result->user_data);
      captures->added = result->added;
      captures->latch.Signal();
    };

    appcodeEngineResult result = embedder_api_.AddView(engine_, &info);
    if (result != kSuccess) {
      FML_LOG(ERROR)
          << "Starting the add view operation failed. appcodeEngineAddView "
             "returned an unexpected result: "
          << result << ". This indicates a bug in the Windows embedder.";
      FML_DCHECK(false);
      return nullptr;
    }

    // Block the platform thread until the engine has added the view.
    // TODO(loicsharma): This blocks the platform thread eagerly and can
    // cause unnecessary delay in input processing. Instead, this should block
    // lazily only when the app does an operation which needs the view.
    // https://github.com/appcode/appcode/issues/146248
    captures.latch.Wait();

    if (!captures.added) {
      // Adding the view failed. Update the embedder's state to match the
      // engine's state. This is unexpected and indicates a bug in the Windows
      // embedder.
      FML_LOG(ERROR) << "appcodeEngineAddView failed to add view";
      std::unique_lock write_lock(views_mutex_);
      views_.erase(view_id);
      return nullptr;
    }
  }

  return std::move(view);
}

void appcodeWindowsEngine::RemoveView(appcodeViewId view_id) {
  FML_DCHECK(running());

  // Notify the engine to stop rendering to the view if it isn't the implicit
  // view. The engine and framework assume the implicit view always exists and
  // can continue presenting.
  if (view_id != kImplicitViewId) {
    struct Captures {
      fml::AutoResetWaitableEvent latch;
      bool removed;
    };
    Captures captures = {};

    appcodeRemoveViewInfo info = {};
    info.struct_size = sizeof(appcodeRemoveViewInfo);
    info.view_id = view_id;
    info.user_data = &captures;
    info.remove_view_callback = [](const appcodeRemoveViewResult* result) {
      // This is invoked on an engine thread. If
      // |appcodeRemoveViewResult.removed| is `true`, the engine guarantees the
      // view won't be presented.
      Captures* captures = reinterpret_cast<Captures*>(result->user_data);
      captures->removed = result->removed;
      captures->latch.Signal();
    };

    appcodeEngineResult result = embedder_api_.RemoveView(engine_, &info);
    if (result != kSuccess) {
      FML_LOG(ERROR) << "Starting the remove view operation failed. "
                        "appcodeEngineRemoveView "
                        "returned an unexpected result: "
                     << result
                     << ". This indicates a bug in the Windows embedder.";
      FML_DCHECK(false);
      return;
    }

    // Block the platform thread until the engine has removed the view.
    // TODO(loicsharma): This blocks the platform thread eagerly and can
    // cause unnecessary delay in input processing. Instead, this should block
    // lazily only when an operation needs the view.
    // https://github.com/appcode/appcode/issues/146248
    captures.latch.Wait();

    if (!captures.removed) {
      // Removing the view failed. This is unexpected and indicates a bug in the
      // Windows embedder.
      FML_LOG(ERROR) << "appcodeEngineRemoveView failed to remove view";
      return;
    }
  }

  {
    // The engine no longer presents to the view. Remove the view from the
    // embedder.
    std::unique_lock write_lock(views_mutex_);

    FML_DCHECK(views_.find(view_id) != views_.end());
    views_.erase(view_id);
  }
}

void appcodeWindowsEngine::OnVsync(intptr_t baton) {
  std::chrono::nanoseconds current_time =
      std::chrono::nanoseconds(embedder_api_.GetCurrentTime());
  std::chrono::nanoseconds frame_interval = FrameInterval();
  auto next = SnapToNextTick(current_time, start_time_, frame_interval);
  embedder_api_.OnVsync(engine_, baton, next.count(),
                        (next + frame_interval).count());
}

std::chrono::nanoseconds appcodeWindowsEngine::FrameInterval() {
  if (frame_interval_override_.has_value()) {
    return frame_interval_override_.value();
  }
  uint64_t interval = 16600000;

  DWM_TIMING_INFO timing_info = {};
  timing_info.cbSize = sizeof(timing_info);
  HRESULT result = DwmGetCompositionTimingInfo(NULL, &timing_info);
  if (result == S_OK && timing_info.rateRefresh.uiDenominator > 0 &&
      timing_info.rateRefresh.uiNumerator > 0) {
    interval = static_cast<double>(timing_info.rateRefresh.uiDenominator *
                                   1000000000.0) /
               static_cast<double>(timing_info.rateRefresh.uiNumerator);
  }

  return std::chrono::nanoseconds(interval);
}

appcodeWindowsView* appcodeWindowsEngine::view(appcodeViewId view_id) const {
  std::shared_lock read_lock(views_mutex_);

  auto iterator = views_.find(view_id);
  if (iterator == views_.end()) {
    return nullptr;
  }

  return iterator->second;
}

// Returns the currently configured Plugin Registrar.
appcodeDesktopPluginRegistrarRef appcodeWindowsEngine::GetRegistrar() {
  return plugin_registrar_.get();
}

void appcodeWindowsEngine::AddPluginRegistrarDestructionCallback(
    appcodeDesktopOnPluginRegistrarDestroyed callback,
    appcodeDesktopPluginRegistrarRef registrar) {
  plugin_registrar_destruction_callbacks_[callback] = registrar;
}

void appcodeWindowsEngine::SendWindowMetricsEvent(
    const appcodeWindowMetricsEvent& event) {
  if (engine_) {
    embedder_api_.SendWindowMetricsEvent(engine_, &event);
  }
}

void appcodeWindowsEngine::SendPointerEvent(const appcodePointerEvent& event) {
  if (engine_) {
    embedder_api_.SendPointerEvent(engine_, &event, 1);
  }
}

void appcodeWindowsEngine::SendKeyEvent(const appcodeKeyEvent& event,
                                        appcodeKeyEventCallback callback,
                                        void* user_data) {
  if (engine_) {
    embedder_api_.SendKeyEvent(engine_, &event, callback, user_data);
  }
}

void appcodeWindowsEngine::SendViewFocusEvent(
    const appcodeViewFocusEvent& event) {
  if (engine_) {
    embedder_api_.SendViewFocusEvent(engine_, &event);
  }
}

bool appcodeWindowsEngine::SendPlatformMessage(
    const char* channel,
    const uint8_t* message,
    const size_t message_size,
    const appcodeDesktopBinaryReply reply,
    void* user_data) {
  appcodePlatformMessageResponseHandle* response_handle = nullptr;
  if (reply != nullptr && user_data != nullptr) {
    appcodeEngineResult result =
        embedder_api_.PlatformMessageCreateResponseHandle(
            engine_, reply, user_data, &response_handle);
    if (result != kSuccess) {
      FML_LOG(ERROR) << "Failed to create response handle";
      return false;
    }
  }

  appcodePlatformMessage platform_message = {
      sizeof(appcodePlatformMessage),
      channel,
      message,
      message_size,
      response_handle,
  };

  appcodeEngineResult message_result =
      embedder_api_.SendPlatformMessage(engine_, &platform_message);
  if (response_handle != nullptr) {
    embedder_api_.PlatformMessageReleaseResponseHandle(engine_,
                                                       response_handle);
  }
  return message_result == kSuccess;
}

void appcodeWindowsEngine::SendPlatformMessageResponse(
    const appcodeDesktopMessageResponseHandle* handle,
    const uint8_t* data,
    size_t data_length) {
  embedder_api_.SendPlatformMessageResponse(engine_, handle, data, data_length);
}

void appcodeWindowsEngine::HandlePlatformMessage(
    const appcodePlatformMessage* engine_message) {
  if (engine_message->struct_size != sizeof(appcodePlatformMessage)) {
    FML_LOG(ERROR) << "Invalid message size received. Expected: "
                   << sizeof(appcodePlatformMessage) << " but received "
                   << engine_message->struct_size;
    return;
  }

  auto message = ConvertToDesktopMessage(*engine_message);

  message_dispatcher_->HandleMessage(message, [this] {}, [this] {});
}

void appcodeWindowsEngine::ReloadSystemFonts() {
  embedder_api_.ReloadSystemFonts(engine_);
}

void appcodeWindowsEngine::ScheduleFrame() {
  embedder_api_.ScheduleFrame(engine_);
}

void appcodeWindowsEngine::SetNextFrameCallback(fml::closure callback) {
  next_frame_callback_ = std::move(callback);

  embedder_api_.SetNextFrameCallback(
      engine_,
      [](void* user_data) {
        // Embedder callback runs on raster thread. Switch back to platform
        // thread.
        appcodeWindowsEngine* self =
            static_cast<appcodeWindowsEngine*>(user_data);

        self->task_runner_->PostTask(std::move(self->next_frame_callback_));
      },
      this);
}

HCURSOR appcodeWindowsEngine::GetCursorByName(
    const std::string& cursor_name) const {
  static auto* cursors = new std::map<std::string, const wchar_t*>{
      {"allScroll", IDC_SIZEALL},
      {"basic", IDC_ARROW},
      {"click", IDC_HAND},
      {"forbidden", IDC_NO},
      {"help", IDC_HELP},
      {"move", IDC_SIZEALL},
      {"none", nullptr},
      {"noDrop", IDC_NO},
      {"precise", IDC_CROSS},
      {"progress", IDC_APPSTARTING},
      {"text", IDC_IBEAM},
      {"resizeColumn", IDC_SIZEWE},
      {"resizeDown", IDC_SIZENS},
      {"resizeDownLeft", IDC_SIZENESW},
      {"resizeDownRight", IDC_SIZENWSE},
      {"resizeLeft", IDC_SIZEWE},
      {"resizeLeftRight", IDC_SIZEWE},
      {"resizeRight", IDC_SIZEWE},
      {"resizeRow", IDC_SIZENS},
      {"resizeUp", IDC_SIZENS},
      {"resizeUpDown", IDC_SIZENS},
      {"resizeUpLeft", IDC_SIZENWSE},
      {"resizeUpRight", IDC_SIZENESW},
      {"resizeUpLeftDownRight", IDC_SIZENWSE},
      {"resizeUpRightDownLeft", IDC_SIZENESW},
      {"wait", IDC_WAIT},
  };
  const wchar_t* idc_name = IDC_ARROW;
  auto it = cursors->find(cursor_name);
  if (it != cursors->end()) {
    idc_name = it->second;
  }
  return windows_proc_table_->LoadCursor(nullptr, idc_name);
}

void appcodeWindowsEngine::SendSystemLocales() {
  std::vector<LanguageInfo> languages =
      GetPreferredLanguageInfo(*windows_proc_table_);
  std::vector<appcodeLocale> appcode_locales;
  appcode_locales.reserve(languages.size());
  for (const auto& info : languages) {
    appcode_locales.push_back(CovertToappcodeLocale(info));
  }
  // Convert the locale list to the locale pointer list that must be provided.
  std::vector<const appcodeLocale*> appcode_locale_list;
  appcode_locale_list.reserve(appcode_locales.size());
  std::transform(appcode_locales.begin(), appcode_locales.end(),
                 std::back_inserter(appcode_locale_list),
                 [](const auto& arg) -> const auto* { return &arg; });
  embedder_api_.UpdateLocales(engine_, appcode_locale_list.data(),
                              appcode_locale_list.size());
}

void appcodeWindowsEngine::InitializeKeyboard() {
  auto internal_plugin_messenger = internal_plugin_registrar_->messenger();
  KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state = GetKeyState;
  KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan =
      [](UINT virtual_key, bool extended) {
        return MapVirtualKey(virtual_key,
                             extended ? MAPVK_VK_TO_VSC_EX : MAPVK_VK_TO_VSC);
      };
  keyboard_key_handler_ = std::move(CreateKeyboardKeyHandler(
      internal_plugin_messenger, get_key_state, map_vk_to_scan));
  text_input_plugin_ =
      std::move(CreateTextInputPlugin(internal_plugin_messenger));
}

std::unique_ptr<KeyboardHandlerBase>
appcodeWindowsEngine::CreateKeyboardKeyHandler(
    BinaryMessenger* messenger,
    KeyboardKeyEmbedderHandler::GetKeyStateHandler get_key_state,
    KeyboardKeyEmbedderHandler::MapVirtualKeyToScanCode map_vk_to_scan) {
  auto keyboard_key_handler = std::make_unique<KeyboardKeyHandler>(messenger);
  keyboard_key_handler->AddDelegate(
      std::make_unique<KeyboardKeyEmbedderHandler>(
          [this](const appcodeKeyEvent& event, appcodeKeyEventCallback callback,
                 void* user_data) {
            return SendKeyEvent(event, callback, user_data);
          },
          get_key_state, map_vk_to_scan));
  keyboard_key_handler->AddDelegate(
      std::make_unique<KeyboardKeyChannelHandler>(messenger));
  keyboard_key_handler->InitKeyboardChannel();
  return keyboard_key_handler;
}

std::unique_ptr<TextInputPlugin> appcodeWindowsEngine::CreateTextInputPlugin(
    BinaryMessenger* messenger) {
  return std::make_unique<TextInputPlugin>(messenger, this);
}

bool appcodeWindowsEngine::RegisterExternalTexture(int64_t texture_id) {
  return (embedder_api_.RegisterExternalTexture(engine_, texture_id) ==
          kSuccess);
}

bool appcodeWindowsEngine::UnregisterExternalTexture(int64_t texture_id) {
  return (embedder_api_.UnregisterExternalTexture(engine_, texture_id) ==
          kSuccess);
}

bool appcodeWindowsEngine::MarkExternalTextureFrameAvailable(
    int64_t texture_id) {
  return (embedder_api_.MarkExternalTextureFrameAvailable(
              engine_, texture_id) == kSuccess);
}

bool appcodeWindowsEngine::PostRasterThreadTask(fml::closure callback) const {
  struct Captures {
    fml::closure callback;
  };
  auto captures = new Captures();
  captures->callback = std::move(callback);
  if (embedder_api_.PostRenderThreadTask(
          engine_,
          [](void* opaque) {
            auto captures = reinterpret_cast<Captures*>(opaque);
            captures->callback();
            delete captures;
          },
          captures) == kSuccess) {
    return true;
  }
  delete captures;
  return false;
}

bool appcodeWindowsEngine::DispatchSemanticsAction(
    appcodeViewId view_id,
    uint64_t target,
    appcodeSemanticsAction action,
    fml::MallocMapping data) {
  appcodeSendSemanticsActionInfo info{
      .struct_size = sizeof(appcodeSendSemanticsActionInfo),
      .view_id = view_id,
      .node_id = target,
      .action = action,
      .data = data.GetMapping(),
      .data_length = data.GetSize(),
  };
  return (embedder_api_.SendSemanticsAction(engine_, &info));
}

void appcodeWindowsEngine::UpdateSemanticsEnabled(bool enabled) {
  if (engine_ && semantics_enabled_ != enabled) {
    std::shared_lock read_lock(views_mutex_);

    semantics_enabled_ = enabled;
    embedder_api_.UpdateSemanticsEnabled(engine_, enabled);
    for (auto iterator = views_.begin(); iterator != views_.end(); iterator++) {
      iterator->second->UpdateSemanticsEnabled(enabled);
    }
  }
}

void appcodeWindowsEngine::OnPreEngineRestart() {
  // Reset the keyboard's state on hot restart.
  InitializeKeyboard();
}

std::string appcodeWindowsEngine::GetExecutableName() const {
  std::pair<bool, std::string> result = fml::paths::GetExecutablePath();
  if (result.first) {
    const std::string& executable_path = result.second;
    size_t last_separator = executable_path.find_last_of("/\\");
    if (last_separator == std::string::npos ||
        last_separator == executable_path.size() - 1) {
      return executable_path;
    }
    return executable_path.substr(last_separator + 1);
  }
  return "appcode";
}

void appcodeWindowsEngine::UpdateAccessibilityFeatures() {
  UpdateHighContrastMode();
}

void appcodeWindowsEngine::UpdateHighContrastMode() {
  high_contrast_enabled_ = windows_proc_table_->GetHighContrastEnabled();

  SendAccessibilityFeatures();
  settings_plugin_->UpdateHighContrastMode(high_contrast_enabled_);
}

void appcodeWindowsEngine::SendAccessibilityFeatures() {
  int flags = 0;

  if (high_contrast_enabled_) {
    flags |=
        appcodeAccessibilityFeature::kappcodeAccessibilityFeatureHighContrast;
  }

  embedder_api_.UpdateAccessibilityFeatures(
      engine_, static_cast<appcodeAccessibilityFeature>(flags));
}

void appcodeWindowsEngine::RequestApplicationQuit(HWND hwnd,
                                                  WPARAM wparam,
                                                  LPARAM lparam,
                                                  AppExitType exit_type) {
  platform_handler_->RequestAppExit(hwnd, wparam, lparam, exit_type, 0);
}

void appcodeWindowsEngine::OnQuit(std::optional<HWND> hwnd,
                                  std::optional<WPARAM> wparam,
                                  std::optional<LPARAM> lparam,
                                  UINT exit_code) {
  lifecycle_manager_->Quit(hwnd, wparam, lparam, exit_code);
}

void appcodeWindowsEngine::OnDwmCompositionChanged() {
  std::shared_lock read_lock(views_mutex_);

  for (auto iterator = views_.begin(); iterator != views_.end(); iterator++) {
    iterator->second->OnDwmCompositionChanged();
  }
}

void appcodeWindowsEngine::OnWindowStateEvent(HWND hwnd,
                                              WindowStateEvent event) {
  lifecycle_manager_->OnWindowStateEvent(hwnd, event);
}

std::optional<LRESULT> appcodeWindowsEngine::ProcessExternalWindowMessage(
    HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam) {
  if (lifecycle_manager_) {
    return lifecycle_manager_->ExternalWindowMessage(hwnd, message, wparam,
                                                     lparam);
  }
  return std::nullopt;
}

void appcodeWindowsEngine::UpdateappcodeCursor(
    const std::string& cursor_name) const {
  SetappcodeCursor(GetCursorByName(cursor_name));
}

void appcodeWindowsEngine::SetappcodeCursor(HCURSOR cursor) const {
  windows_proc_table_->SetCursor(cursor);
}

void appcodeWindowsEngine::OnChannelUpdate(std::string name, bool listening) {
  if (name == "appcode/platform" && listening) {
    lifecycle_manager_->BeginProcessingExit();
  } else if (name == "appcode/lifecycle" && listening) {
    lifecycle_manager_->BeginProcessingLifecycle();
  }
}

void appcodeWindowsEngine::OnViewFocusChangeRequest(
    const appcodeViewFocusChangeRequest* request) {
  std::shared_lock read_lock(views_mutex_);

  auto iterator = views_.find(request->view_id);
  if (iterator == views_.end()) {
    return;
  }

  appcodeWindowsView* view = iterator->second;
  view->Focus();
}

bool appcodeWindowsEngine::Present(const appcodePresentViewInfo* info) {
  // This runs on the raster thread. Lock the views map for the entirety of the
  // present operation to block the platform thread from destroying the
  // view during the present.
  std::shared_lock read_lock(views_mutex_);

  auto iterator = views_.find(info->view_id);
  if (iterator == views_.end()) {
    return false;
  }

  appcodeWindowsView* view = iterator->second;

  return compositor_->Present(view, info->layers, info->layers_count);
}

}  // namespace appcode
