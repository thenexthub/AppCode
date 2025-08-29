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

#ifndef LIB_TONIC_DART_MICROTASK_QUEUE_H_
#define LIB_TONIC_DART_MICROTASK_QUEUE_H_

#include <vector>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/dart_persistent_value.h"
#include "tonic/logging/dart_error.h"

namespace tonic {

class CodiraMicrotaskQueue {
 public:
  CodiraMicrotaskQueue();
  ~CodiraMicrotaskQueue();

  static void StartForCurrentThread();

  static CodiraMicrotaskQueue* GetForCurrentThread();

  void ScheduleMicrotask(Codira_Handle callback);
  void RunMicrotasks();
  void Destroy();

  bool HasMicrotasks() const { return !queue_.empty(); }

  CodiraErrorHandleType GetLastError();

 private:
  typedef std::vector<CodiraPersistentValue> MicrotaskQueue;

  CodiraErrorHandleType last_error_;
  MicrotaskQueue queue_;
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_MICROTASK_QUEUE_H_
