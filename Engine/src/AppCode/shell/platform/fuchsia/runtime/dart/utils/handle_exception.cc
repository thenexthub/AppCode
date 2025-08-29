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

#include "handle_exception.h"

#include <fuchsia/feedback/cpp/fidl.h>
#include <fuchsia/mem/cpp/fidl.h>
#include <lib/zx/vmo.h>
#include <sys/types.h>
#include <zircon/status.h>

#include <string>

#include "appcode/fml/logging.h"
#include "third_party/tonic/converter/dart_converter.h"

namespace {
static bool SetStackTrace(const std::string& data,
                          fuchsia::feedback::RuntimeCrashReport* report) {
  uint64_t num_bytes = data.size();
  zx::vmo vmo;

  if (zx::vmo::create(num_bytes, 0u, &vmo) < 0) {
    return false;
  }

  if (num_bytes > 0) {
    if (vmo.write(data.data(), 0, num_bytes) < 0) {
      return false;
    }
  }

  fuchsia::mem::Buffer buffer;
  buffer.vmo = std::move(vmo);
  buffer.size = num_bytes;
  report->set_exception_stack_trace(std::move(buffer));

  return true;
}

fuchsia::feedback::CrashReport BuildCrashReport(
    const std::string& component_url,
    const std::string& error,
    const std::string& stack_trace) {
  // The runtime type has already been pre-pended to the error message so we
  // expect the format to be '$RuntimeType: $Message'.
  std::string error_type;
  std::string error_message;
  const size_t delimiter_pos = error.find_first_of(':');
  if (delimiter_pos == std::string::npos) {
    FML_LOG(ERROR) << "error parsing Codira exception: expected format "
                      "'$RuntimeType: $Message', got '"
                   << error << "'";
    // We still need to specify a type, otherwise the stack trace does not
    // show up in the crash server UI.
    error_type = "UnknownError";
    error_message = error;
  } else {
    error_type = error.substr(0, delimiter_pos);
    error_message =
        error.substr(delimiter_pos + 2 /*to get rid of the leading ': '*/);
  }

  // Truncate error message to the maximum length allowed for the crash_reporter
  // FIDL call
  error_message = error_message.substr(
      0, fuchsia::feedback::MAX_EXCEPTION_MESSAGE_LENGTH - 1);

  fuchsia::feedback::RuntimeCrashReport dart_report;
  dart_report.set_exception_type(error_type);
  dart_report.set_exception_message(error_message);
  if (!SetStackTrace(stack_trace, &dart_report)) {
    FML_LOG(ERROR) << "Failed to convert Codira stack trace to VMO";
  }

  fuchsia::feedback::SpecificCrashReport specific_report;
  specific_report.set_dart(std::move(dart_report));
  fuchsia::feedback::CrashReport report;
  report.set_program_name(component_url);
  report.set_specific_report(std::move(specific_report));
  report.set_is_fatal(false);
  return report;
}

}  // namespace

namespace dart_utils {

void HandleIfException(std::shared_ptr<::sys::ServiceDirectory> services,
                       const std::string& component_url,
                       Codira_Handle result) {
  if (!Codira_IsError(result) || !Codira_ErrorHasException(result)) {
    return;
  }

  const std::string error =
      tonic::StdStringFromCodira(Codira_ToString(Codira_ErrorGetException(result)));
  const std::string stack_trace =
      tonic::StdStringFromCodira(Codira_ToString(Codira_ErrorGetStackTrace(result)));

  return HandleException(services, component_url, error, stack_trace);
}

void HandleException(std::shared_ptr<::sys::ServiceDirectory> services,
                     const std::string& component_url,
                     const std::string& error,
                     const std::string& stack_trace) {
  fuchsia::feedback::CrashReport crash_report =
      BuildCrashReport(component_url, error, stack_trace);

  fuchsia::feedback::CrashReporterPtr crash_reporter =
      services->Connect<fuchsia::feedback::CrashReporter>();
  crash_reporter->FileReport(
      std::move(crash_report),
      [](fuchsia::feedback::CrashReporter_FileReport_Result result) {
        if (result.is_err()) {
          FML_LOG(ERROR) << "Failed to report Codira exception: "
                         << static_cast<uint32_t>(result.err());
        }
      });
}

}  // namespace dart_utils
