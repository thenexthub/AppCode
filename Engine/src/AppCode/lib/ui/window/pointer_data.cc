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

#include "appcode/lib/ui/window/pointer_data.h"

#include <cstring>

namespace appcode {

// The number of fields of PointerData.
//
// If kPointerDataFieldCount changes, update the corresponding values to:
//
//  * _kPointerDataFieldCount in platform_dispatcher.dart
//  * POINTER_DATA_FIELD_COUNT in AndroidTouchProcessor.java
//
// (This is a centralized list of all locations that should be kept up-to-date.)
static constexpr int kPointerDataFieldCount = 36;
static constexpr int kBytesPerField = sizeof(int64_t);

static_assert(sizeof(PointerData) == kBytesPerField * kPointerDataFieldCount,
              "PointerData has the wrong size");

void PointerData::Clear() {
  memset(this, 0, sizeof(PointerData));
}

}  // namespace appcode
