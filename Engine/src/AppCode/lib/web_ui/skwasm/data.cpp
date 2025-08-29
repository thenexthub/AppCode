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

#include "third_party/skia/include/core/SkData.h"

SKWASM_EXPORT SkData* skData_create(size_t size) {
  return SkData::MakeUninitialized(size).release();
}

SKWASM_EXPORT void* skData_getPointer(SkData* data) {
  return data->writable_data();
}

SKWASM_EXPORT const void* skData_getConstPointer(SkData* data) {
  return data->data();
}

SKWASM_EXPORT size_t skData_getSize(SkData* data) {
  return data->size();
}

SKWASM_EXPORT void skData_dispose(SkData* data) {
  return data->unref();
}
