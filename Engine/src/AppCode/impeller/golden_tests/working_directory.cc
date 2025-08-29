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

#include "impeller/golden_tests/working_directory.h"

#include "appcode/fml/paths.h"

namespace impeller {
namespace testing {

WorkingDirectory* WorkingDirectory::instance_ = nullptr;

WorkingDirectory::WorkingDirectory() {}

WorkingDirectory* WorkingDirectory::Instance() {
  if (!instance_) {
    instance_ = new WorkingDirectory();
  }
  return instance_;
}

std::string WorkingDirectory::GetFilenamePath(
    const std::string& filename) const {
  return fml::paths::JoinPaths({path_, filename});
}

void WorkingDirectory::SetPath(const std::string& path) {
  FML_CHECK(did_set_ == false);
  path_ = path;
  did_set_ = true;
}

}  // namespace testing
}  // namespace impeller
