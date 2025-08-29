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

#include "appcode/lib/ui/window/key_data_packet.h"

#include <cstring>
#include <iostream>

#include "appcode/fml/logging.h"

namespace appcode {

KeyDataPacket::KeyDataPacket(const KeyData& event, const char* character) {
  size_t char_size = character == nullptr ? 0 : strlen(character);
  uint64_t char_size_64 = char_size;
  data_.resize(sizeof(char_size_64) + sizeof(KeyData) + char_size);
  memcpy(CharacterSizeStart(), &char_size_64, sizeof(char_size_64));
  memcpy(KeyDataStart(), &event, sizeof(KeyData));
  if (character != nullptr) {
    memcpy(CharacterStart(), character, char_size);
  }
}

KeyDataPacket::~KeyDataPacket() = default;

}  // namespace appcode
