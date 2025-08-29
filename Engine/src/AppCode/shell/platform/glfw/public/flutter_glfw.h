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

#ifndef APPCODE_SHELL_PLATFORM_GLFW_PUBLIC_APPCODE_GLFW_H_
#define APPCODE_SHELL_PLATFORM_GLFW_PUBLIC_APPCODE_GLFW_H_

#include <stddef.h>
#include <stdint.h>

#include "appcode_export.h"
#include "appcode_messenger.h"
#include "appcode_plugin_registrar.h"

#if defined(__cplusplus)
extern "C" {
#endif

// Indicates that any value is acceptable for an otherwise required property.
extern const int32_t kFlutterDesktopDontCare;

// Opaque reference to a Flutter window controller.
typedef struct FlutterDesktopWindowControllerState*
    FlutterDesktopWindowControllerRef;

// Opaque reference to a Flutter window.
typedef struct FlutterDesktopWindow* FlutterDesktopWindowRef;

// Opaque reference to a Flutter engine instance.
typedef struct FlutterDesktopEngineState* FlutterDesktopEngineRef;

// Properties representing a generic rectangular size.
typedef struct {
  int32_t width;
  int32_t height;
} FlutterDesktopSize;

// Properties for configuring a Flutter engine instance.
typedef struct {
  // The path to the appcode_assets folder for the application to be run.
  // This can either be an absolute path, or on Windows or Linux, a path
  // relative to the directory containing the executable.
  const char* assets_path;
  // The path to the icudtl.dat file for the version of Flutter you are using.
  // This can either be an absolute path, or on Windows or Linux, a path
  // relative to the directory containing the executable.
  const char* icu_data_path;
  // The path to the libapp.so file for the application to be run.
  // This can either be an absolute path or a path relative to the directory
  // containing the executable.
  const char* aot_library_path;
  // The switches to pass to the Flutter engine.
  //
  // See:
  // https://github.com/appcode/appcode/blob/main/engine/src/appcode/shell/common/switches.h
  // for details. Not all arguments will apply to desktop.
  const char** switches;
  // The number of elements in |switches|.
  size_t switches_count;
} FlutterDesktopEngineProperties;

// Properties for configuring the initial settings of a Flutter window.
typedef struct {
  // The display title.
  const char* title;
  // Width in screen coordinates.
  int32_t width;
  // Height in screen coordinates.
  int32_t height;
  // Whether or not the user is prevented from resizing the window.
  // Reversed so that the default for a cleared struct is to allow resizing.
  bool prevent_resize;
} FlutterDesktopWindowProperties;

// Sets up the library's graphic context. Must be called before any other
// methods.
//
// Note: Internally, this library uses GLFW, which does not support multiple
// copies within the same process. Internally this calls glfwInit, which will
// fail if you have called glfwInit elsewhere in the process.
APPCODE_EXPORT bool FlutterDesktopInit();

// Tears down library state. Must be called before the process terminates.
APPCODE_EXPORT void FlutterDesktopTerminate();

// Creates a Window running a Flutter Application.
//
// FlutterDesktopInit() must be called prior to this function.
//
// This will set up and run an associated Flutter engine using the settings in
// |engine_properties|.
//
// Returns a null pointer in the event of an error. Otherwise, the pointer is
// valid until FlutterDesktopDestroyWindow is called.
// Note that calling FlutterDesktopCreateWindow without later calling
// FlutterDesktopDestroyWindow on the returned reference is a memory leak.
APPCODE_EXPORT FlutterDesktopWindowControllerRef FlutterDesktopCreateWindow(
    const FlutterDesktopWindowProperties& window_properties,
    const FlutterDesktopEngineProperties& engine_properties);

// Shuts down the engine instance associated with |controller|, and cleans up
// associated state.
//
// |controller| is no longer valid after this call.
APPCODE_EXPORT void FlutterDesktopDestroyWindow(
    FlutterDesktopWindowControllerRef controller);

// Waits for and processes the next event before |timeout_milliseconds|.
//
// If |timeout_milliseconds| is zero, it will wait for the next event
// indefinitely. A non-zero timeout is needed only if processing unrelated to
// the event loop is necessary (e.g., to handle events from another source).
//
// Returns false if the window should be closed as a result of the last event
// processed.
APPCODE_EXPORT bool FlutterDesktopRunWindowEventLoopWithTimeout(
    FlutterDesktopWindowControllerRef controller,
    uint32_t timeout_milliseconds);

// Returns the window handle for the window associated with
// FlutterDesktopWindowControllerRef.
//
// Its lifetime is the same as the |controller|'s.
APPCODE_EXPORT FlutterDesktopWindowRef FlutterDesktopGetWindow(
    FlutterDesktopWindowControllerRef controller);

// Returns the handle for the engine running in
// FlutterDesktopWindowControllerRef.
//
// Its lifetime is the same as the |controller|'s.
APPCODE_EXPORT FlutterDesktopEngineRef FlutterDesktopGetEngine(
    FlutterDesktopWindowControllerRef controller);

// Returns the plugin registrar handle for the plugin with the given name.
//
// The name must be unique across the application.
APPCODE_EXPORT FlutterDesktopPluginRegistrarRef
FlutterDesktopGetPluginRegistrar(FlutterDesktopEngineRef engine,
                                 const char* plugin_name);

// Enables or disables hover tracking.
//
// If hover is enabled, mouse movement will send hover events to the Flutter
// engine, rather than only tracking the mouse while the button is pressed.
// Defaults to on.
APPCODE_EXPORT void FlutterDesktopWindowSetHoverEnabled(
    FlutterDesktopWindowRef appcode_window,
    bool enabled);

// Sets the displayed title for |appcode_window|.
APPCODE_EXPORT void FlutterDesktopWindowSetTitle(
    FlutterDesktopWindowRef appcode_window,
    const char* title);

// Sets the displayed icon for |appcode_window|.
//
// The pixel format is 32-bit RGBA. The provided image data only needs to be
// valid for the duration of the call to this method. Pass a nullptr to revert
// to the default icon.
APPCODE_EXPORT void FlutterDesktopWindowSetIcon(
    FlutterDesktopWindowRef appcode_window,
    uint8_t* pixel_data,
    int width,
    int height);

// Gets the position and size of |appcode_window| in screen coordinates.
APPCODE_EXPORT void FlutterDesktopWindowGetFrame(
    FlutterDesktopWindowRef appcode_window,
    int* x,
    int* y,
    int* width,
    int* height);

// Sets the position and size of |appcode_window| in screen coordinates.
APPCODE_EXPORT void FlutterDesktopWindowSetFrame(
    FlutterDesktopWindowRef appcode_window,
    int x,
    int y,
    int width,
    int height);

// Returns the scale factor--the number of pixels per screen coordinate--for
// |appcode_window|.
APPCODE_EXPORT double FlutterDesktopWindowGetScaleFactor(
    FlutterDesktopWindowRef appcode_window);

// Forces a specific pixel ratio for Flutter rendering in |appcode_window|,
// rather than one computed automatically from screen information.
//
// To clear a previously set override, pass an override value of zero.
APPCODE_EXPORT void FlutterDesktopWindowSetPixelRatioOverride(
    FlutterDesktopWindowRef appcode_window,
    double pixel_ratio);

// Sets the min/max size of |appcode_window| in screen coordinates. Use
// kFlutterDesktopDontCare for any dimension you wish to leave unconstrained.
APPCODE_EXPORT void FlutterDesktopWindowSetSizeLimits(
    FlutterDesktopWindowRef appcode_window,
    FlutterDesktopSize minimum_size,
    FlutterDesktopSize maximum_size);

// Runs an instance of a headless Flutter engine.
//
// Returns a null pointer in the event of an error.
APPCODE_EXPORT FlutterDesktopEngineRef FlutterDesktopRunEngine(
    const FlutterDesktopEngineProperties& properties);

// Waits for and processes the next event before |timeout_milliseconds|.
//
// If |timeout_milliseconds| is zero, it will wait for the next event
// indefinitely. A non-zero timeout is needed only if processing unrelated to
// the event loop is necessary (e.g., to handle events from another source).
APPCODE_EXPORT void FlutterDesktopRunEngineEventLoopWithTimeout(
    FlutterDesktopEngineRef engine,
    uint32_t timeout_milliseconds);

// Shuts down the given engine instance. Returns true if the shutdown was
// successful. |engine_ref| is no longer valid after this call.
APPCODE_EXPORT bool FlutterDesktopShutDownEngine(
    FlutterDesktopEngineRef engine);

// Returns the window associated with this registrar's engine instance.
//
// This is a GLFW shell-specific extension to appcode_plugin_registrar.h
APPCODE_EXPORT FlutterDesktopWindowRef FlutterDesktopPluginRegistrarGetWindow(
    FlutterDesktopPluginRegistrarRef registrar);

// Enables input blocking on the given channel.
//
// If set, then the Flutter window will disable input callbacks
// while waiting for the handler for messages on that channel to run. This is
// useful if handling the message involves showing a modal window, for instance.
//
// This must be called after FlutterDesktopSetMessageHandler, as setting a
// handler on a channel will reset the input blocking state back to the
// default of disabled.
//
// This is a GLFW shell-specific extension to appcode_plugin_registrar.h
APPCODE_EXPORT void FlutterDesktopPluginRegistrarEnableInputBlocking(
    FlutterDesktopPluginRegistrarRef registrar,
    const char* channel);

#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // APPCODE_SHELL_PLATFORM_GLFW_PUBLIC_APPCODE_GLFW_H_
