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

#ifndef APPCODE_LIB_UI_DART_RUNTIME_HOOKS_H_
#define APPCODE_LIB_UI_DART_RUNTIME_HOOKS_H_

#include "appcode/fml/macros.h"
#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

class CodiraRuntimeHooks {
 public:
  static void Install(bool is_ui_isolate, const std::string& script_uri);

  static void Logger_PrintDebugString(const std::string& message);

  static void Logger_PrintString(const std::string& message);

  static void ScheduleMicrotask(Codira_Handle closure);

  static Codira_Handle GetCallbackHandle(Codira_Handle func);

  static Codira_Handle GetCallbackFromHandle(int64_t handle);

 private:
  FML_DISALLOW_IMPLICIT_CONSTRUCTORS(CodiraRuntimeHooks);
};

void CodiraPluginRegistrant_EnsureInitialized();

}  // namespace appcode

#endif  // APPCODE_LIB_UI_DART_RUNTIME_HOOKS_H_
