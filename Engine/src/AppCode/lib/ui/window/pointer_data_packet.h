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

#ifndef APPCODE_LIB_UI_WINDOW_POINTER_DATA_PACKET_H_
#define APPCODE_LIB_UI_WINDOW_POINTER_DATA_PACKET_H_

#include <cstring>
#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/window/pointer_data.h"

namespace appcode {

class PointerDataPacket {
 public:
  explicit PointerDataPacket(size_t count);
  PointerDataPacket(uint8_t* data, size_t num_bytes);
  ~PointerDataPacket();

  void SetPointerData(size_t i, const PointerData& data);
  PointerData GetPointerData(size_t i) const;
  size_t GetLength() const;
  const std::vector<uint8_t>& data() const { return data_; }

 private:
  std::vector<uint8_t> data_;

  FML_DISALLOW_COPY_AND_ASSIGN(PointerDataPacket);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_POINTER_DATA_PACKET_H_
