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

#ifndef APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_H_
#define APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_H_

#include <string>
#include <vector>

#include "appcode/fml/memory/ref_counted.h"
#include "appcode/fml/memory/ref_ptr.h"
#include "appcode/lib/ui/window/platform_message_response.h"

namespace appcode {

class PlatformMessage {
 public:
  PlatformMessage(std::string channel,
                  fml::MallocMapping data,
                  fml::RefPtr<PlatformMessageResponse> response);
  PlatformMessage(std::string channel,
                  fml::RefPtr<PlatformMessageResponse> response);
  ~PlatformMessage();

  const std::string& channel() const { return channel_; }
  const fml::MallocMapping& data() const { return data_; }
  bool hasData() { return has_data_; }

  const fml::RefPtr<PlatformMessageResponse>& response() const {
    return response_;
  }

  fml::MallocMapping releaseData() { return std::move(data_); }

 private:
  std::string channel_;
  fml::MallocMapping data_;
  bool has_data_;
  fml::RefPtr<PlatformMessageResponse> response_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_WINDOW_PLATFORM_MESSAGE_H_
