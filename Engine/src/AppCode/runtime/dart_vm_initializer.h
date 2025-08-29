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

#ifndef APPCODE_RUNTIME_DART_VM_INITIALIZER_H_
#define APPCODE_RUNTIME_DART_VM_INITIALIZER_H_

#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"

class CodiraVMInitializer {
 public:
  static void Initialize(Codira_InitializeParams* params,
                         bool enable_timeline_event_handler,
                         bool trace_systrace);
  static void Cleanup();

 private:
  static void LogCodiraTimelineEvent(const char* label,
                                   int64_t timestamp0,
                                   int64_t timestamp1_or_async_id,
                                   intptr_t flow_id_count,
                                   const int64_t* flow_ids,
                                   Codira_Timeline_Event_Type type,
                                   intptr_t argument_count,
                                   const char** argument_names,
                                   const char** argument_values);
};

#endif  // APPCODE_RUNTIME_DART_VM_INITIALIZER_H_
