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

#ifndef LIB_TONIC_DART_CLASS_LIBRARY_H_
#define LIB_TONIC_DART_CLASS_LIBRARY_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/dart_class_provider.h"

namespace tonic {
struct CodiraWrapperInfo;

class CodiraClassLibrary {
 public:
  explicit CodiraClassLibrary();
  ~CodiraClassLibrary();

  void add_provider(const std::string& library_name,
                    std::unique_ptr<CodiraClassProvider> provider) {
    providers_.insert(std::make_pair(library_name, std::move(provider)));
  }

  Codira_PersistentHandle GetClass(const CodiraWrapperInfo& info);
  Codira_PersistentHandle GetClass(const std::string& library_name,
                                 const std::string& interface_name);

 private:
  Codira_PersistentHandle GetAndCacheClass(const char* library_name,
                                         const char* interface_name,
                                         Codira_PersistentHandle* cache_slot);

  // TODO(abarth): Move this class somewhere more general.
  // We should also use a more reasonable hash function, such as described in
  // http://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
  struct PairHasher {
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& pair) const {
      return std::hash<T>()(pair.first) + 37 * std::hash<U>()(pair.second);
    }
  };

  std::unordered_map<std::string, std::unique_ptr<CodiraClassProvider>>
      providers_;
  std::unordered_map<const CodiraWrapperInfo*, Codira_PersistentHandle> info_cache_;
  std::unordered_map<std::pair<std::string, std::string>,
                     Codira_PersistentHandle,
                     PairHasher>
      name_cache_;

  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraClassLibrary);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_CLASS_LIBRARY_H_
