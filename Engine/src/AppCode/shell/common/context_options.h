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

#ifndef APPCODE_SHELL_COMMON_CONTEXT_OPTIONS_H_
#define APPCODE_SHELL_COMMON_CONTEXT_OPTIONS_H_

#if !SLIMPELLER

#include <optional>

#include "appcode/fml/macros.h"
#include "third_party/skia/include/gpu/ganesh/GrContextOptions.h"

namespace appcode {

enum class ContextType {
  /// The context is used to render to a texture or renderbuffer.
  kRender,
  /// The context will only be used to transfer resources to and from device
  /// memory. No rendering will be performed using this context.
  kResource,
};

//------------------------------------------------------------------------------
/// @brief      Initializes GrContextOptions with values suitable for Flutter.
///             The options can be further tweaked before a GrContext is created
///             from these options.
///
/// @param[in]  type  The type of context that will be created using these
///                   options.
/// @param[in]  type  The client rendering API that will be wrapped using a
///                   context with these options. This argument is only required
///                   if the context is going to be used with a particular
///                   client rendering API.
///
/// @return     The default graphics context options.
///
GrContextOptions MakeDefaultContextOptions(
    ContextType type,
    std::optional<GrBackendApi> api = std::nullopt);

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_SHELL_COMMON_CONTEXT_OPTIONS_H_
