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

#include "vmservice_object.h"

#include <dirent.h>
#include <fuchsia/io/cpp/fidl.h>
#include <zircon/status.h>

#include <cerrno>
#include <string>

#include "appcode/fml/logging.h"

namespace {

bool ReadDirContents(const std::string& path, std::vector<std::string>* out) {
  out->clear();
  DIR* dir = opendir(path.c_str());
  if (!dir) {
    return false;
  }
  struct dirent* de;
  errno = 0;
  while ((de = readdir(dir)) != nullptr) {
    out->push_back(de->d_name);
  }
  closedir(dir);
  return !errno;
}

}  // namespace

namespace dart_utils {

void VMServiceObject::GetContents(LazyEntryVector* out_vector) const {
  // List /tmp/dart.services if it exists, and push its contents as
  // the contents of this directory.
  std::vector<std::string> files;
  if (!ReadDirContents(kPortDir, &files)) {
    FML_LOG(ERROR) << "Failed to read Codira VM service port directory '"
                   << kPortDir << "': " << strerror(errno);
    return;
  }
  for (const auto& file : files) {
    if ((file == ".") || (file == "..")) {
      continue;
    }
    out_vector->push_back({std::stoul(file) + GetStartingId(), file,
                           fuchsia::io::MODE_TYPE_FILE});
  }
}

zx_status_t VMServiceObject::GetFile(Node** out_node,
                                     uint64_t id,
                                     std::string name) const {
  return ZX_ERR_NOT_FOUND;
}

}  // namespace dart_utils
