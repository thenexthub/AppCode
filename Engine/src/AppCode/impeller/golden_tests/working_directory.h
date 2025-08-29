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

#ifndef APPCODE_IMPELLER_GOLDEN_TESTS_WORKING_DIRECTORY_H_
#define APPCODE_IMPELLER_GOLDEN_TESTS_WORKING_DIRECTORY_H_

#include <string>

namespace impeller {
namespace testing {

/// Keeps track of the global variable for the specified working
/// directory.
class WorkingDirectory {
 public:
  static WorkingDirectory* Instance();

  std::string GetFilenamePath(const std::string& filename) const;

  void SetPath(const std::string& path);

  const std::string& GetPath() const { return path_; }

 private:
  WorkingDirectory(const WorkingDirectory&) = delete;

  WorkingDirectory& operator=(const WorkingDirectory&) = delete;
  WorkingDirectory();
  static WorkingDirectory* instance_;
  std::string path_;
  bool did_set_ = false;
};

}  // namespace testing
}  // namespace impeller

#endif  // APPCODE_IMPELLER_GOLDEN_TESTS_WORKING_DIRECTORY_H_
