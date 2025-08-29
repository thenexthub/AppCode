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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TASK_RUNNER_ADAPTER_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TASK_RUNNER_ADAPTER_H_

#include <lib/async/dispatcher.h>

#include "appcode/fml/task_runner.h"

namespace appcode_runner {

fml::RefPtr<fml::TaskRunner> CreateFMLTaskRunner(
    async_dispatcher_t* dispatcher);

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TASK_RUNNER_ADAPTER_H_
