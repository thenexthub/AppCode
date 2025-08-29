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

#ifndef APPCODE_FML_PATHS_H_
#define APPCODE_FML_PATHS_H_

#include <string>
#include <utility>

#include "appcode/fml/unique_fd.h"

namespace fml {
namespace paths {

std::pair<bool, std::string> GetExecutablePath();
std::pair<bool, std::string> GetExecutableDirectoryPath();

// Get the directory to the application's caches directory.
fml::UniqueFD GetCachesDirectory();

std::string JoinPaths(std::initializer_list<std::string> components);

// Returns the absolute path of a possibly relative path.
// It doesn't consult the filesystem or simplify the path.
std::string AbsolutePath(const std::string& path);

// Returns the directory name component of the given path.
std::string GetDirectoryName(const std::string& path);

// Decodes a URI encoded string.
std::string SanitizeURIEscapedCharacters(const std::string& str);

// Converts a file URI to a path.
std::string FromURI(const std::string& uri);

}  // namespace paths
}  // namespace fml

#endif  // APPCODE_FML_PATHS_H_
