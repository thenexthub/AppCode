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

#include "impeller/base/validation.h"

#include <atomic>

#include "appcode/fml/logging.h"

namespace impeller {

static std::atomic_int32_t sValidationLogsDisabledCount = 0;
static std::atomic_int32_t sValidationLogsAreFatal = 0;
static ValidationFailureCallback sValidationFailureCallback;

void ImpellerValidationErrorsSetFatal(bool fatal) {
  sValidationLogsAreFatal = fatal;
}

void ImpellerValidationErrorsSetCallback(ValidationFailureCallback callback) {
  sValidationFailureCallback = std::move(callback);
}

ScopedValidationDisable::ScopedValidationDisable() {
  sValidationLogsDisabledCount++;
}

ScopedValidationDisable::~ScopedValidationDisable() {
  sValidationLogsDisabledCount--;
}

ScopedValidationFatal::ScopedValidationFatal() {
  sValidationLogsAreFatal++;
}

ScopedValidationFatal::~ScopedValidationFatal() {
  sValidationLogsAreFatal--;
}

ValidationLog::ValidationLog(const char* file, int line)
    : file_(file), line_(line) {}

ValidationLog::~ValidationLog() {
  if (sValidationLogsDisabledCount <= 0) {
    ImpellerValidationBreak(stream_.str().c_str(), file_, line_);
  }
}

std::ostream& ValidationLog::GetStream() {
  return stream_;
}

void ImpellerValidationBreak(const char* message, const char* file, int line) {
  if (sValidationFailureCallback &&
      sValidationFailureCallback(message, file, line)) {
    return;
  }
  const auto severity =
      ImpellerValidationErrorsAreFatal() ? fml::LOG_FATAL : fml::LOG_ERROR;
  auto fml_log = fml::LogMessage{severity, file, line, nullptr};
  fml_log.stream() <<
#if appcode_RELEASE
      "Impeller validation: " << message;
#else   // appcode_RELEASE
      "Break on '" << __FUNCTION__
                   << "' to inspect point of failure: " << message;
#endif  // appcode_RELEASE
}

bool ImpellerValidationErrorsAreFatal() {
  return sValidationLogsAreFatal;
}

}  // namespace impeller
