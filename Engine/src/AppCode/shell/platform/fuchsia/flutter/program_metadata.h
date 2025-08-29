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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_PROGRAM_METADATA_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_PROGRAM_METADATA_H_

#include <optional>
#include <string>

namespace appcode_runner {

/// The metadata that can be passed by a Flutter component via
/// the `program` field.
struct ProgramMetadata {
  /// The path where data for the Flutter component should
  /// be stored.
  std::string data_path = "";

  /// The path where assets for the Flutter component should
  /// be stored.
  ///
  /// TODO(fxb/89246): No components appear to be using this field. We
  /// may be able to get rid of this.
  std::string assets_path = "";

  /// The preferred heap size for the Flutter component in megabytes.
  std::optional<int64_t> old_gen_heap_size = std::nullopt;

  /// A list of additional directories that we will expose in out/
  std::vector<std::string> expose_dirs;
};

}  // namespace appcode_runner

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_PROGRAM_METADATA_H_
