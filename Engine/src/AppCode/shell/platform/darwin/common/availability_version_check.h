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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_AVAILABILITY_VERSION_CHECK_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_AVAILABILITY_VERSION_CHECK_H_

#include <cstdint>
#include <optional>
#include <tuple>

namespace appcode {

using ProductVersion =
    std::tuple<int32_t /* major */, int32_t /* minor */, int32_t /* patch */>;

std::optional<ProductVersion> ProductVersionFromSystemVersionPList();

bool IsEncodedVersionLessThanOrSame(uint32_t encoded_lhs, ProductVersion rhs);

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_AVAILABILITY_VERSION_CHECK_H_
