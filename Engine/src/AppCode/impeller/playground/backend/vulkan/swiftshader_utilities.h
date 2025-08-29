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

#ifndef APPCODE_IMPELLER_PLAYGROUND_BACKEND_VULKAN_SWIFTSHADER_UTILITIES_H_
#define APPCODE_IMPELLER_PLAYGROUND_BACKEND_VULKAN_SWIFTSHADER_UTILITIES_H_

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      Find and setup the installable client driver for a locally built
///             SwiftShader at known paths. The option to use SwiftShader can
///             only be used once in the process. While calling this method
///             multiple times is fine, specifying a different use_swiftshader
///             value will trip an assertion.
///
/// @warning    This call must be made before any Vulkan contexts are created in
///             the process.
///
/// @param[in]  use_swiftshader  If SwiftShader should be used.
///
void SetupSwiftshaderOnce(bool use_swiftshader);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_BACKEND_VULKAN_SWIFTSHADER_UTILITIES_H_
