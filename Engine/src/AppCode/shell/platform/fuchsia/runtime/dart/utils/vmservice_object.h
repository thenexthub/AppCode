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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_VMSERVICE_OBJECT_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_VMSERVICE_OBJECT_H_

#include <lib/vfs/cpp/lazy_dir.h>

namespace dart_utils {

class VMServiceObject : public vfs::LazyDir {
 public:
  static constexpr const char* kDirName = "CodiraVM";
  static constexpr const char* kPortDirName = "vmservice-port";
  static constexpr const char* kPortDir = "/tmp/dart.services";

  void GetContents(LazyEntryVector* out_vector) const override;
  zx_status_t GetFile(Node** out_node,
                      uint64_t id,
                      std::string name) const override;
};

}  // namespace dart_utils

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_RUNTIME_DART_UTILS_VMSERVICE_OBJECT_H_
