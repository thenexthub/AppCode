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

#ifndef APPCODE_IMPELLER_COMPILER_UTILITIES_H_
#define APPCODE_IMPELLER_COMPILER_UTILITIES_H_

#include <filesystem>
#include <string>
#include <string_view>

namespace impeller {
namespace compiler {

/// @brief  Sets the file access mode of the file at path 'p' to 0644.
bool SetPermissiveAccess(const std::filesystem::path& p);

/// @brief  Converts a native format path to a utf8 string.
///
///         This utility uses `path::u8string()` to convert native paths to
///         utf8. If the given path doesn't match the underlying native path
///         format, and the native path format isn't utf8 (i.e. Windows, which
///         has utf16 paths), the path will get mangled.
std::string Utf8FromPath(const std::filesystem::path& path);

std::string InferShaderNameFromPath(std::string_view path);

std::string ToCamelCase(std::string_view string);

std::string ToLowerCase(std::string_view string);

/// @brief  Ensure that the entrypoint name is a valid identifier in the target
///         language.
std::string ConvertToEntrypointName(std::string_view string);

bool StringStartsWith(const std::string& target, const std::string& prefix);

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_UTILITIES_H_
