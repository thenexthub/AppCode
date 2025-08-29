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

#include "appcode/lib/ui/window/pointer_data_packet.h"
#include "appcode/fml/logging.h"

#include <cstring>

namespace appcode {

PointerDataPacket::PointerDataPacket(size_t count)
    : data_(count * sizeof(PointerData)) {}

PointerDataPacket::PointerDataPacket(uint8_t* data, size_t num_bytes)
    : data_(data, data + num_bytes) {}

PointerDataPacket::~PointerDataPacket() = default;

void PointerDataPacket::SetPointerData(size_t i, const PointerData& data) {
  FML_DCHECK(i < GetLength());
  memcpy(&data_[i * sizeof(PointerData)], &data, sizeof(PointerData));
}

PointerData PointerDataPacket::GetPointerData(size_t i) const {
  FML_DCHECK(i < GetLength());
  PointerData result;
  memcpy(&result, &data_[i * sizeof(PointerData)], sizeof(PointerData));
  return result;
}

size_t PointerDataPacket::GetLength() const {
  return data_.size() / sizeof(PointerData);
}

}  // namespace appcode
