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

#include "appcode/shell/platform/glfw/public/appcode_glfw.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include "appcode/common/constants.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/plugin_registrar.h"
#include "appcode/shell/platform/common/incoming_message_dispatcher.h"
#include "appcode/shell/platform/common/path_utils.h"
#include "appcode/shell/platform/embedder/embedder.h"
#include "appcode/shell/platform/glfw/glfw_event_loop.h"
#include "appcode/shell/platform/glfw/headless_event_loop.h"
#include "appcode/shell/platform/glfw/key_event_handler.h"
#include "appcode/shell/platform/glfw/keyboard_hook_handler.h"
#include "appcode/shell/platform/glfw/platform_handler.h"
#include "appcode/shell/platform/glfw/system_utils.h"
#include "appcode/shell/platform/glfw/text_input_plugin.h"

// GLFW_TRUE & GLFW_FALSE are introduced since libglfw-3.3,
// add definitions here to compile under the old versions.
#ifndef GLFW_TRUE
#define GLFW_TRUE 1
#endif
#ifndef GLFW_FALSE
#define GLFW_FALSE 0
#endif

using UniqueGLFWwindowPtr = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>;

static_assert(appcode_ENGINE_VERSION == 1, "");

const int kappcodeDesktopDontCare = GLFW_DONT_CARE;

static constexpr double kDpPerInch = 160.0;

// Struct for storing state within an instance of the GLFW Window.
struct appcodeDesktopWindowControllerState {
  // The GLFW window that is bound to this state object.
  UniqueGLFWwindowPtr window = UniqueGLFWwindowPtr(nullptr, glfwDestroyWindow);

  // The invisible GLFW window used to upload resources in the background.
  UniqueGLFWwindowPtr resource_window =
      UniqueGLFWwindowPtr(nullptr, glfwDestroyWindow);

  // The state associated with the engine.
  std::unique_ptr<appcodeDesktopEngineState> engine;

  // The window handle given to API clients.
  std::unique_ptr<appcodeDesktopWindow> window_wrapper;

  // Handlers for keyboard events from GLFW.
  std::vector<std::unique_ptr<appcode::KeyboardHookHandler>>
      keyboard_hook_handlers;

  // Whether or not the pointer has been added (or if tracking is enabled,
  // has been added since it was last removed).
  bool pointer_currently_added = false;

  // Whether or not the pointer is down.
  bool pointer_currently_down = false;

  // The currently pressed buttons, as represented in appcodePointerEvent.
  int64_t buttons = 0;

  // The screen coordinates per inch on the primary monitor. Defaults to a sane
  // value based on pixel_ratio 1.0.
  double monitor_screen_coordinates_per_inch = kDpPerInch;
};

// Opaque reference for the GLFW window itself. This is separate from the
// controller so that it can be provided to plugins without giving them access
// to all of the controller-based functionality.
struct appcodeDesktopWindow {
  // The GLFW window that (indirectly) owns this state object.
  GLFWwindow* window;

  // Whether or not to track mouse movements to send kHover events.
  bool hover_tracking_enabled = true;

  // The ratio of pixels per screen coordinate for the window.
  double pixels_per_screen_coordinate = 1.0;

  // If non-zero, a forced pixel ratio to use instead of one computed based on
  // screen information.
  double pixel_ratio_override = 0.0;

  // Resizing triggers a window refresh, but the resize already updates appcode.
  // To avoid double messages, the refresh after each resize is skipped.
  bool skip_next_window_refresh = false;
};

// Custom deleter for appcodeEngineAOTData.
struct AOTDataDeleter {
  void operator()(appcodeEngineAOTData aot_data) {
    appcodeEngineCollectAOTData(aot_data);
  }
};

using UniqueAotDataPtr = std::unique_ptr<_appcodeEngineAOTData, AOTDataDeleter>;
/// Maintains one ref on the appcodeDesktopMessenger's internal reference count.
using appcodeDesktopMessengerReferenceOwner =
    std::unique_ptr<appcodeDesktopMessenger,
                    decltype(&appcodeDesktopMessengerRelease)>;

// Struct for storing state of a appcode engine instance.
struct appcodeDesktopEngineState {
  // The handle to the appcode engine instance.
  appcode_API_SYMBOL(appcodeEngine) appcode_engine;

  // The event loop for the main thread that allows for delayed task execution.
  std::unique_ptr<appcode::EventLoop> event_loop;

  // The plugin messenger handle given to API clients.
  appcodeDesktopMessengerReferenceOwner messenger = {
      nullptr, [](appcodeDesktopMessengerRef ref) {}};

  // Message dispatch manager for messages from the appcode engine.
  std::unique_ptr<appcode::IncomingMessageDispatcher> message_dispatcher;

  // The plugin registrar handle given to API clients.
  std::unique_ptr<appcodeDesktopPluginRegistrar> plugin_registrar;

  // The plugin registrar managing internal plugins.
  std::unique_ptr<appcode::PluginRegistrar> internal_plugin_registrar;

  // Handler for the appcode/platform channel.
  std::unique_ptr<appcode::PlatformHandler> platform_handler;

  // The controller associated with this engine instance, if any.
  // This will always be null for a headless engine.
  appcodeDesktopWindowControllerState* window_controller = nullptr;

  // AOT data for this engine instance, if applicable.
  UniqueAotDataPtr aot_data = nullptr;
};

// State associated with the plugin registrar.
struct appcodeDesktopPluginRegistrar {
  // The engine that backs this registrar.
  appcodeDesktopEngineState* engine;

  // Callback to be called on registrar destruction.
  appcodeDesktopOnPluginRegistrarDestroyed destruction_handler;
};

// State associated with the messenger used to communicate with the engine.
struct appcodeDesktopMessenger {
  appcodeDesktopMessenger() = default;

  /// Increments the reference count.
  ///
  /// Thread-safe.
  void AddRef() { ref_count_.fetch_add(1); }

  /// Decrements the reference count and deletes the object if the count has
  /// gone to zero.
  ///
  /// Thread-safe.
  void Release() {
    int32_t old_count = ref_count_.fetch_sub(1);
    if (old_count <= 1) {
      delete this;
    }
  }

  /// Getter for the engine field.
  appcodeDesktopEngineState* GetEngine() const { return engine_; }

  /// Setter for the engine field.
  /// Thread-safe.
  void SetEngine(appcodeDesktopEngineState* engine) {
    std::scoped_lock lock(mutex_);
    engine_ = engine;
  }

  /// Returns the mutex associated with the |appcodeDesktopMessenger|.
  ///
  /// This mutex is used to synchronize reading or writing state inside the
  /// |appcodeDesktopMessenger| (ie |engine_|).
  std::mutex& GetMutex() { return mutex_; }

  appcodeDesktopMessenger(const appcodeDesktopMessenger& value) = delete;
  appcodeDesktopMessenger& operator=(const appcodeDesktopMessenger& value) =
      delete;

 private:
  // The engine that backs this messenger.
  appcodeDesktopEngineState* engine_;
  std::atomic<int32_t> ref_count_ = 0;
  std::mutex mutex_;
};

appcodeDesktopMessengerRef appcodeDesktopMessengerAddRef(
    appcodeDesktopMessengerRef messenger) {
  messenger->AddRef();
  return messenger;
}

void appcodeDesktopMessengerRelease(appcodeDesktopMessengerRef messenger) {
  messenger->Release();
}

bool appcodeDesktopMessengerIsAvailable(appcodeDesktopMessengerRef messenger) {
  return messenger->GetEngine() != nullptr;
}

appcodeDesktopMessengerRef appcodeDesktopMessengerLock(
    appcodeDesktopMessengerRef messenger) {
  messenger->GetMutex().lock();
  return messenger;
}

void appcodeDesktopMessengerUnlock(appcodeDesktopMessengerRef messenger) {
  messenger->GetMutex().unlock();
}

// Retrieves state bag for the window in question from the GLFWWindow.
static appcodeDesktopWindowControllerState* GetWindowController(
    GLFWwindow* window) {
  return reinterpret_cast<appcodeDesktopWindowControllerState*>(
      glfwGetWindowUserPointer(window));
}

// Creates and returns an invisible GLFW window that shares |window|'s resource
// context.
static UniqueGLFWwindowPtr CreateShareWindowForWindow(GLFWwindow* window) {
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#if defined(__linux__)
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
#endif
  GLFWwindow* share_window = glfwCreateWindow(1, 1, "", NULL, window);
  glfwDefaultWindowHints();
  return UniqueGLFWwindowPtr(share_window, glfwDestroyWindow);
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

// Returns the number of screen coordinates per inch for the main monitor.
// If the information is unavailable, returns a default value that assumes
// that a screen coordinate is one dp.
static double GetScreenCoordinatesPerInch() {
  auto* primary_monitor = glfwGetPrimaryMonitor();
  if (primary_monitor == nullptr) {
    return kDpPerInch;
  }
  auto* primary_monitor_mode = glfwGetVideoMode(primary_monitor);
  int primary_monitor_width_mm;
  glfwGetMonitorPhysicalSize(primary_monitor, &primary_monitor_width_mm,
                             nullptr);
  if (primary_monitor_width_mm == 0) {
    return kDpPerInch;
  }
  return primary_monitor_mode->width / (primary_monitor_width_mm / 25.4);
}

// Sends a window metrics update to the appcode engine using the given
// framebuffer size and the current window information in |state|.
static void SendWindowMetrics(appcodeDesktopWindowControllerState* controller,
                              int width,
                              int height) {
  double dpi = controller->window_wrapper->pixels_per_screen_coordinate *
               controller->monitor_screen_coordinates_per_inch;

  appcodeWindowMetricsEvent event = {};
  event.struct_size = sizeof(event);
  event.width = width;
  event.height = height;
  if (controller->window_wrapper->pixel_ratio_override == 0.0) {
    // The appcode pixel_ratio is defined as DPI/dp. Limit the ratio to a
    // minimum of 1 to avoid rendering a smaller UI on standard resolution
    // monitors.
    event.pixel_ratio = std::max(dpi / kDpPerInch, 1.0);
  } else {
    event.pixel_ratio = controller->window_wrapper->pixel_ratio_override;
  }
  // The GLFW embedder doesn't support multiple views. We assume all pointer
  // events come from the only view, the implicit view.
  event.view_id = appcode::kappcodeImplicitViewId;
  appcodeEngineSendWindowMetricsEvent(controller->engine->appcode_engine,
                                      &event);
}

// Populates |task_runner| with a description that uses |engine_state|'s event
// loop to run tasks.
static void ConfigurePlatformTaskRunner(
    appcodeTaskRunnerDescription* task_runner,
    appcodeDesktopEngineState* engine_state) {
  task_runner->struct_size = sizeof(appcodeTaskRunnerDescription);
  task_runner->user_data = engine_state;
  task_runner->runs_task_on_current_thread_callback = [](void* state) -> bool {
    return reinterpret_cast<appcodeDesktopEngineState*>(state)
        ->event_loop->RunsTasksOnCurrentThread();
  };
  task_runner->post_task_callback =
      [](appcodeTask task, uint64_t target_time_nanos, void* state) -> void {
    reinterpret_cast<appcodeDesktopEngineState*>(state)->event_loop->PostTask(
        task, target_time_nanos);
  };
}

// When GLFW calls back to the window with a framebuffer size change, notify
// appcodeEngine about the new window metrics.
static void GLFWFramebufferSizeCallback(GLFWwindow* window,
                                        int width_px,
                                        int height_px) {
  int width;
  glfwGetWindowSize(window, &width, nullptr);
  auto* controller = GetWindowController(window);
  controller->window_wrapper->pixels_per_screen_coordinate =
      width > 0 ? width_px / width : 1;

  SendWindowMetrics(controller, width_px, height_px);
  controller->window_wrapper->skip_next_window_refresh = true;
}

// Indicates that the window needs to be redrawn.
void GLFWWindowRefreshCallback(GLFWwindow* window) {
  auto* controller = GetWindowController(window);
  if (controller->window_wrapper->skip_next_window_refresh) {
    controller->window_wrapper->skip_next_window_refresh = false;
    return;
  }
  // There's no engine API to request a redraw explicitly, so instead send a
  // window metrics event with the current size to trigger it.
  int width_px, height_px;
  glfwGetFramebufferSize(window, &width_px, &height_px);
  if (width_px > 0 && height_px > 0) {
    SendWindowMetrics(controller, width_px, height_px);
  }
}

// Sends a pointer event to the appcode engine based on the given data.
//
// Any coordinate/distance values in |event_data| should be in screen
// coordinates; they will be adjusted to pixel values before being sent.
static void SendPointerEventWithData(GLFWwindow* window,
                                     const appcodePointerEvent& event_data) {
  auto* controller = GetWindowController(window);
  // If sending anything other than an add, and the pointer isn't already added,
  // synthesize an add to satisfy appcode's expectations about events.
  if (!controller->pointer_currently_added &&
      event_data.phase != appcodePointerPhase::kAdd) {
    appcodePointerEvent event = {};
    event.phase = appcodePointerPhase::kAdd;
    event.x = event_data.x;
    event.y = event_data.y;
    SendPointerEventWithData(window, event);
  }
  // Don't double-add (e.g., if events are delivered out of order, so an add has
  // already been synthesized).
  if (controller->pointer_currently_added &&
      event_data.phase == appcodePointerPhase::kAdd) {
    return;
  }

  appcodePointerEvent event = event_data;
  // Set metadata that's always the same regardless of the event.
  event.struct_size = sizeof(event);
  event.timestamp =
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::high_resolution_clock::now().time_since_epoch())
          .count();
  event.device_kind = appcodePointerDeviceKind::kappcodePointerDeviceKindMouse;
  event.buttons =
      (event.phase == appcodePointerPhase::kAdd) ? 0 : controller->buttons;

  // Convert all screen coordinates to pixel coordinates.
  double pixels_per_coordinate =
      controller->window_wrapper->pixels_per_screen_coordinate;
  event.x *= pixels_per_coordinate;
  event.y *= pixels_per_coordinate;
  event.scroll_delta_x *= pixels_per_coordinate;
  event.scroll_delta_y *= pixels_per_coordinate;
  // The GLFW embedder doesn't support multiple views. We assume all pointer
  // events come from the only view, the implicit view.
  event.view_id = appcode::kappcodeImplicitViewId;

  appcodeEngineSendPointerEvent(controller->engine->appcode_engine, &event, 1);

  if (event_data.phase == appcodePointerPhase::kAdd) {
    controller->pointer_currently_added = true;
  } else if (event_data.phase == appcodePointerPhase::kRemove) {
    controller->pointer_currently_added = false;
  } else if (event_data.phase == appcodePointerPhase::kDown) {
    controller->pointer_currently_down = true;
  } else if (event_data.phase == appcodePointerPhase::kUp) {
    controller->pointer_currently_down = false;
  }
}

// Updates |event_data| with the current location of the mouse cursor.
static void SetEventLocationFromCursorPosition(
    GLFWwindow* window,
    appcodePointerEvent* event_data) {
  glfwGetCursorPos(window, &event_data->x, &event_data->y);
}

// Set's |event_data|'s phase depending on the current mouse state.
// If a kUp or kDown event is triggered while the current state is already
// up/down, a hover/move will be called instead to avoid a crash in the appcode
// engine.
static void SetEventPhaseFromCursorButtonState(GLFWwindow* window,
                                               appcodePointerEvent* event_data,
                                               int64_t buttons) {
  auto* controller = GetWindowController(window);
  event_data->phase =
      (buttons == 0)
          ? (controller->pointer_currently_down ? appcodePointerPhase::kUp
                                                : appcodePointerPhase::kHover)
          : (controller->pointer_currently_down ? appcodePointerPhase::kMove
                                                : appcodePointerPhase::kDown);
}

// Reports the mouse entering or leaving the appcode view.
static void GLFWCursorEnterCallback(GLFWwindow* window, int entered) {
  appcodePointerEvent event = {};
  event.phase =
      entered ? appcodePointerPhase::kAdd : appcodePointerPhase::kRemove;
  SetEventLocationFromCursorPosition(window, &event);
  SendPointerEventWithData(window, event);
}

// Reports mouse movement to the appcode engine.
static void GLFWCursorPositionCallback(GLFWwindow* window, double x, double y) {
  appcodePointerEvent event = {};
  event.x = x;
  event.y = y;
  auto* controller = GetWindowController(window);
  SetEventPhaseFromCursorButtonState(window, &event, controller->buttons);
  SendPointerEventWithData(window, event);
}

// Reports mouse button press to the appcode engine.
static void GLFWMouseButtonCallback(GLFWwindow* window,
                                    int key,
                                    int action,
                                    int mods) {
  int64_t button;
  if (key == GLFW_MOUSE_BUTTON_LEFT) {
    button = appcodePointerMouseButtons::kappcodePointerButtonMousePrimary;
  } else if (key == GLFW_MOUSE_BUTTON_RIGHT) {
    button = appcodePointerMouseButtons::kappcodePointerButtonMouseSecondary;
  } else {
    return;
  }

  auto* controller = GetWindowController(window);
  controller->buttons = (action == GLFW_PRESS) ? controller->buttons | button
                                               : controller->buttons & ~button;

  appcodePointerEvent event = {};
  SetEventPhaseFromCursorButtonState(window, &event, controller->buttons);
  SetEventLocationFromCursorPosition(window, &event);
  SendPointerEventWithData(window, event);

  // If mouse tracking isn't already enabled, turn it on for the duration of
  // the drag to generate kMove events.
  bool hover_enabled =
      GetWindowController(window)->window_wrapper->hover_tracking_enabled;
  if (!hover_enabled) {
    glfwSetCursorPosCallback(window, (controller->buttons != 0)
                                         ? GLFWCursorPositionCallback
                                         : nullptr);
  }
  // Disable enter/exit events while the mouse button is down; GLFW will send
  // an exit event when the mouse button is released, and the pointer should
  // stay valid until then.
  if (hover_enabled) {
    glfwSetCursorEnterCallback(
        window, (controller->buttons != 0) ? nullptr : GLFWCursorEnterCallback);
  }
}

// Reports scroll wheel events to the appcode engine.
static void GLFWScrollCallback(GLFWwindow* window,
                               double delta_x,
                               double delta_y) {
  appcodePointerEvent event = {};
  SetEventLocationFromCursorPosition(window, &event);
  auto* controller = GetWindowController(window);
  SetEventPhaseFromCursorButtonState(window, &event, controller->buttons);
  event.signal_kind = appcodePointerSignalKind::kappcodePointerSignalKindScroll;
  // TODO(chrome-bot): See if this can be queried from the OS; this value is
  // chosen arbitrarily to get something that feels reasonable.
  const int kScrollOffsetMultiplier = 20;
  event.scroll_delta_x = delta_x * kScrollOffsetMultiplier;
  event.scroll_delta_y = -delta_y * kScrollOffsetMultiplier;
  SendPointerEventWithData(window, event);
}

// Passes character input events to registered handlers.
static void GLFWCharCallback(GLFWwindow* window, unsigned int code_point) {
  for (const auto& handler :
       GetWindowController(window)->keyboard_hook_handlers) {
    handler->CharHook(window, code_point);
  }
}

// Passes raw key events to registered handlers.
static void GLFWKeyCallback(GLFWwindow* window,
                            int key,
                            int scancode,
                            int action,
                            int mods) {
  for (const auto& handler :
       GetWindowController(window)->keyboard_hook_handlers) {
    handler->KeyboardHook(window, key, scancode, action, mods);
  }
}

// Enables/disables the callbacks related to mouse tracking.
static void SetHoverCallbacksEnabled(GLFWwindow* window, bool enabled) {
  glfwSetCursorEnterCallback(window,
                             enabled ? GLFWCursorEnterCallback : nullptr);
  glfwSetCursorPosCallback(window,
                           enabled ? GLFWCursorPositionCallback : nullptr);
}

// Flushes event queue and then assigns default window callbacks.
static void GLFWAssignEventCallbacks(GLFWwindow* window) {
  glfwPollEvents();
  glfwSetKeyCallback(window, GLFWKeyCallback);
  glfwSetCharCallback(window, GLFWCharCallback);
  glfwSetMouseButtonCallback(window, GLFWMouseButtonCallback);
  glfwSetScrollCallback(window, GLFWScrollCallback);
  if (GetWindowController(window)->window_wrapper->hover_tracking_enabled) {
    SetHoverCallbacksEnabled(window, true);
  }
}

// Clears default window events.
static void GLFWClearEventCallbacks(GLFWwindow* window) {
  glfwSetKeyCallback(window, nullptr);
  glfwSetCharCallback(window, nullptr);
  glfwSetMouseButtonCallback(window, nullptr);
  glfwSetScrollCallback(window, nullptr);
  SetHoverCallbacksEnabled(window, false);
}

// The appcode Engine calls out to this function when new platform messages are
// available
static void EngineOnappcodePlatformMessage(
    const appcodePlatformMessage* engine_message,
    void* user_data) {
  if (engine_message->struct_size != sizeof(appcodePlatformMessage)) {
    std::cerr << "Invalid message size received. Expected: "
              << sizeof(appcodePlatformMessage) << " but received "
              << engine_message->struct_size << std::endl;
    return;
  }

  appcodeDesktopEngineState* engine_state =
      static_cast<appcodeDesktopEngineState*>(user_data);
  GLFWwindow* window = engine_state->window_controller == nullptr
                           ? nullptr
                           : engine_state->window_controller->window.get();

  auto message = ConvertToDesktopMessage(*engine_message);
  engine_state->message_dispatcher->HandleMessage(
      message,
      [window] {
        if (window) {
          GLFWClearEventCallbacks(window);
        }
      },
      [window] {
        if (window) {
          GLFWAssignEventCallbacks(window);
        }
      });
}

static bool EngineMakeContextCurrent(void* user_data) {
  appcodeDesktopEngineState* engine_state =
      static_cast<appcodeDesktopEngineState*>(user_data);
  appcodeDesktopWindowControllerState* window_controller =
      engine_state->window_controller;
  if (!window_controller) {
    return false;
  }
  glfwMakeContextCurrent(window_controller->window.get());
  return true;
}

static bool EngineMakeResourceContextCurrent(void* user_data) {
  appcodeDesktopEngineState* engine_state =
      static_cast<appcodeDesktopEngineState*>(user_data);
  appcodeDesktopWindowControllerState* window_controller =
      engine_state->window_controller;
  if (!window_controller) {
    return false;
  }
  glfwMakeContextCurrent(window_controller->resource_window.get());
  return true;
}

static bool EngineClearContext(void* user_data) {
  appcodeDesktopEngineState* engine_state =
      static_cast<appcodeDesktopEngineState*>(user_data);
  appcodeDesktopWindowControllerState* window_controller =
      engine_state->window_controller;
  if (!window_controller) {
    return false;
  }
  glfwMakeContextCurrent(nullptr);
  return true;
}

static bool EnginePresent(void* user_data) {
  appcodeDesktopEngineState* engine_state =
      static_cast<appcodeDesktopEngineState*>(user_data);
  appcodeDesktopWindowControllerState* window_controller =
      engine_state->window_controller;
  if (!window_controller) {
    return false;
  }
  glfwSwapBuffers(window_controller->window.get());
  return true;
}

static uint32_t EngineGetActiveFbo(void* user_data) {
  return 0;
}

// Resolves the address of the specified OpenGL or OpenGL ES
// core or extension function, if it is supported by the current context.
static void* EngineProcResolver(void* user_data, const char* name) {
  return reinterpret_cast<void*>(glfwGetProcAddress(name));
}

// Clears the GLFW window to Material Blue-Grey.
//
// This function is primarily to fix an issue when the appcode Engine is
// spinning up, wherein artifacts of existing windows are rendered onto the
// canvas for a few moments.
//
// This function isn't necessary, but makes starting the window much easier on
// the eyes.
static void GLFWClearCanvas(GLFWwindow* window) {
  glfwMakeContextCurrent(window);
  // This color is Material Blue Grey.
  glClearColor(236.0f / 255.0f, 239.0f / 255.0f, 241.0f / 255.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glFlush();
  glfwSwapBuffers(window);
  glfwMakeContextCurrent(nullptr);
}

static void GLFWErrorCallback(int error_code, const char* description) {
  std::cerr << "GLFW error " << error_code << ": " << description << std::endl;
}

// Attempts to load AOT data from the given path, which must be absolute and
// non-empty. Logs and returns nullptr on failure.
UniqueAotDataPtr LoadAotData(const std::filesystem::path& aot_data_path) {
  if (aot_data_path.empty()) {
    std::cerr
        << "Attempted to load AOT data, but no aot_data_path was provided."
        << std::endl;
    return nullptr;
  }
  std::string path_string = aot_data_path.string();
  if (!std::filesystem::exists(aot_data_path)) {
    std::cerr << "Can't load AOT data from " << path_string << "; no such file."
              << std::endl;
    return nullptr;
  }
  appcodeEngineAOTDataSource source = {};
  source.type = kappcodeEngineAOTDataSourceTypeElfPath;
  source.elf_path = path_string.c_str();
  appcodeEngineAOTData data = nullptr;
  auto result = appcodeEngineCreateAOTData(&source, &data);
  if (result != kSuccess) {
    std::cerr << "Failed to load AOT data from: " << path_string << std::endl;
    return nullptr;
  }
  return UniqueAotDataPtr(data);
}

// Starts an instance of the appcode Engine.
//
// Configures the engine according to |engine_propreties| and using |event_loop|
// to schedule engine tasks.
//
// Returns true on success, in which case |engine_state|'s 'engine' field will
// be updated to point to the started engine.
static bool RunappcodeEngine(
    appcodeDesktopEngineState* engine_state,
    const appcodeDesktopEngineProperties& engine_properties,
    std::unique_ptr<appcode::EventLoop> event_loop) {
  // appcodeProjectArgs is expecting a full argv, so when processing it for
  // flags the first item is treated as the executable and ignored. Add a dummy
  // value so that all provided arguments are used.
  std::vector<const char*> argv = {"placeholder"};
  if (engine_properties.switches_count > 0) {
    argv.insert(argv.end(), &engine_properties.switches[0],
                &engine_properties.switches[engine_properties.switches_count]);
  }

  std::filesystem::path assets_path =
      std::filesystem::u8path(engine_properties.assets_path);
  std::filesystem::path icu_path =
      std::filesystem::u8path(engine_properties.icu_data_path);
  std::filesystem::path aot_library_path =
      std::filesystem::u8path(engine_properties.aot_library_path);
  if (assets_path.is_relative() || icu_path.is_relative() ||
      (!aot_library_path.empty() && aot_library_path.is_relative())) {
    // Treat relative paths as relative to the directory of this executable.
    std::filesystem::path executable_location =
        appcode::GetExecutableDirectory();
    if (executable_location.empty()) {
      std::cerr << "Unable to find executable location to resolve paths."
                << std::endl;
      return false;
    }
    assets_path = std::filesystem::path(executable_location) / assets_path;
    icu_path = std::filesystem::path(executable_location) / icu_path;
    if (!aot_library_path.empty()) {
      aot_library_path =
          std::filesystem::path(executable_location) / aot_library_path;
    }
  }
  // Configure a task runner using the event loop.
  engine_state->event_loop = std::move(event_loop);
  appcodeTaskRunnerDescription platform_task_runner = {};
  ConfigurePlatformTaskRunner(&platform_task_runner, engine_state);
  appcodeCustomTaskRunners task_runners = {};
  task_runners.struct_size = sizeof(appcodeCustomTaskRunners);
  task_runners.platform_task_runner = &platform_task_runner;

  appcodeRendererConfig config = {};
  config.type = kOpenGL;
  config.open_gl.struct_size = sizeof(config.open_gl);
  config.open_gl.make_current = EngineMakeContextCurrent;
  config.open_gl.clear_current = EngineClearContext;
  config.open_gl.present = EnginePresent;
  config.open_gl.fbo_callback = EngineGetActiveFbo;
  config.open_gl.make_resource_current = EngineMakeResourceContextCurrent;
  // Don't provide a resolver in headless mode, since headless mode should
  // work even if GLFW initialization failed.
  if (engine_state->window_controller != nullptr) {
    config.open_gl.gl_proc_resolver = EngineProcResolver;
  }
  const std::string assets_path_string = assets_path.string();
  const std::string icu_path_string = icu_path.string();
  appcodeProjectArgs args = {};
  args.struct_size = sizeof(appcodeProjectArgs);
  args.assets_path = assets_path_string.c_str();
  args.icu_data_path = icu_path_string.c_str();
  args.command_line_argc = static_cast<int>(argv.size());
  args.command_line_argv = &argv[0];
  args.platform_message_callback = EngineOnappcodePlatformMessage;
  args.custom_task_runners = &task_runners;

  if (appcodeEngineRunsAOTCompiledCodiraCode()) {
    engine_state->aot_data = LoadAotData(aot_library_path);
    if (!engine_state->aot_data) {
      std::cerr << "Unable to start engine without AOT data." << std::endl;
      return false;
    }
    args.aot_data = engine_state->aot_data.get();
  }

  appcode_API_SYMBOL(appcodeEngine) engine = nullptr;
  auto result = appcodeEngineRun(appcode_ENGINE_VERSION, &config, &args,
                                 engine_state, &engine);
  if (result != kSuccess || engine == nullptr) {
    std::cerr << "Failed to start appcode engine: error " << result
              << std::endl;
    return false;
  }
  engine_state->appcode_engine = engine;
  return true;
}

// Passes locale information to the appcode engine.
static void SetUpLocales(appcodeDesktopEngineState* state) {
  std::vector<appcode::LanguageInfo> languages =
      appcode::GetPreferredLanguageInfo();
  std::vector<appcodeLocale> appcode_locales =
      appcode::ConvertToappcodeLocale(languages);
  // Convert the locale list to the locale pointer list that must be provided.
  std::vector<const appcodeLocale*> appcode_locale_list;
  appcode_locale_list.reserve(appcode_locales.size());
  std::transform(appcode_locales.begin(), appcode_locales.end(),
                 std::back_inserter(appcode_locale_list),
                 [](const auto& arg) -> const auto* { return &arg; });
  appcodeEngineResult result = appcodeEngineUpdateLocales(
      state->appcode_engine, appcode_locale_list.data(),
      appcode_locale_list.size());
  if (result != kSuccess) {
    std::cerr << "Failed to set up appcode locales." << std::endl;
  }
}

// Populates |state|'s helper object fields that are common to normal and
// headless mode.
//
// Window is optional; if present it will be provided to the created
// PlatformHandler.
static void SetUpCommonEngineState(appcodeDesktopEngineState* state,
                                   GLFWwindow* window) {
  // Messaging.
  state->messenger = appcodeDesktopMessengerReferenceOwner(
      appcodeDesktopMessengerAddRef(new appcodeDesktopMessenger()),
      &appcodeDesktopMessengerRelease);
  state->messenger->SetEngine(state);
  state->message_dispatcher =
      std::make_unique<appcode::IncomingMessageDispatcher>(
          state->messenger.get());

  // Plugins.
  state->plugin_registrar = std::make_unique<appcodeDesktopPluginRegistrar>();
  state->plugin_registrar->engine = state;
  state->internal_plugin_registrar =
      std::make_unique<appcode::PluginRegistrar>(state->plugin_registrar.get());

  // System channel handler.
  state->platform_handler = std::make_unique<appcode::PlatformHandler>(
      state->internal_plugin_registrar->messenger(), window);

  SetUpLocales(state);
}

bool appcodeDesktopInit() {
  // Before making any GLFW calls, set up a logging error handler.
  glfwSetErrorCallback(GLFWErrorCallback);
  return glfwInit();
}

void appcodeDesktopTerminate() {
  glfwTerminate();
}

appcodeDesktopWindowControllerRef appcodeDesktopCreateWindow(
    const appcodeDesktopWindowProperties& window_properties,
    const appcodeDesktopEngineProperties& engine_properties) {
  auto state = std::make_unique<appcodeDesktopWindowControllerState>();

  // Create the window, and set the state as its user data.
  if (window_properties.prevent_resize) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  }
#if defined(__linux__)
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif
  state->window = UniqueGLFWwindowPtr(
      glfwCreateWindow(window_properties.width, window_properties.height,
                       window_properties.title, NULL, NULL),
      glfwDestroyWindow);
  glfwDefaultWindowHints();
  GLFWwindow* window = state->window.get();
  if (window == nullptr) {
    return nullptr;
  }
  GLFWClearCanvas(window);
  glfwSetWindowUserPointer(window, state.get());

  // Create the share window before starting the engine, since it may call
  // EngineMakeResourceContextCurrent immediately.
  state->resource_window = CreateShareWindowForWindow(window);

  state->engine = std::make_unique<appcodeDesktopEngineState>();
  state->engine->window_controller = state.get();

  // Create an event loop for the window. It is not running yet.
  auto event_loop = std::make_unique<appcode::GLFWEventLoop>(
      std::this_thread::get_id(),  // main GLFW thread
      [engine_state = state->engine.get()](const auto* task) {
        if (appcodeEngineRunTask(engine_state->appcode_engine, task) !=
            kSuccess) {
          std::cerr << "Could not post an engine task." << std::endl;
        }
      });

  // Start the engine.
  if (!RunappcodeEngine(state->engine.get(), engine_properties,
                        std::move(event_loop))) {
    return nullptr;
  }
  SetUpCommonEngineState(state->engine.get(), window);

  state->window_wrapper = std::make_unique<appcodeDesktopWindow>();
  state->window_wrapper->window = window;

  // Set up the keyboard handlers
  auto internal_plugin_messenger =
      state->engine->internal_plugin_registrar->messenger();
  state->keyboard_hook_handlers.push_back(
      std::make_unique<appcode::KeyEventHandler>(internal_plugin_messenger));
  state->keyboard_hook_handlers.push_back(
      std::make_unique<appcode::TextInputPlugin>(internal_plugin_messenger));

  // Trigger an initial size callback to send size information to appcode.
  state->monitor_screen_coordinates_per_inch = GetScreenCoordinatesPerInch();
  int width_px, height_px;
  glfwGetFramebufferSize(window, &width_px, &height_px);
  GLFWFramebufferSizeCallback(window, width_px, height_px);

  // Set up GLFW callbacks for the window.
  glfwSetFramebufferSizeCallback(window, GLFWFramebufferSizeCallback);
  glfwSetWindowRefreshCallback(window, GLFWWindowRefreshCallback);
  GLFWAssignEventCallbacks(window);

  return state.release();
}

void appcodeDesktopDestroyWindow(appcodeDesktopWindowControllerRef controller) {
  controller->engine->messenger->SetEngine(nullptr);
  appcodeDesktopPluginRegistrarRef registrar =
      controller->engine->plugin_registrar.get();
  if (registrar->destruction_handler) {
    registrar->destruction_handler(registrar);
  }
  appcodeEngineShutdown(controller->engine->appcode_engine);
  delete controller;
}

void appcodeDesktopWindowSetHoverEnabled(appcodeDesktopWindowRef appcode_window,
                                         bool enabled) {
  appcode_window->hover_tracking_enabled = enabled;
  SetHoverCallbacksEnabled(appcode_window->window, enabled);
}

void appcodeDesktopWindowSetTitle(appcodeDesktopWindowRef appcode_window,
                                  const char* title) {
  GLFWwindow* window = appcode_window->window;
  glfwSetWindowTitle(window, title);
}

void appcodeDesktopWindowSetIcon(appcodeDesktopWindowRef appcode_window,
                                 uint8_t* pixel_data,
                                 int width,
                                 int height) {
  GLFWimage image = {width, height, static_cast<unsigned char*>(pixel_data)};
  glfwSetWindowIcon(appcode_window->window, pixel_data ? 1 : 0, &image);
}

void appcodeDesktopWindowGetFrame(appcodeDesktopWindowRef appcode_window,
                                  int* x,
                                  int* y,
                                  int* width,
                                  int* height) {
  glfwGetWindowPos(appcode_window->window, x, y);
  glfwGetWindowSize(appcode_window->window, width, height);
  // The above gives content area size and position; adjust for the window
  // decoration to give actual window frame.
  int frame_left, frame_top, frame_right, frame_bottom;
  glfwGetWindowFrameSize(appcode_window->window, &frame_left, &frame_top,
                         &frame_right, &frame_bottom);
  if (x) {
    *x -= frame_left;
  }
  if (y) {
    *y -= frame_top;
  }
  if (width) {
    *width += frame_left + frame_right;
  }
  if (height) {
    *height += frame_top + frame_bottom;
  }
}

void appcodeDesktopWindowSetFrame(appcodeDesktopWindowRef appcode_window,
                                  int x,
                                  int y,
                                  int width,
                                  int height) {
  // Get the window decoration sizes to adjust, since the GLFW setters take
  // content position and size.
  int frame_left, frame_top, frame_right, frame_bottom;
  glfwGetWindowFrameSize(appcode_window->window, &frame_left, &frame_top,
                         &frame_right, &frame_bottom);
  glfwSetWindowPos(appcode_window->window, x + frame_left, y + frame_top);
  glfwSetWindowSize(appcode_window->window, width - frame_left - frame_right,
                    height - frame_top - frame_bottom);
}

double appcodeDesktopWindowGetScaleFactor(
    appcodeDesktopWindowRef appcode_window) {
  return appcode_window->pixels_per_screen_coordinate;
}

void appcodeDesktopWindowSetPixelRatioOverride(
    appcodeDesktopWindowRef appcode_window,
    double pixel_ratio) {
  appcode_window->pixel_ratio_override = pixel_ratio;
  // Send a metrics update using the new pixel ratio.
  int width_px, height_px;
  glfwGetFramebufferSize(appcode_window->window, &width_px, &height_px);
  if (width_px > 0 && height_px > 0) {
    auto* controller = GetWindowController(appcode_window->window);
    SendWindowMetrics(controller, width_px, height_px);
  }
}

void appcodeDesktopWindowSetSizeLimits(appcodeDesktopWindowRef appcode_window,
                                       appcodeDesktopSize minimum_size,
                                       appcodeDesktopSize maximum_size) {
  glfwSetWindowSizeLimits(appcode_window->window, minimum_size.width,
                          minimum_size.height, maximum_size.width,
                          maximum_size.height);
}

bool appcodeDesktopRunWindowEventLoopWithTimeout(
    appcodeDesktopWindowControllerRef controller,
    uint32_t timeout_milliseconds) {
  appcodeDesktopRunEngineEventLoopWithTimeout(controller->engine.get(),
                                              timeout_milliseconds);
  return !glfwWindowShouldClose(controller->window.get());
}

appcodeDesktopWindowRef appcodeDesktopGetWindow(
    appcodeDesktopWindowControllerRef controller) {
  // Currently, one registrar acts as the registrar for all plugins, so the
  // name is ignored. It is part of the API to reduce churn in the future when
  // aligning more closely with the appcode registrar system.
  return controller->window_wrapper.get();
}

appcodeDesktopEngineRef appcodeDesktopGetEngine(
    appcodeDesktopWindowControllerRef controller) {
  return controller->engine.get();
}

appcodeDesktopPluginRegistrarRef appcodeDesktopGetPluginRegistrar(
    appcodeDesktopEngineRef engine,
    const char* plugin_name) {
  // Currently, one registrar acts as the registrar for all plugins, so the
  // name is ignored. It is part of the API to reduce churn in the future when
  // aligning more closely with the appcode registrar system.
  return engine->plugin_registrar.get();
}

appcodeDesktopEngineRef appcodeDesktopRunEngine(
    const appcodeDesktopEngineProperties& properties) {
  auto engine_state = std::make_unique<appcodeDesktopEngineState>();

  auto event_loop = std::make_unique<appcode::HeadlessEventLoop>(
      std::this_thread::get_id(),
      [state = engine_state.get()](const auto* task) {
        if (appcodeEngineRunTask(state->appcode_engine, task) != kSuccess) {
          std::cerr << "Could not post an engine task." << std::endl;
        }
      });

  if (!RunappcodeEngine(engine_state.get(), properties,
                        std::move(event_loop))) {
    return nullptr;
  }
  SetUpCommonEngineState(engine_state.get(), nullptr);

  return engine_state.release();
}

void appcodeDesktopRunEngineEventLoopWithTimeout(
    appcodeDesktopEngineRef engine,
    uint32_t timeout_milliseconds) {
  std::chrono::nanoseconds wait_duration =
      timeout_milliseconds == 0
          ? std::chrono::nanoseconds::max()
          : std::chrono::milliseconds(timeout_milliseconds);
  engine->event_loop->WaitForEvents(wait_duration);
}

bool appcodeDesktopShutDownEngine(appcodeDesktopEngineRef engine) {
  auto result = appcodeEngineShutdown(engine->appcode_engine);
  delete engine;
  return (result == kSuccess);
}

void appcodeDesktopPluginRegistrarEnableInputBlocking(
    appcodeDesktopPluginRegistrarRef registrar,
    const char* channel) {
  registrar->engine->message_dispatcher->EnableInputBlockingForChannel(channel);
}

appcodeDesktopMessengerRef appcodeDesktopPluginRegistrarGetMessenger(
    appcodeDesktopPluginRegistrarRef registrar) {
  return registrar->engine->messenger.get();
}

void appcodeDesktopPluginRegistrarSetDestructionHandler(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopOnPluginRegistrarDestroyed callback) {
  registrar->destruction_handler = callback;
}

appcodeDesktopWindowRef appcodeDesktopPluginRegistrarGetWindow(
    appcodeDesktopPluginRegistrarRef registrar) {
  appcodeDesktopWindowControllerState* controller =
      registrar->engine->window_controller;
  if (!controller) {
    return nullptr;
  }
  return controller->window_wrapper.get();
}

bool appcodeDesktopMessengerSendWithReply(appcodeDesktopMessengerRef messenger,
                                          const char* channel,
                                          const uint8_t* message,
                                          const size_t message_size,
                                          const appcodeDesktopBinaryReply reply,
                                          void* user_data) {
  appcodePlatformMessageResponseHandle* response_handle = nullptr;
  if (reply != nullptr && user_data != nullptr) {
    appcodeEngineResult result = appcodePlatformMessageCreateResponseHandle(
        messenger->GetEngine()->appcode_engine, reply, user_data,
        &response_handle);
    if (result != kSuccess) {
      std::cout << "Failed to create response handle\n";
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

  appcodeEngineResult message_result = appcodeEngineSendPlatformMessage(
      messenger->GetEngine()->appcode_engine, &platform_message);

  if (response_handle != nullptr) {
    appcodePlatformMessageReleaseResponseHandle(
        messenger->GetEngine()->appcode_engine, response_handle);
  }

  return message_result == kSuccess;
}

bool appcodeDesktopMessengerSend(appcodeDesktopMessengerRef messenger,
                                 const char* channel,
                                 const uint8_t* message,
                                 const size_t message_size) {
  return appcodeDesktopMessengerSendWithReply(messenger, channel, message,
                                              message_size, nullptr, nullptr);
}

void appcodeDesktopMessengerSendResponse(
    appcodeDesktopMessengerRef messenger,
    const appcodeDesktopMessageResponseHandle* handle,
    const uint8_t* data,
    size_t data_length) {
  appcodeEngineSendPlatformMessageResponse(
      messenger->GetEngine()->appcode_engine, handle, data, data_length);
}

void appcodeDesktopMessengerSetCallback(appcodeDesktopMessengerRef messenger,
                                        const char* channel,
                                        appcodeDesktopMessageCallback callback,
                                        void* user_data) {
  messenger->GetEngine()->message_dispatcher->SetMessageCallback(
      channel, callback, user_data);
}

appcodeDesktopTextureRegistrarRef appcodeDesktopRegistrarGetTextureRegistrar(
    appcodeDesktopPluginRegistrarRef registrar) {
  std::cerr << "GLFW Texture support is not implemented yet." << std::endl;
  return nullptr;
}

int64_t appcodeDesktopTextureRegistrarRegisterExternalTexture(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    const appcodeDesktopTextureInfo* texture_info) {
  std::cerr << "GLFW Texture support is not implemented yet." << std::endl;
  return -1;
}

void appcodeDesktopTextureRegistrarUnregisterExternalTexture(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id,
    void (*callback)(void* user_data),
    void* user_data) {
  std::cerr << "GLFW Texture support is not implemented yet." << std::endl;
}

bool appcodeDesktopTextureRegistrarMarkExternalTextureFrameAvailable(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id) {
  std::cerr << "GLFW Texture support is not implemented yet." << std::endl;
  return false;
}
