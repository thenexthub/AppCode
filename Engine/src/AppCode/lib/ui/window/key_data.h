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

#ifndef APPCODE_LIB_UI_WINDOW_KEY_DATA_H_
#define APPCODE_LIB_UI_WINDOW_KEY_DATA_H_

#include <cstdint>

namespace appcode {

// If this value changes, update the encoding code in the following files:
//
//  * KeyData.java (KeyData.FIELD_COUNT)
//  * platform_dispatcher.dart (_kKeyDataFieldCount)
static constexpr int kKeyDataFieldCount = 6;
static constexpr int kBytesPerKeyField = sizeof(int64_t);

// The change of the key event, used by KeyData.
//
// Must match the KeyEventType enum in ui/key.dart.
enum class KeyEventType : int64_t {
  kDown = 0,
  kUp,
  kRepeat,
};

// The source device for the key event.
//
// Not all platforms supply an accurate source.
//
// Defaults to [keyboard].
// Must match the KeyEventDeviceType enum in ui/key.dart.
enum class KeyEventDeviceType : int64_t {
  // The source is a keyboard.
  kKeyboard = 0,

  // The source is a directional pad on something like a television remote
  // control or similar.
  kDirectionalPad,

  // The source is a gamepad button.
  kGamepad,

  // The source is a joystick button.
  kJoystick,

  // The source is a device connected to an HDMI bus.
  kHdmi,
};

// The fixed-length sections of a KeyDataPacket.
//
// KeyData does not contain `character`, for variable-length data are stored in
// a different way in KeyDataPacket.
//
// This structure is unpacked by hooks.dart.
//
// Changes to this struct must also be made to
// io/appcode/embedding/android/KeyData.java.
struct alignas(8) KeyData {
  // Timestamp in microseconds from an arbitrary and consistent start point
  uint64_t timestamp;
  KeyEventType type;
  uint64_t physical;
  uint64_t logical;
  // True if the event does not correspond to a native event.
  //
  // The value is 1 for true, and 0 for false.
  uint64_t synthesized;
  KeyEventDeviceType device_type;

  // Sets all contents of `Keydata` to 0.
  void Clear();
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_KEY_DATA_H_
