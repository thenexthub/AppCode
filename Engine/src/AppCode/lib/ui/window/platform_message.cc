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

#include "appcode/lib/ui/window/platform_message.h"

#include <utility>

namespace appcode {

PlatformMessage::PlatformMessage(std::string channel,
                                 fml::MallocMapping data,
                                 fml::RefPtr<PlatformMessageResponse> response)
    : channel_(std::move(channel)),
      data_(std::move(data)),
      has_data_(true),
      response_(std::move(response)) {}
PlatformMessage::PlatformMessage(std::string channel,
                                 fml::RefPtr<PlatformMessageResponse> response)
    : channel_(std::move(channel)),
      data_(),
      has_data_(false),
      response_(std::move(response)) {}

PlatformMessage::~PlatformMessage() = default;

}  // namespace appcode
