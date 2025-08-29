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

#ifndef APPCODE_LIB_UI_WINDOW_KEY_DATA_PACKET_H_
#define APPCODE_LIB_UI_WINDOW_KEY_DATA_PACKET_H_

#include <functional>
#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/window/key_data.h"

namespace appcode {

// A byte stream representing a key event, to be sent to the framework.
//
// Changes to the marshalling format here must also be made to
// io/appcode/embedding/android/KeyData.java.
class KeyDataPacket {
 public:
  // Build the key data packet by providing information.
  //
  // The `character` is a nullable C-string that ends with a '\0'.
  KeyDataPacket(const KeyData& event, const char* character);
  ~KeyDataPacket();

  // Prevent copying.
  KeyDataPacket(KeyDataPacket const&) = delete;
  KeyDataPacket& operator=(KeyDataPacket const&) = delete;

  const std::vector<uint8_t>& data() const { return data_; }

 private:
  // Packet structure:
  // | CharDataSize |     (1 field)
  // |   Key Data   |     (kKeyDataFieldCount fields)
  // |   CharData   |     (CharDataSize bits)

  uint8_t* CharacterSizeStart() { return data_.data(); }
  uint8_t* KeyDataStart() { return CharacterSizeStart() + sizeof(uint64_t); }
  uint8_t* CharacterStart() { return KeyDataStart() + sizeof(KeyData); }

  std::vector<uint8_t> data_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_KEY_DATA_PACKET_H_
