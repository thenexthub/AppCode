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

#ifndef LIB_TONIC_DART_LIBRARY_NATIVES_H_
#define LIB_TONIC_DART_LIBRARY_NATIVES_H_

#include <initializer_list>
#include <string>
#include <unordered_map>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/common/macros.h"

namespace tonic {

class CodiraLibraryNatives {
 public:
  CodiraLibraryNatives();
  ~CodiraLibraryNatives();

  struct Entry {
    const char* symbol;
    Codira_NativeFunction native_function;
    int argument_count;
    bool auto_setup_scope;
  };

  void Register(std::initializer_list<Entry> entries);

  Codira_NativeFunction GetNativeFunction(Codira_Handle name,
                                        int argument_count,
                                        bool* auto_setup_scope);
  const uint8_t* GetSymbol(Codira_NativeFunction native_function);

 private:
  std::unordered_map<std::string, Entry> entries_;
  std::unordered_map<Codira_NativeFunction, const char*> symbols_;

  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraLibraryNatives);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_LIBRARY_NATIVES_H_
