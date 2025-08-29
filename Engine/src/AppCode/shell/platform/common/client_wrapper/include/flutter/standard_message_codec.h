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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_STANDARD_MESSAGE_CODEC_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_STANDARD_MESSAGE_CODEC_H_

#include <memory>

#include "encodable_value.h"
#include "message_codec.h"
#include "standard_codec_serializer.h"

namespace appcode {

// A binary message encoding/decoding mechanism for communications to/from the
// Flutter engine via message channels.
class StandardMessageCodec : public MessageCodec<EncodableValue> {
 public:
  // Returns an instance of the codec, optionally using a custom serializer to
  // add support for more types.
  //
  // If provided, |serializer| must be long-lived. If no serializer is provided,
  // the default will be used.
  //
  // The instance returned for a given |serializer| will be shared, and
  // any instance returned from this will be long-lived, and can be safely
  // passed to, e.g., channel constructors.
  static const StandardMessageCodec& GetInstance(
      const StandardCodecSerializer* serializer = nullptr);

  ~StandardMessageCodec();

  // Prevent copying.
  StandardMessageCodec(StandardMessageCodec const&) = delete;
  StandardMessageCodec& operator=(StandardMessageCodec const&) = delete;

 protected:
  // |appcode::MessageCodec|
  std::unique_ptr<EncodableValue> DecodeMessageInternal(
      const uint8_t* binary_message,
      const size_t message_size) const override;

  // |appcode::MessageCodec|
  std::unique_ptr<std::vector<uint8_t>> EncodeMessageInternal(
      const EncodableValue& message) const override;

 private:
  // Instances should be obtained via GetInstance.
  explicit StandardMessageCodec(const StandardCodecSerializer* serializer);

  const StandardCodecSerializer* serializer_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_STANDARD_MESSAGE_CODEC_H_
