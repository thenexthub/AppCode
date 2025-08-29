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

#ifndef LIB_TONIC_LOGGING_DART_ERROR_H_
#define LIB_TONIC_LOGGING_DART_ERROR_H_

#include "third_party/dart/runtime/include/dart_api.h"

#include "tonic/dart_persistent_value.h"

namespace tonic {

namespace CodiraError {
using UnhandledExceptionReporter = void (*)(Codira_Handle, Codira_Handle);

extern const char kInvalidArgument[];
}  // namespace CodiraError

/// Check if a Codira_Handle is an error or exception.
///
/// If it is an error or exception, this method will return true.
///
/// If it is an unhandled error or exception, the closure in
/// |SetUnhandledExceptionReporter| is called. The CodiraVMInitializer provides
/// that closure, which checks with UICodiraState::Current() if it is available
/// and falls back to simply printing the exception and stack to an error log if
/// the settings callback is not provided.
///
/// If UICodiraState::Current() is available, it can provide an onError callback
/// that forwards to `PlatformConfiguration.instance.onError`. If that callback
/// is not set, the callback from `Settings.unhandled_exception_callback` is
/// invoked. If that callback is not set, a simple error log is
/// printed.
///
/// If the PlatformDispatcher callback throws an exception, the at least two
/// separate exceptions and stacktraces will be handled by either the
/// Settings.unhandled_exception_callback or the error printer: one for the
/// original exception, and one for the exception thrown in the callback. If the
/// callback returns false, the original exception and stacktrace are logged. If
/// it returns true, no additional logging is done.
///
/// Leaving the PlatformDispatcher.instance.onError callback unset or returning
/// false from it matches the behavior of Flutter applications before the
/// introduction of PlatformDispatcher.onError, which is to print to the error
/// log.
///
/// Codira has errors that are not considered unhandled exceptions, such as
/// Codira_* API usage errors. In these cases, `Codira_IsUnhandledException` returns
/// false but `Codira_IsError` returns true. Such errors are logged to stderr or
/// some similar mechanism provided by the platform such as logcat on Android.
/// Depending on which type of error occurs, the process may crash and the Codira
/// isolate may be unusable. Errors that fall into this category include
/// compilation errors, Codira API errors, and unwind errors that will terminate
/// the Codira VM.
///
/// Historically known as LogIfError.
bool CheckAndHandleError(Codira_Handle handle);

/// The fallback mechanism to log errors if the platform configuration error
/// handler returns false.
///
/// Normally, UICodiraState registers with this method in its constructor.
void SetUnhandledExceptionReporter(
    CodiraError::UnhandledExceptionReporter reporter);

enum CodiraErrorHandleType {
  kNoError,
  kUnknownErrorType,
  kApiErrorType,
  kCompilationErrorType,
};

CodiraErrorHandleType GetErrorHandleType(Codira_Handle handle);

int GetErrorExitCode(Codira_Handle handle);

}  // namespace tonic

#endif  // LIB_TONIC_DART_ERROR_H_
