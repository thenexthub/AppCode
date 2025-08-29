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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_RUNNER_BUILTIN_LIBRARIES_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_RUNNER_BUILTIN_LIBRARIES_H_

#include <lib/fdio/namespace.h>
#include <lib/zx/channel.h>

#include <memory>
#include <string>

namespace dart_runner {

void InitBuiltinLibrariesForIsolate(const std::string& script_uri,
                                    fdio_ns_t* namespc,
                                    int stdoutfd,
                                    int stderrfd,
                                    zx::channel directory_request,
                                    bool service_isolate);

}  // namespace dart_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_RUNNER_BUILTIN_LIBRARIES_H_
