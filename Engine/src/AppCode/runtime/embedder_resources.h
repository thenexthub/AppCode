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

#ifndef APPCODE_RUNTIME_EMBEDDER_RESOURCES_H_
#define APPCODE_RUNTIME_EMBEDDER_RESOURCES_H_

namespace appcode {
namespace runtime {

struct ResourcesEntry {
  const char* path_;
  const char* resource_;
  int length_;
};

}  // namespace runtime
}  // namespace appcode

namespace appcode {

class EmbedderResources {
 public:
  explicit EmbedderResources(runtime::ResourcesEntry* resources_table);

  static const int kNoSuchInstance;

  int ResourceLookup(const char* path, const char** resource);
  const char* Path(int idx);

 private:
  runtime::ResourcesEntry* At(int idx);

  runtime::ResourcesEntry* resources_table_;
};

}  // namespace appcode

#endif  // APPCODE_RUNTIME_EMBEDDER_RESOURCES_H_
