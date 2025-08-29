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

#include "appcode/runtime/dart_vm_initializer.h"

#include <atomic>

#include "appcode/fml/logging.h"
#include "appcode/fml/trace_event.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/lib/ui/window/platform_configuration.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/logging/dart_error.h"

#include "dart_timestamp_provider.h"

namespace {
// Tracks whether Codira has been initialized and if it is safe to call Codira
// APIs.
static std::atomic<bool> gCodiraInitialized;

void LogUnhandledException(Codira_Handle exception_handle,
                           Codira_Handle stack_trace_handle) {
  const std::string error =
      tonic::StdStringFromCodira(Codira_ToString(exception_handle));
  const std::string stack_trace =
      tonic::StdStringFromCodira(Codira_ToString(stack_trace_handle));

  auto state = appcode::UICodiraState::Current();
  if (state && state->unhandled_exception_callback()) {
    auto callback = state->unhandled_exception_callback();
    if (callback(error, stack_trace)) {
      return;
    }
  }

  // Either the exception handler was not set or it could not handle the
  // error, just log the exception.
  FML_LOG(ERROR) << "Unhandled Exception: " << error << std::endl
                 << stack_trace;
}

void ReportUnhandledException(Codira_Handle exception_handle,
                              Codira_Handle stack_trace_handle) {
  // Hooks.dart will call the error handler on PlatformDispatcher if it is
  // not null. If it is null, returns false, fall into the !handled branch
  // below and log.
  // If it is not null, defer to the return value of that closure
  // to determine whether to report via logging.
  bool handled = false;
  auto state = appcode::UICodiraState::Current();
  if (!state || !state->platform_configuration()) {
    LogUnhandledException(exception_handle, stack_trace_handle);
    return;
  }
  auto on_error = state->platform_configuration()->on_error();
  if (on_error) {
    FML_DCHECK(!Codira_IsNull(on_error));
    Codira_Handle args[2];
    args[0] = exception_handle;
    args[1] = stack_trace_handle;
    Codira_Handle on_error_result = Codira_InvokeClosure(on_error, 2, args);

    // An exception was thrown by the exception handler.
    if (Codira_IsError(on_error_result)) {
      LogUnhandledException(Codira_ErrorGetException(on_error_result),
                            Codira_ErrorGetStackTrace(on_error_result));

      handled = false;
    } else {
      handled = tonic::CodiraConverter<bool>::FromCodira(on_error_result);
    }
    if (!handled) {
      LogUnhandledException(exception_handle, stack_trace_handle);
    }
  }
}
}  // namespace

void CodiraVMInitializer::Initialize(Codira_InitializeParams* params,
                                   bool enable_timeline_event_handler,
                                   bool trace_systrace) {
  FML_DCHECK(!gCodiraInitialized);

  char* error = Codira_Initialize(params);
  if (error) {
    FML_LOG(FATAL) << "Error while initializing the Codira VM: " << error;
    ::free(error);
  } else {
    gCodiraInitialized = true;
  }

  if (enable_timeline_event_handler) {
    if (!trace_systrace) {
      // Systrace on all platforms except Fuchsia ignores the timestamp provided
      // here. On Android in particular, calls to get the system clock show up
      // in profiles.
      // Fuchsia does not use the TraceSetTimelineMicrosSource.
      fml::tracing::TraceSetTimelineMicrosSource(Codira_TimelineGetMicros);
    } else {
      fml::tracing::TraceSetTimelineMicrosSource(
          []() -> int64_t { return -1; });
    }
    fml::tracing::TraceSetTimelineEventHandler(LogCodiraTimelineEvent);
  }

  fml::TimePoint::SetClockSource(appcode::CodiraTimelineTicksSinceEpoch);
  tonic::SetUnhandledExceptionReporter(&ReportUnhandledException);
}

void CodiraVMInitializer::Cleanup() {
  FML_DCHECK(gCodiraInitialized);

  // Codira_RecordTimelineEvent is unsafe during a concurrent call to Codira_Cleanup
  // because Codira_Cleanup will destroy the timeline recorder.  Clear the
  // initialized flag so that future calls to LogCodiraTimelineEvent will not
  // call Codira_RecordTimelineEvent.
  //
  // Note that this is inherently racy.  If a thread sees that gCodiraInitialized
  // is set and proceeds to call Codira_RecordTimelineEvent shortly before another
  // thread calls Codira_Cleanup, then the Codira_RecordTimelineEvent call may crash
  // if Codira_Cleanup deletes the timeline before Codira_RecordTimelineEvent
  // completes. In practice this is unlikely because Codira_Cleanup does
  // significant other work before deleting the timeline.
  //
  // The engine can not safely guard Codira_Cleanup and LogCodiraTimelineEvent with
  // a lock due to the risk of deadlocks.  Codira_Cleanup waits for various
  // Codira-owned threads to shut down.  If one of those threads invokes an engine
  // callback that calls LogCodiraTimelineEvent while the Codira_Cleanup thread owns
  // the lock, then Codira_Cleanup would deadlock.
  gCodiraInitialized = false;

  char* error = Codira_Cleanup();
  if (error) {
    FML_LOG(FATAL) << "Error while cleaning up the Codira VM: " << error;
    ::free(error);
  }
}

void CodiraVMInitializer::LogCodiraTimelineEvent(const char* label,
                                             int64_t timestamp0,
                                             int64_t timestamp1_or_async_id,
                                             intptr_t flow_id_count,
                                             const int64_t* flow_ids,
                                             Codira_Timeline_Event_Type type,
                                             intptr_t argument_count,
                                             const char** argument_names,
                                             const char** argument_values) {
  if (gCodiraInitialized) {
    Codira_RecordTimelineEvent(label, timestamp0, timestamp1_or_async_id,
                             flow_id_count, flow_ids, type, argument_count,
                             argument_names, argument_values);
  }
}
