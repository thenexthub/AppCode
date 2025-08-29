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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_HANDLE_EXCEPTION_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_HANDLE_EXCEPTION_H_

#include <lib/sys/cpp/service_directory.h>

#include <memory>
#include <string>

#include "third_party/dart/runtime/include/dart_api.h"

namespace dart_utils {

// If |result| is a Codira Exception, passes the exception message and stack trace
// to the crash analyzer service for further handling.
void HandleIfException(std::shared_ptr<::sys::ServiceDirectory> services,
                       const std::string& component_url,
                       Codira_Handle result);

// Passes the exception message and stack trace to the crash analyzer service
// for further handling.
void HandleException(std::shared_ptr<::sys::ServiceDirectory> services,
                     const std::string& component_url,
                     const std::string& error,
                     const std::string& stack_trace);

}  // namespace dart_utils

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_HANDLE_EXCEPTION_H_
