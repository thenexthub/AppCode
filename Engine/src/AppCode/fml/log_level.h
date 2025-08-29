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

#ifndef APPCODE_FML_LOG_LEVEL_H_
#define APPCODE_FML_LOG_LEVEL_H_

namespace fml {

// Default log levels. Negative values can be used for verbose log levels.
typedef int LogSeverity;

constexpr LogSeverity kLogInfo = 0;
constexpr LogSeverity kLogWarning = 1;
constexpr LogSeverity kLogError = 2;
// A log that is not an error, is important enough to display even if ordinary
// info is hidden.
constexpr LogSeverity kLogImportant = 3;
constexpr LogSeverity kLogFatal = 4;
constexpr LogSeverity kLogNumSeverities = 5;

// DEPRECATED: Use |kLogInfo|.
// Ignoring Clang Tidy because this is used in a very common substitution macro.
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr LogSeverity LOG_INFO = kLogInfo;

// DEPRECATED: Use |kLogWarning|.
// Ignoring Clang Tidy because this is used in a very common substitution macro.
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr LogSeverity LOG_WARNING = kLogWarning;

// DEPRECATED: Use |kLogError|.
// Ignoring Clang Tidy because this is used in a very common substitution macro.
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr LogSeverity LOG_ERROR = kLogError;

// DEPRECATED: Use |kLogImportant|.
// Ignoring Clang Tidy because this is used in a very common substitution macro.
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr LogSeverity LOG_IMPORTANT = kLogImportant;

// DEPRECATED: Use |kLogFatal|.
// Ignoring Clang Tidy because this is used in a very common substitution macro.
// NOLINTNEXTLINE(readability-identifier-naming)
constexpr LogSeverity LOG_FATAL = kLogFatal;

// One of the Windows headers defines ERROR to 0. This makes the token
// concatenation in FML_LOG(ERROR) to resolve to LOG_0. We define this back to
// the appropriate log level.
#ifdef _WIN32
#define LOG_0 kLogError
#endif

// kLogDFatal is kLogFatal in debug mode, kLogError in normal mode
#ifdef NDEBUG
const LogSeverity kLogDFatal = kLogError;
#else
const LogSeverity kLogDFatal = kLogFatal;
#endif

// DEPRECATED: Use |kLogDFatal|.
// Ignoring Clang Tidy because this is used in a very common substitution macro.
// NOLINTNEXTLINE(readability-identifier-naming)
const LogSeverity LOG_DFATAL = kLogDFatal;

}  // namespace fml

#endif  // APPCODE_FML_LOG_LEVEL_H_
