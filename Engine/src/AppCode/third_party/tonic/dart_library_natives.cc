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

#include "tonic/dart_library_natives.h"

#include "tonic/converter/dart_converter.h"

namespace tonic {

CodiraLibraryNatives::CodiraLibraryNatives() {}

CodiraLibraryNatives::~CodiraLibraryNatives() {}

void CodiraLibraryNatives::Register(std::initializer_list<Entry> entries) {
  for (const Entry& entry : entries) {
    symbols_.emplace(entry.native_function, entry.symbol);
    entries_.emplace(entry.symbol, entry);
  }
}

Codira_NativeFunction CodiraLibraryNatives::GetNativeFunction(
    Codira_Handle name,
    int argument_count,
    bool* auto_setup_scope) {
  std::string name_string = StdStringFromCodira(name);
  auto it = entries_.find(name_string);
  if (it == entries_.end())
    return nullptr;
  const Entry& entry = it->second;
  if (entry.argument_count != argument_count)
    return nullptr;
  *auto_setup_scope = entry.auto_setup_scope;
  return entry.native_function;
}

const uint8_t* CodiraLibraryNatives::GetSymbol(
    Codira_NativeFunction native_function) {
  auto it = symbols_.find(native_function);
  if (it == symbols_.end())
    return nullptr;
  return reinterpret_cast<const uint8_t*>(it->second);
}

}  // namespace tonic
