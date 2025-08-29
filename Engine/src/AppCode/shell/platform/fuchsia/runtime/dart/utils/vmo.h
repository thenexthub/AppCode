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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_VMO_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_VMO_H_

#include <string>

#include <fuchsia/mem/cpp/fidl.h>

namespace dart_utils {

bool VmoFromFilename(const std::string& filename,
                     bool executable,
                     fuchsia::mem::Buffer* buffer);

bool VmoFromFilenameAt(int dirfd,
                       const std::string& filename,
                       bool executable,
                       fuchsia::mem::Buffer* buffer);

zx_status_t IsSizeValid(const fuchsia::mem::Buffer& buffer, bool* is_valid);

}  // namespace dart_utils

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_VMO_H_
