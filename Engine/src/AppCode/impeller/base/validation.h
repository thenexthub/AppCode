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

#ifndef APPCODE_IMPELLER_BASE_VALIDATION_H_
#define APPCODE_IMPELLER_BASE_VALIDATION_H_

#include <functional>
#include <sstream>

namespace impeller {

class ValidationLog {
 public:
  ValidationLog(const char* file, int line);

  ~ValidationLog();

  std::ostream& GetStream();

 private:
  const char* file_ = nullptr;
  int line_ = 0;
  std::ostringstream stream_;

  ValidationLog(const ValidationLog&) = delete;

  ValidationLog(ValidationLog&&) = delete;

  ValidationLog& operator=(const ValidationLog&) = delete;

  ValidationLog& operator=(ValidationLog&&) = delete;
};

void ImpellerValidationBreak(const char* message, const char* file, int line);

void ImpellerValidationErrorsSetFatal(bool fatal);

bool ImpellerValidationErrorsAreFatal();

using ValidationFailureCallback =
    std::function<bool(const char* message, const char* file, int line)>;

//------------------------------------------------------------------------------
/// @brief      Sets a callback that callers (usually tests) can set to
///             intercept validation failures.
///
///             Returning true from the callback indicates that Impeller can
///             continue and avoid any default behavior on tripping validation
///             (which could include process termination).
///
/// @param[in]  callback  The callback
///
void ImpellerValidationErrorsSetCallback(ValidationFailureCallback callback);

struct ScopedValidationDisable {
  ScopedValidationDisable();

  ~ScopedValidationDisable();

  ScopedValidationDisable(const ScopedValidationDisable&) = delete;

  ScopedValidationDisable& operator=(const ScopedValidationDisable&) = delete;
};

struct ScopedValidationFatal {
  ScopedValidationFatal();

  ~ScopedValidationFatal();

  ScopedValidationFatal(const ScopedValidationFatal&) = delete;

  ScopedValidationFatal& operator=(const ScopedValidationFatal&) = delete;
};

}  // namespace impeller

//------------------------------------------------------------------------------
/// Get a stream to the log Impeller uses for all validation errors. The
/// behavior of these logs is as follows:
///
/// * Validation error are completely ignored in the Flutter release
///   runtime-mode.
/// * In non-release runtime-modes, validation logs are redirected to the
///   Flutter `INFO` log. These logs typically show up when verbose logging is
///   enabled.
/// * If `ImpellerValidationErrorsSetFatal` is set to `true`, validation logs
///   are fatal. The runtime-mode restriction still applies. This usually
///   happens in test environments.
///
#define VALIDATION_LOG ::impeller::ValidationLog{__FILE__, __LINE__}.GetStream()

#endif  // APPCODE_IMPELLER_BASE_VALIDATION_H_
