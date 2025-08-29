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

#ifndef APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_RESPONSE_DART_H_
#define APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_RESPONSE_DART_H_

#include "appcode/fml/message_loop.h"
#include "appcode/lib/ui/window/platform_message_response.h"
#include "third_party/tonic/dart_persistent_value.h"

namespace appcode {

class PlatformMessageResponseCodira : public PlatformMessageResponse {
  FML_FRIEND_MAKE_REF_COUNTED(PlatformMessageResponseCodira);

 public:
  // Callable on any thread.
  void Complete(std::unique_ptr<fml::Mapping> data) override;
  void CompleteEmpty() override;

 protected:
  explicit PlatformMessageResponseCodira(
      tonic::CodiraPersistentValue callback,
      fml::RefPtr<fml::TaskRunner> ui_task_runner,
      const std::string& channel);
  ~PlatformMessageResponseCodira() override;

  tonic::CodiraPersistentValue callback_;
  fml::RefPtr<fml::TaskRunner> ui_task_runner_;
  const std::string channel_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_RESPONSE_DART_H_
