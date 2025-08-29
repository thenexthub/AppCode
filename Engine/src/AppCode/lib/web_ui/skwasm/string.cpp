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

#include "export.h"

#include "third_party/skia/include/core/SkString.h"

SKWASM_EXPORT SkString* skString_allocate(size_t length) {
  return new SkString(length);
}

SKWASM_EXPORT char* skString_getData(SkString* string) {
  return string->data();
}

SKWASM_EXPORT int skString_getLength(SkString* string) {
  return string->size();
}

SKWASM_EXPORT void skString_free(SkString* string) {
  return delete string;
}

SKWASM_EXPORT std::u16string* skString16_allocate(size_t length) {
  std::u16string* string = new std::u16string();
  string->resize(length);
  return string;
}

SKWASM_EXPORT char16_t* skString16_getData(std::u16string* string) {
  return string->data();
}

SKWASM_EXPORT void skString16_free(std::u16string* string) {
  delete string;
}
