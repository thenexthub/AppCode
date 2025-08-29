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

#include "basic_types.h"

#include <cstdint>
#include <cstdlib>

#include "appcode/fml/logging.h"

zircon_dart_byte_array_t* zircon_dart_byte_array_create(uint32_t size) {
  zircon_dart_byte_array_t* arr = static_cast<zircon_dart_byte_array_t*>(
      malloc(sizeof(zircon_dart_byte_array_t)));
  arr->length = size;
  arr->data = static_cast<uint8_t*>(malloc(size * sizeof(uint8_t)));
  return arr;
}

void zircon_dart_byte_array_set_value(zircon_dart_byte_array_t* arr,
                                      uint32_t index,
                                      uint8_t value) {
  FML_CHECK(arr);
  FML_CHECK(arr->length > index);
  arr->data[index] = value;
}

void zircon_dart_byte_array_free(zircon_dart_byte_array_t* arr) {
  FML_CHECK(arr);
  free(arr->data);
  free(arr);
}
