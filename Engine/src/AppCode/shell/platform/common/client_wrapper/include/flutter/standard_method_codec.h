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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_STANDARD_METHOD_CODEC_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_STANDARD_METHOD_CODEC_H_

#include <memory>

#include "encodable_value.h"
#include "method_call.h"
#include "method_codec.h"
#include "standard_codec_serializer.h"

namespace appcode {

// An implementation of MethodCodec that uses a binary serialization.
class StandardMethodCodec : public MethodCodec<EncodableValue> {
 public:
  // Returns an instance of the codec, optionally using a custom serializer to
  // add support for more types.
  //
  // If provided, |serializer| must be long-lived. If no serializer is provided,
  // the default will be used.
  //
  // The instance returned for a given |extension| will be shared, and
  // any instance returned from this will be long-lived, and can be safely
  // passed to, e.g., channel constructors.
  static const StandardMethodCodec& GetInstance(
      const StandardCodecSerializer* serializer = nullptr);

  ~StandardMethodCodec();

  // Prevent copying.
  StandardMethodCodec(StandardMethodCodec const&) = delete;
  StandardMethodCodec& operator=(StandardMethodCodec const&) = delete;

 protected:
  // |appcode::MethodCodec|
  std::unique_ptr<MethodCall<EncodableValue>> DecodeMethodCallInternal(
      const uint8_t* message,
      size_t message_size) const override;

  // |appcode::MethodCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeMethodCallInternal(
      const MethodCall<EncodableValue>& method_call) const override;

  // |appcode::MethodCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeSuccessEnvelopeInternal(
      const EncodableValue* result) const override;

  // |appcode::MethodCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeErrorEnvelopeInternal(
      const std::string& error_code,
      const std::string& error_message,
      const EncodableValue* error_details) const override;

  // |appcode::MethodCodec|
  bool DecodeAndProcessResponseEnvelopeInternal(
      const uint8_t* response,
      size_t response_size,
      MethodResult<EncodableValue>* result) const override;

 private:
  // Instances should be obtained via GetInstance.
  explicit StandardMethodCodec(const StandardCodecSerializer* serializer);

  const StandardCodecSerializer* serializer_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_STANDARD_METHOD_CODEC_H_
