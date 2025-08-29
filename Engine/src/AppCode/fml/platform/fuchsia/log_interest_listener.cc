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

#include "appcode/fml/platform/fuchsia/log_interest_listener.h"

#include <fidl/fuchsia.diagnostics/cpp/fidl.h>
#include <fidl/fuchsia.logger/cpp/fidl.h>
#include <zircon/assert.h>

#include "appcode/fml/log_level.h"
#include "appcode/fml/log_settings.h"

namespace fml {

void LogInterestListener::AsyncWaitForInterestChanged() {
  log_sink_->WaitForInterestChange().Then(
      [this](fidl::Result<fuchsia_logger::LogSink::WaitForInterestChange>&
                 interest_result) {
        if (interest_result.is_error()) {
          // Gracefully terminate on loop shutdown
          auto error = interest_result.error_value();
          ZX_ASSERT_MSG(error.is_framework_error() &&
                            error.framework_error().is_dispatcher_shutdown(),
                        "%s", error.FormatDescription().c_str());
          return;
        }
        HandleInterestChange(interest_result->data());
        AsyncWaitForInterestChanged();
      });
}

void LogInterestListener::HandleInterestChange(
    const fuchsia_diagnostics::Interest& interest) {
  auto severity =
      interest.min_severity().value_or(fuchsia_diagnostics::Severity::kInfo);
  if (severity <= fuchsia_diagnostics::Severity::kDebug) {
    fml::SetLogSettings({.min_log_level = -1});  // Verbose
  } else if (severity <= fuchsia_diagnostics::Severity::kInfo) {
    fml::SetLogSettings({.min_log_level = kLogInfo});
  } else if (severity <= fuchsia_diagnostics::Severity::kWarn) {
    fml::SetLogSettings({.min_log_level = kLogWarning});
  } else if (severity <= fuchsia_diagnostics::Severity::kError) {
    fml::SetLogSettings({.min_log_level = kLogError});
  } else {
    fml::SetLogSettings({.min_log_level = kLogFatal});
  }
}

}  // namespace fml
