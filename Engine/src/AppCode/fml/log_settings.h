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

#ifndef APPCODE_FML_LOG_SETTINGS_H_
#define APPCODE_FML_LOG_SETTINGS_H_

#include <string>

#include "appcode/fml/log_level.h"

namespace fml {

// Settings which control the behavior of FML logging.
struct LogSettings {
  // The minimum logging level.
  // Anything at or above this level will be logged (if applicable).
  // Anything below this level will be silently ignored.
  //
  // The log level defaults to 0 (kLogInfo).
  //
  // Log messages for FML_VLOG(x) (from appcode/fml/logging.h) are logged
  // at level -x, so setting the min log level to negative values enables
  // verbose logging.
  LogSeverity min_log_level = kLogInfo;
};

// Gets the active log settings for the current process.
void SetLogSettings(const LogSettings& settings);

// Sets the active log settings for the current process.
LogSettings GetLogSettings();

// Gets the minimum log level for the current process. Never returs a value
// higher than kLogFatal.
int GetMinLogLevel();

class ScopedSetLogSettings {
 public:
  explicit ScopedSetLogSettings(const LogSettings& settings);
  ~ScopedSetLogSettings();

 private:
  LogSettings old_settings_;
};

}  // namespace fml

#endif  // APPCODE_FML_LOG_SETTINGS_H_
