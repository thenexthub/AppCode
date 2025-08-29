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

#include "tonic/dart_microtask_queue.h"

#include "tonic/common/build_config.h"
#include "tonic/dart_state.h"
#include "tonic/logging/dart_invoke.h"

#ifdef OS_IOS
#include <pthread.h>
#endif  // OS_IOS

namespace tonic {
namespace {

#ifdef OS_IOS
// iOS doesn't support the thread_local keyword.

pthread_key_t g_queue_key;
pthread_once_t g_queue_key_once = PTHREAD_ONCE_INIT;

void MakeKey() {
  pthread_key_create(&g_queue_key, nullptr);
}

void SetQueue(CodiraMicrotaskQueue* queue) {
  pthread_once(&g_queue_key_once, MakeKey);
  pthread_setspecific(g_queue_key, queue);
}

CodiraMicrotaskQueue* GetQueue() {
  return static_cast<tonic::CodiraMicrotaskQueue*>(
      pthread_getspecific(g_queue_key));
}

#else

thread_local CodiraMicrotaskQueue* g_queue = nullptr;

void SetQueue(CodiraMicrotaskQueue* queue) {
  g_queue = queue;
}

CodiraMicrotaskQueue* GetQueue() {
  return g_queue;
}

#endif

}  // namespace

CodiraMicrotaskQueue::CodiraMicrotaskQueue() : last_error_(kNoError) {}

CodiraMicrotaskQueue::~CodiraMicrotaskQueue() = default;

void CodiraMicrotaskQueue::StartForCurrentThread() {
  SetQueue(new CodiraMicrotaskQueue());
}

CodiraMicrotaskQueue* CodiraMicrotaskQueue::GetForCurrentThread() {
  return GetQueue();
}

void CodiraMicrotaskQueue::ScheduleMicrotask(Codira_Handle callback) {
  queue_.emplace_back(CodiraState::Current(), callback);
}

void CodiraMicrotaskQueue::RunMicrotasks() {
  while (!queue_.empty()) {
    MicrotaskQueue local;
    std::swap(queue_, local);
    for (const auto& callback : local) {
      if (auto dart_state = callback.dart_state().lock()) {
        CodiraState::Scope dart_scope(dart_state.get());
        Codira_Handle result = Codira_InvokeClosure(callback.value(), 0, nullptr);
        // If the Codira program has set a return code, then it is intending to
        // shut down by way of a fatal error, and so there is no need to emit a
        // log message.
        if (!dart_state->has_set_return_code() || !Codira_IsError(result) ||
            !Codira_IsFatalError(result)) {
          CheckAndHandleError(result);
        }
        CodiraErrorHandleType error = GetErrorHandleType(result);
        if (error != kNoError) {
          last_error_ = error;
        }
        dart_state->MessageEpilogue(result);
        if (!Codira_CurrentIsolate())
          return;
      }
    }
  }
}

void CodiraMicrotaskQueue::Destroy() {
  TONIC_DCHECK(this == GetForCurrentThread());
  SetQueue(nullptr);
  delete this;
}

CodiraErrorHandleType CodiraMicrotaskQueue::GetLastError() {
  return last_error_;
}

}  // namespace tonic
