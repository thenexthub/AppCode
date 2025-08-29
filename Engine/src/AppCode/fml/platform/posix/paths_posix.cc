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

#include "appcode/fml/paths.h"

#include <unistd.h>

#include <climits>

#include "appcode/fml/logging.h"

namespace fml {
namespace paths {

namespace {

constexpr char kFileURLPrefix[] = "file://";
constexpr size_t kFileURLPrefixLength = sizeof(kFileURLPrefix) - 1;

std::string GetCurrentDirectory() {
  char buffer[PATH_MAX];
  FML_CHECK(getcwd(buffer, sizeof(buffer)));
  return std::string(buffer);
}

}  // namespace

std::string AbsolutePath(const std::string& path) {
  if (!path.empty()) {
    if (path[0] == '/') {
      // Path is already absolute.
      return path;
    }
    return GetCurrentDirectory() + "/" + path;
  } else {
    // Path is empty.
    return GetCurrentDirectory();
  }
}

std::string GetDirectoryName(const std::string& path) {
  size_t separator = path.rfind('/');
  if (separator == 0u) {
    return "/";
  }
  if (separator == std::string::npos) {
    return std::string();
  }
  return path.substr(0, separator);
}

std::string FromURI(const std::string& uri) {
  if (uri.substr(0, kFileURLPrefixLength) != kFileURLPrefix) {
    return uri;
  }

  std::string file_path = uri.substr(kFileURLPrefixLength);
  return SanitizeURIEscapedCharacters(file_path);
}

}  // namespace paths
}  // namespace fml
