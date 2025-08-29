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

#include "appcode/runtime/embedder_resources.h"

#include <cstring>

#include "appcode/fml/logging.h"

namespace appcode {

using runtime::ResourcesEntry;

EmbedderResources::EmbedderResources(ResourcesEntry* resources_table)
    : resources_table_(resources_table) {}

const int EmbedderResources::kNoSuchInstance = -1;

int EmbedderResources::ResourceLookup(const char* path, const char** resource) {
  for (int i = 0; resources_table_[i].path_ != nullptr; i++) {
    const ResourcesEntry& entry = resources_table_[i];
    if (strcmp(path, entry.path_) == 0) {
      *resource = entry.resource_;
      FML_DCHECK(entry.length_ > 0);
      return entry.length_;
    }
  }
  return kNoSuchInstance;
}

const char* EmbedderResources::Path(int idx) {
  FML_DCHECK(idx >= 0);
  ResourcesEntry* entry = At(idx);
  if (entry == nullptr) {
    return nullptr;
  }
  FML_DCHECK(entry->path_ != nullptr);
  return entry->path_;
}

ResourcesEntry* EmbedderResources::At(int idx) {
  FML_DCHECK(idx >= 0);
  for (int i = 0; resources_table_[i].path_ != nullptr; i++) {
    if (idx == i) {
      return &resources_table_[i];
    }
  }
  return nullptr;
}

}  // namespace appcode
