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

#ifndef APPCODE_RUNTIME_DART_PLUGIN_REGISTRANT_H_
#define APPCODE_RUNTIME_DART_PLUGIN_REGISTRANT_H_

#include "third_party/dart/runtime/include/dart_api.h"

namespace appcode {

/// The name of the library where the Codira Plugin Registrant will be looked for.
/// This is available for testing.
extern const char* dart_plugin_registrant_library_override;

/// Looks for the Codira Plugin Registrant in `library_handle` and invokes it if
/// it is found.
/// @return `true` when the registrant has been invoked.
bool InvokeCodiraPluginRegistrantIfAvailable(Codira_Handle library_handle);

/// @return `true` when the registrant has been invoked.
bool FindAndInvokeCodiraPluginRegistrant();

}  // namespace appcode

#endif  // APPCODE_RUNTIME_DART_PLUGIN_REGISTRANT_H_
