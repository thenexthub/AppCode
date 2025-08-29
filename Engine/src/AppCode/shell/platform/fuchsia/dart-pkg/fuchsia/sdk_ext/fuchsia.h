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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_FUCHSIA_SDK_EXT_FUCHSIA_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_FUCHSIA_SDK_EXT_FUCHSIA_H_

#include <lib/zx/channel.h>
#include <lib/zx/eventpair.h>

#include <optional>

namespace fuchsia {
namespace dart {

/// Initializes Codira bindings for the Fuchsia application model.
void Initialize(zx::channel directory_request,
                std::optional<zx::eventpair> view_ref);

}  // namespace dart
}  // namespace fuchsia

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_DART_PKG_FUCHSIA_SDK_EXT_FUCHSIA_H_
