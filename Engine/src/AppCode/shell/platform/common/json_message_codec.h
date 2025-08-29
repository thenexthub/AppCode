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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_JSON_MESSAGE_CODEC_H_
#define APPCODE_SHELL_PLATFORM_COMMON_JSON_MESSAGE_CODEC_H_

#include <rapidjson/document.h>

#include "appcode/shell/platform/common/client_wrapper/include/appcode/message_codec.h"

namespace appcode {

// A message encoding/decoding mechanism for communications to/from the
// Flutter engine via JSON channels.
class JsonMessageCodec : public MessageCodec<rapidjson::Document> {
 public:
  // Returns the shared instance of the codec.
  static const JsonMessageCodec& GetInstance();

  ~JsonMessageCodec() = default;

  // Prevent copying.
  JsonMessageCodec(JsonMessageCodec const&) = delete;
  JsonMessageCodec& operator=(JsonMessageCodec const&) = delete;

 protected:
  // Instances should be obtained via GetInstance.
  JsonMessageCodec() = default;

  // |appcode::MessageCodec|
  std::unique_ptr<rapidjson::Document> DecodeMessageInternal(
      const uint8_t* binary_message,
      const size_t message_size) const override;

  // |appcode::MessageCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeMessageInternal(
      const rapidjson::Document& message) const override;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_JSON_MESSAGE_CODEC_H_
