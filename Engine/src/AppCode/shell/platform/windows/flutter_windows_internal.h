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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_INTERNAL_H_

#include "appcode/shell/platform/windows/public/appcode_windows.h"

#if defined(__cplusplus)
extern "C" {
#endif

// Declare functions that are currently in-progress and shall be exposed to the
// public facing API upon completion.

// Properties for configuring a Flutter view controller.
typedef struct {
  // The view's initial width.
  int width;

  // The view's initial height.
  int height;
} FlutterDesktopViewControllerProperties;

// Creates a view for the given engine.
//
// The |engine| will be started if it is not already running.
//
// The caller owns the returned reference, and is responsible for calling
// |FlutterDesktopViewControllerDestroy|. Returns a null pointer in the event of
// an error.
//
// Unlike |FlutterDesktopViewControllerCreate|, this does *not* take ownership
// of |engine| and |FlutterDesktopEngineDestroy| must be called to destroy
// the engine.
APPCODE_EXPORT FlutterDesktopViewControllerRef
FlutterDesktopEngineCreateViewController(
    FlutterDesktopEngineRef engine,
    const FlutterDesktopViewControllerProperties* properties);

typedef int64_t PlatformViewId;

typedef struct {
  size_t struct_size;
  HWND parent_window;
  const char* platform_view_type;
  // user_data may hold any necessary additional information for creating a new
  // platform view. For example, an instance of FlutterWindow.
  void* user_data;
  PlatformViewId platform_view_id;
} FlutterPlatformViewCreationParameters;

typedef HWND (*FlutterPlatformViewFactory)(
    const FlutterPlatformViewCreationParameters*);

typedef struct {
  size_t struct_size;
  FlutterPlatformViewFactory factory;
  void* user_data;  // Arbitrary user data supplied to the creation struct.
} FlutterPlatformViewTypeEntry;

APPCODE_EXPORT void FlutterDesktopEngineRegisterPlatformViewType(
    FlutterDesktopEngineRef engine,
    const char* view_type_name,
    FlutterPlatformViewTypeEntry view_type);

// Returns the engine associated with the given identifier. Engine identifier
// must be valid and for a running engine, otherwise the behavior is undefined.
//
// Identifier can be obtained from PlatformDispatcher.instance.engineId.
//
// This method must be called from the platform thread.
APPCODE_EXPORT FlutterDesktopEngineRef FlutterDesktopEngineForId(
    int64_t engine_id);

#if defined(__cplusplus)
}
#endif

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_APPCODE_WINDOWS_INTERNAL_H_
