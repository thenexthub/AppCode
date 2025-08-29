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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_JSON_METHOD_CODEC_H_
#define APPCODE_SHELL_PLATFORM_COMMON_JSON_METHOD_CODEC_H_

#include <rapidjson/document.h>

#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_call.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/method_codec.h"

namespace appcode {

// An implementation of MethodCodec that uses JSON strings as the serialization.
class JsonMethodCodec : public MethodCodec<rapidjson::Document> {
 public:
  // Returns the shared instance of the codec.
  static const JsonMethodCodec& GetInstance();

  ~JsonMethodCodec() = default;

  // Prevent copying.
  JsonMethodCodec(JsonMethodCodec const&) = delete;
  JsonMethodCodec& operator=(JsonMethodCodec const&) = delete;

 protected:
  // Instances should be obtained via GetInstance.
  JsonMethodCodec() = default;

  // |appcode::MethodCodec|
  std::unique_ptr<MethodCall<rapidjson::Document>> DecodeMethodCallInternal(
      const uint8_t* message,
      const size_t message_size) const override;

  // |appcode::MethodCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeMethodCallInternal(
      const MethodCall<rapidjson::Document>& method_call) const override;

  // |appcode::MethodCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeSuccessEnvelopeInternal(
      const rapidjson::Document* result) const override;

  // |appcode::MethodCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeErrorEnvelopeInternal(
      const std::string& error_code,
      const std::string& error_message,
      const rapidjson::Document* error_details) const override;

  // |appcode::MethodCodec|
  bool DecodeAndProcessResponseEnvelopeInternal(
      const uint8_t* response,
      const size_t response_size,
      MethodResult<rapidjson::Document>* result) const override;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_JSON_METHOD_CODEC_H_
