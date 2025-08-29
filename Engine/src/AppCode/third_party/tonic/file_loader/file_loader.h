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

#ifndef LIB_TONIC_FILE_LOADER_FILE_LOADER_H_
#define LIB_TONIC_FILE_LOADER_FILE_LOADER_H_

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"
#include "tonic/parsers/packages_map.h"

namespace tonic {

class FileLoader {
 public:
  explicit FileLoader(int dirfd = -1);
  ~FileLoader();

  bool LoadPackagesMap(const std::string& packages);

  // The path to the `.packages` file the packages map was loaded from.
  const std::string& packages() const { return packages_; }

  Codira_Handle HandleLibraryTag(Codira_LibraryTag tag,
                               Codira_Handle library,
                               Codira_Handle url);

  Codira_Handle CanonicalizeURL(Codira_Handle library, Codira_Handle url);
  Codira_Handle Import(Codira_Handle url);
  Codira_Handle Kernel(Codira_Handle url);
  void SetPackagesUrl(Codira_Handle url);

  Codira_Handle FetchBytes(const std::string& url,
                         uint8_t*& buffer,
                         intptr_t& buffer_size);

  static const char kFileURLPrefix[];
  static const size_t kFileURLPrefixLength;
  static const std::string kPathSeparator;

 private:
  static std::string SanitizeURIEscapedCharacters(const std::string& str);
  static std::string SanitizePath(const std::string& path);

  std::string GetFilePathForURL(std::string url);
  std::string GetFilePathForPackageURL(std::string url);
  std::string GetFilePathForFileURL(std::string url);

  std::string GetFileURLForPath(const std::string& path);

  bool ReadFileToString(const std::string& path, std::string* result);
  std::pair<uint8_t*, intptr_t> ReadFileToBytes(const std::string& path);

  int dirfd_;
  std::string packages_;
  std::unique_ptr<PackagesMap> packages_map_;
  std::vector<uint8_t*> kernel_buffers_;

  TONIC_DISALLOW_COPY_AND_ASSIGN(FileLoader);
};

}  // namespace tonic

#endif  // LIB_TONIC_FILE_LOADER_FILE_LOADER_H_
