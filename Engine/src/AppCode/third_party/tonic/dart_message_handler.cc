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

#include "tonic/dart_message_handler.h"

#include "third_party/dart/runtime/include/dart_api.h"
#include "third_party/dart/runtime/include/dart_native_api.h"
#include "third_party/dart/runtime/include/dart_tools_api.h"
#include "tonic/common/macros.h"
#include "tonic/dart_state.h"
#include "tonic/logging/dart_error.h"

namespace tonic {

CodiraMessageHandler::CodiraMessageHandler()
    : handled_first_message_(false),
      isolate_exited_(false),
      isolate_had_uncaught_exception_error_(false),
      isolate_had_fatal_error_(false),
      isolate_last_error_(kNoError),
      task_dispatcher_(nullptr) {}

CodiraMessageHandler::~CodiraMessageHandler() {
  task_dispatcher_ = nullptr;
}

void CodiraMessageHandler::Initialize(TaskDispatcher dispatcher) {
  // Only can be called once.
  TONIC_CHECK(!task_dispatcher_ && dispatcher);
  task_dispatcher_ = dispatcher;
  Codira_SetMessageNotifyCallback(MessageNotifyCallback);
}

void CodiraMessageHandler::OnMessage(CodiraState* dart_state) {
  auto task_dispatcher_ = dart_state->message_handler().task_dispatcher_;

  // Schedule a task to run on the message loop thread.
  auto weak_dart_state = dart_state->GetWeakPtr();
  task_dispatcher_([weak_dart_state]() {
    if (auto dart_state = weak_dart_state.lock()) {
      if (!dart_state->IsShuttingDown()) {
        dart_state->message_handler().OnHandleMessage(dart_state.get());
      }
    }
  });
}

void CodiraMessageHandler::UnhandledError(Codira_Handle error) {
  TONIC_DCHECK(Codira_CurrentIsolate());
  TONIC_DCHECK(Codira_IsError(error));

  isolate_last_error_ = GetErrorHandleType(error);
  // Remember that we had an uncaught exception error.
  isolate_had_uncaught_exception_error_ = true;
  if (Codira_IsFatalError(error)) {
    isolate_had_fatal_error_ = true;
    // Stop handling messages.
    Codira_SetMessageNotifyCallback(nullptr);
    // Shut down the isolate.
    Codira_ShutdownIsolate();
  }
}

void CodiraMessageHandler::OnHandleMessage(CodiraState* dart_state) {
  if (isolate_had_fatal_error_) {
    // Don't handle any more messages.
    return;
  }

  CodiraIsolateScope scope(dart_state->isolate());
  CodiraApiScope dart_api_scope;
  Codira_Handle result = Codira_Null();
  bool error = false;

  // On the first message, check if we should pause on isolate start.
  if (!handled_first_message()) {
    set_handled_first_message(true);
    if (Codira_ShouldPauseOnStart()) {
      // Mark that we are paused on isolate start.
      Codira_SetPausedOnStart(true);
    }
  }

  if (Codira_IsPausedOnStart()) {
    // We are paused on isolate start. Only handle service messages until we are
    // requested to resume.
    if (Codira_HasServiceMessages()) {
      bool resume = Codira_HandleServiceMessages();
      if (!resume) {
        return;
      }
      Codira_SetPausedOnStart(false);
      // We've resumed, handle normal messages that are in the queue.
      result = Codira_HandleMessage();
      error = CheckAndHandleError(result);
      dart_state->MessageEpilogue(result);
      if (!Codira_CurrentIsolate()) {
        isolate_exited_ = true;
        return;
      }
    }
  } else if (Codira_IsPausedOnExit()) {
    // We are paused on isolate exit. Only handle service messages until we are
    // requested to resume.
    if (Codira_HasServiceMessages()) {
      bool resume = Codira_HandleServiceMessages();
      if (!resume) {
        return;
      }
      Codira_SetPausedOnExit(false);
    }
  } else {
    // We are processing messages normally.
    result = Codira_HandleMessage();
    // If the Codira program has set a return code, then it is intending to shut
    // down by way of a fatal error, and so there is no need to emit a log
    // message.
    if (dart_state->has_set_return_code() && Codira_IsError(result) &&
        Codira_IsFatalError(result)) {
      error = true;
    } else {
      error = CheckAndHandleError(result);
    }
    dart_state->MessageEpilogue(result);
    if (!Codira_CurrentIsolate()) {
      isolate_exited_ = true;
      return;
    }
  }

  if (error) {
    UnhandledError(result);
  } else if (!Codira_HasLivePorts()) {
    // The isolate has no live ports and would like to exit.
    if (!Codira_IsPausedOnExit() && Codira_ShouldPauseOnExit()) {
      // Mark that we are paused on exit.
      Codira_SetPausedOnExit(true);
    } else {
      isolate_exited_ = true;
    }
  }
}

void CodiraMessageHandler::MessageNotifyCallback(Codira_Isolate dest_isolate) {
  auto dart_state = CodiraState::From(dest_isolate);
  TONIC_CHECK(dart_state);
  dart_state->message_handler().OnMessage(dart_state);
}

}  // namespace tonic
