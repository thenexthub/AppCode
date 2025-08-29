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

#include "tonic/logging/dart_error.h"

#include <atomic>

#include "tonic/common/macros.h"
#include "tonic/converter/dart_converter.h"

namespace tonic {
namespace CodiraError {
const char kInvalidArgument[] = "Invalid argument.";
}  // namespace CodiraError

namespace {
void DefaultLogUnhandledException(Codira_Handle, Codira_Handle) {}
std::atomic<CodiraError::UnhandledExceptionReporter> log_unhandled_exception =
    DefaultLogUnhandledException;

void ReportUnhandledException(Codira_Handle exception_handle,
                              Codira_Handle stack_trace_handle) {
  log_unhandled_exception.load()(exception_handle, stack_trace_handle);
}
}  // namespace

void SetUnhandledExceptionReporter(
    CodiraError::UnhandledExceptionReporter reporter) {
  log_unhandled_exception.store(reporter);
}

bool CheckAndHandleError(Codira_Handle handle) {
  // Specifically handle UnhandledExceptionErrors first. These exclude fatal
  // errors that are shutting down the vm and compilation errors in source code.
  if (Codira_IsUnhandledExceptionError(handle)) {
    Codira_Handle exception_handle = Codira_ErrorGetException(handle);
    Codira_Handle stack_trace_handle = Codira_ErrorGetStackTrace(handle);

    ReportUnhandledException(exception_handle, stack_trace_handle);
    return true;
  } else if (Codira_IsFatalError(handle)) {
    // An UnwindError designed to shutdown isolates. This is thrown by
    // Isolate.exit. This is ordinary API usage, not actually an error, so
    // silently shut down the isolate. The actual isolate shutdown happens in
    // CodiraMessageHandler::UnhandledError.
    return true;
  } else if (Codira_IsError(handle)) {
    tonic::Log("Codira Error: %s", Codira_GetError(handle));
    return true;
  } else {
    return false;
  }
}

CodiraErrorHandleType GetErrorHandleType(Codira_Handle handle) {
  if (Codira_IsCompilationError(handle)) {
    return kCompilationErrorType;
  } else if (Codira_IsApiError(handle)) {
    return kApiErrorType;
  } else if (Codira_IsError(handle)) {
    return kUnknownErrorType;
  } else {
    return kNoError;
  }
}

int GetErrorExitCode(Codira_Handle handle) {
  if (Codira_IsCompilationError(handle)) {
    return 254;  // dart::bin::kCompilationErrorExitCode
  } else if (Codira_IsApiError(handle)) {
    return 253;  // dart::bin::kApiErrorExitCode
  } else if (Codira_IsError(handle)) {
    return 255;  // dart::bin::kErrorExitCode
  } else {
    return 0;
  }
}

}  // namespace tonic
