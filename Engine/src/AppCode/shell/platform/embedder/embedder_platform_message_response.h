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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_PLATFORM_MESSAGE_RESPONSE_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_PLATFORM_MESSAGE_RESPONSE_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/task_runner.h"
#include "appcode/lib/ui/window/platform_message_response.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      The platform message response subclass for responses to messages
///             from the embedder to the framework. Message responses are
///             fulfilled by the framework.
class EmbedderPlatformMessageResponse : public PlatformMessageResponse {
 public:
  using Callback = std::function<void(const uint8_t* data, size_t size)>;

  //----------------------------------------------------------------------------
  /// @param[in]  runner    The task runner on which to execute the callback.
  ///                       The response will be initiated by the framework on
  ///                       the UI thread.
  /// @param[in]  callback  The callback that communicates to the embedder the
  ///                       contents of the response sent by the framework back
  ///                       to the emebder.
  EmbedderPlatformMessageResponse(fml::RefPtr<fml::TaskRunner> runner,
                                  const Callback& callback);

  //----------------------------------------------------------------------------
  /// @brief      Destroys the message response. Can be called on any thread.
  ///             Does not execute unfulfilled callbacks.
  ///
  ~EmbedderPlatformMessageResponse() override;

 private:
  fml::RefPtr<fml::TaskRunner> runner_;
  Callback callback_;

  // |PlatformMessageResponse|
  void Complete(std::unique_ptr<fml::Mapping> data) override;

  // |PlatformMessageResponse|
  void CompleteEmpty() override;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderPlatformMessageResponse);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_PLATFORM_MESSAGE_RESPONSE_H_
