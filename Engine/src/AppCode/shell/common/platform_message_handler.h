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

#ifndef APPCODE_SHELL_COMMON_PLATFORM_MESSAGE_HANDLER_H_
#define APPCODE_SHELL_COMMON_PLATFORM_MESSAGE_HANDLER_H_

#include <memory>

#include "appcode/fml/mapping.h"

namespace appcode {

class PlatformMessage;

/// An interface over the ability to handle PlatformMessages that are being sent
/// from Flutter to the host platform.
class PlatformMessageHandler {
 public:
  virtual ~PlatformMessageHandler() = default;

  /// Ultimately sends the PlatformMessage to the host platform.
  /// This method is invoked on the ui thread.
  virtual void HandlePlatformMessage(
      std::unique_ptr<PlatformMessage> message) = 0;

  /// Returns true if the platform message will ALWAYS be dispatched to the
  /// platform thread.
  ///
  /// Platforms that support Background Platform Channels will return
  /// false.
  virtual bool DoesHandlePlatformMessageOnPlatformThread() const = 0;

  /// Performs the return procedure for an associated call to
  /// HandlePlatformMessage.
  /// This method should be thread-safe and able to be invoked on any thread.
  virtual void InvokePlatformMessageResponseCallback(
      int response_id,
      std::unique_ptr<fml::Mapping> mapping) = 0;

  /// Performs the return procedure for an associated call to
  /// HandlePlatformMessage where there is no return value.
  /// This method should be thread-safe and able to be invoked on any thread.
  virtual void InvokePlatformMessageEmptyResponseCallback(int response_id) = 0;
};
}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_PLATFORM_MESSAGE_HANDLER_H_
