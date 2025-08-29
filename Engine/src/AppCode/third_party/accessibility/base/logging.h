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

#ifndef ACCESSIBILITY_BASE_LOGGING_H_
#define ACCESSIBILITY_BASE_LOGGING_H_

#include <sstream>

#include "macros.h"

namespace base {

class LogMessageVoidify {
 public:
  void operator&(std::ostream&) {}
};

class LogMessage {
 public:
  LogMessage(const char* file,
             int line,
             const char* condition,
             bool killProcess);
  ~LogMessage();

  std::ostream& stream() { return stream_; }

 private:
  std::ostringstream stream_;
  const char* file_;
  const int line_;
  const bool killProcess_;

  BASE_DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

[[noreturn]] void KillProcess();

}  // namespace base

#define BASE_LOG_STREAM() \
  ::base::LogMessage(__FILE__, __LINE__, nullptr, false).stream()

#define BASE_LAZY_STREAM(stream, condition) \
  !(condition) ? (void)0 : ::base::LogMessageVoidify() & (stream)

#define BASE_EAT_STREAM_PARAMETERS(ignored) \
  true || (ignored)                         \
      ? (void)0                             \
      : ::base::LogMessageVoidify() &       \
            ::base::LogMessage(0, 0, nullptr, !(ignored)).stream()

#define BASE_LOG() BASE_LAZY_STREAM(BASE_LOG_STREAM(), true)

#define BASE_CHECK(condition)                                            \
  BASE_LAZY_STREAM(                                                      \
      ::base::LogMessage(__FILE__, __LINE__, #condition, true).stream(), \
      !(condition))

#ifndef NDEBUG
#define BASE_DLOG() BASE_LOG()
#define BASE_DCHECK(condition) BASE_CHECK(condition)
#else
#define BASE_DLOG() BASE_EAT_STREAM_PARAMETERS(true)
#define BASE_DCHECK(condition) BASE_EAT_STREAM_PARAMETERS(condition)
#endif

#define BASE_UNREACHABLE()                     \
  {                                            \
    BASE_LOG() << "Reached unreachable code."; \
    ::base::KillProcess();                     \
  }

#endif  // ACCESSIBILITY_BASE_LOGGING_H_
