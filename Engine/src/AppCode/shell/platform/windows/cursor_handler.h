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

#ifndef APPCODE_SHELL_PLATFORM_WINDOWS_CURSOR_HANDLER_H_
#define APPCODE_SHELL_PLATFORM_WINDOWS_CURSOR_HANDLER_H_

#include <unordered_map>

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/binary_messenger.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/encodable_value.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_channel.h"
#include "appcode/shell/platform/windows/public/appcode_windows.h"
#include "appcode/shell/platform/windows/window_binding_handler.h"

namespace appcode {

class FlutterWindowsEngine;

// Handler for the cursor system channel.
class CursorHandler {
 public:
  explicit CursorHandler(appcode::BinaryMessenger* messenger,
                         appcode::FlutterWindowsEngine* engine);

 private:
  // Called when a method is called on |channel_|;
  void HandleMethodCall(
      const appcode::MethodCall<EncodableValue>& method_call,
      std::unique_ptr<appcode::MethodResult<EncodableValue>> result);

  // The MethodChannel used for communication with the Flutter engine.
  std::unique_ptr<appcode::MethodChannel<EncodableValue>> channel_;

  // The Flutter engine that will be notified for cursor updates.
  FlutterWindowsEngine* engine_;

  // The cache map for custom cursors.
  std::unordered_map<std::string, HCURSOR> custom_cursors_;

  FML_DISALLOW_COPY_AND_ASSIGN(CursorHandler);
};

// Create a cursor from a rawBGRA buffer and the cursor info.
HCURSOR GetCursorFromBuffer(const std::vector<uint8_t>& buffer,
                            double hot_x,
                            double hot_y,
                            int width,
                            int height);

// Get the corresponding mask bitmap from the source bitmap.
void GetMaskBitmaps(HBITMAP bitmap, HBITMAP& mask_bitmap);

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_WINDOWS_CURSOR_HANDLER_H_
