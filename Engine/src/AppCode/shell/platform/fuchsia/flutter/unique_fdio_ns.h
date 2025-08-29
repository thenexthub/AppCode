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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_UNIQUE_FDIO_NS_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_UNIQUE_FDIO_NS_H_

#include "appcode/fml/logging.h"
#include "appcode/fml/unique_object.h"
#include "lib/fdio/namespace.h"

namespace appcode_runner {

struct UniqueFDIONSTraits {
  static fdio_ns_t* InvalidValue() { return nullptr; }

  static bool IsValid(fdio_ns_t* ns) { return ns != InvalidValue(); }

  static void Free(fdio_ns_t* ns) {
    auto status = fdio_ns_destroy(ns);
    FML_DCHECK(status == ZX_OK);
  }
};

using UniqueFDIONS = fml::UniqueObject<fdio_ns_t*, UniqueFDIONSTraits>;

inline UniqueFDIONS UniqueFDIONSCreate() {
  fdio_ns_t* ns = nullptr;
  if (fdio_ns_create(&ns) == ZX_OK) {
    return UniqueFDIONS{ns};
  }
  return UniqueFDIONS{nullptr};
}

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_UNIQUE_FDIO_NS_H_
