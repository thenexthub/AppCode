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

#include "appcode/shell/platform/common/json_message_codec.h"

#include <iostream>
#include <string>

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace appcode {

// static
const JsonMessageCodec& JsonMessageCodec::GetInstance() {
  static JsonMessageCodec sInstance;
  return sInstance;
}

std::unique_ptr<std::vector<uint8_t>> JsonMessageCodec::EncodeMessageInternal(
    const rapidjson::Document& message) const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  // clang-tidy has trouble reasoning about some of the complicated array and
  // pointer-arithmetic code in rapidjson.
  // NOLINTNEXTLINE(clang-analyzer-core.*)
  message.Accept(writer);
  const char* buffer_start = buffer.GetString();
  return std::make_unique<std::vector<uint8_t>>(
      buffer_start, buffer_start + buffer.GetSize());
}

std::unique_ptr<rapidjson::Document> JsonMessageCodec::DecodeMessageInternal(
    const uint8_t* binary_message,
    const size_t message_size) const {
  auto raw_message = reinterpret_cast<const char*>(binary_message);
  auto json_message = std::make_unique<rapidjson::Document>();
  rapidjson::ParseResult result =
      json_message->Parse(raw_message, message_size);
  if (result.IsError()) {
    std::cerr << "Unable to parse JSON message:" << std::endl
              << rapidjson::GetParseError_En(result.Code()) << std::endl;
    return nullptr;
  }
  return json_message;
}

}  // namespace appcode
