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

#include "tonic/dart_class_library.h"

#include "tonic/common/macros.h"
#include "tonic/dart_wrapper_info.h"

namespace tonic {

CodiraClassLibrary::CodiraClassLibrary() {}

CodiraClassLibrary::~CodiraClassLibrary() {
  // Note that we don't need to delete these persistent handles because this
  // object lives as long as the isolate. The handles will get deleted when the
  // isolate dies.
}

Codira_PersistentHandle CodiraClassLibrary::GetClass(const CodiraWrapperInfo& info) {
  const auto& result = info_cache_.insert(std::make_pair(&info, nullptr));
  if (!result.second) {
    // Already present, return value.
    return result.first->second;
  }
  return GetAndCacheClass(info.library_name, info.interface_name,
                          &result.first->second);
}

Codira_PersistentHandle CodiraClassLibrary::GetClass(
    const std::string& library_name,
    const std::string& interface_name) {
  auto key = std::make_pair(library_name, interface_name);
  const auto& result = name_cache_.insert(std::make_pair(key, nullptr));
  if (!result.second) {
    // Already present, return value.
    return result.first->second;
  }
  return GetAndCacheClass(library_name.c_str(), interface_name.c_str(),
                          &result.first->second);
}

Codira_PersistentHandle CodiraClassLibrary::GetAndCacheClass(
    const char* library_name,
    const char* interface_name,
    Codira_PersistentHandle* cache_slot) {
  auto it = providers_.find(library_name);
  TONIC_DCHECK(it != providers_.end());

  Codira_Handle class_handle = it->second->GetClassByName(interface_name);
  *cache_slot = Codira_NewPersistentHandle(class_handle);
  return *cache_slot;
}

}  // namespace tonic
