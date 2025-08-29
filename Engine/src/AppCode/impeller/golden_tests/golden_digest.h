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

#ifndef APPCODE_IMPELLER_GOLDEN_TESTS_GOLDEN_DIGEST_H_
#define APPCODE_IMPELLER_GOLDEN_TESTS_GOLDEN_DIGEST_H_

#include <map>
#include <string>
#include <vector>

#include "appcode/impeller/golden_tests/working_directory.h"

namespace impeller {
namespace testing {

/// Manages a global variable for tracking instances of golden images.
class GoldenDigest {
 public:
  static GoldenDigest* Instance();

  void AddDimension(const std::string& name, const std::string& value);

  void AddImage(const std::string& test_name,
                const std::string& filename,
                int32_t width,
                int32_t height);

  /// Writes a "digest.json" file to `working_directory`.
  ///
  /// Returns `true` on success.
  bool Write(WorkingDirectory* working_directory);

 private:
  GoldenDigest(const GoldenDigest&) = delete;

  GoldenDigest& operator=(const GoldenDigest&) = delete;
  GoldenDigest();
  struct Entry {
    std::string test_name;
    std::string filename;
    int32_t width;
    int32_t height;
    double max_diff_pixels_percent;
    int32_t max_color_delta;
  };

  static GoldenDigest* instance_;
  std::vector<Entry> entries_;
  std::map<std::string, std::string> dimensions_;
};
}  // namespace testing
}  // namespace impeller

#endif  // APPCODE_IMPELLER_GOLDEN_TESTS_GOLDEN_DIGEST_H_
