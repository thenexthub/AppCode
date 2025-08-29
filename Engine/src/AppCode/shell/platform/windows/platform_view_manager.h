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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_PLATFORM_VIEW_MANAGER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_PLATFORM_VIEW_MANAGER_H_

#include <memory>

#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_channel.h"
#include "appcode/shell/platform/windows/appcode_windows_internal.h"
#include "appcode/shell/platform/windows/task_runner.h"

namespace appcode {

// Possible reasons for change of keyboard focus.
enum class FocusChangeDirection {
  kProgrammatic,  // Un-directed focus change.
  kForward,       // Keyboard focus moves forwards, e.g. TAB key.
  kBackward       // Keyboard focus moves backwards, e.g. Shift+TAB.
};

// The platform method handler for platform view related communication between
// the engine and the framework. This base class is derived by a concrete class
// (i.e. PlatformViewPlugin) to provide implementation of its abstract virtual
// methods.
class PlatformViewManager {
 public:
  PlatformViewManager(BinaryMessenger* binary_messenger);

  virtual ~PlatformViewManager();

  // Add a new platform view instance to be lazily instantiated when it is next
  // composited. The manager will invoke Success when this method returns true,
  // and invoke Error otherwise.
  virtual bool AddPlatformView(PlatformViewId id,
                               std::string_view type_name) = 0;

  // The framework may invoke this method when keyboard focus must be given to
  // the platform view. The manager will invoke Success when this method returns
  // true, and invoke Error otherwise.
  virtual bool FocusPlatformView(PlatformViewId id,
                                 FocusChangeDirection direction,
                                 bool focus) = 0;

 private:
  std::unique_ptr<MethodChannel<EncodableValue>> channel_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_PLATFORM_VIEW_MANAGER_H_
