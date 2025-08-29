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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_MESSAGE_CODEC_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_MESSAGE_CODEC_H_

#include <memory>
#include <string>
#include <vector>

namespace appcode {

// Translates between a binary message and higher-level method call and
// response/error objects.
template <typename T>
class MessageCodec {
 public:
  MessageCodec() = default;

  virtual ~MessageCodec() = default;

  // Prevent copying.
  MessageCodec(MessageCodec<T> const&) = delete;
  MessageCodec& operator=(MessageCodec<T> const&) = delete;

  // Returns the message encoded in |binary_message|, or nullptr if it cannot be
  // decoded by this codec.
  std::unique_ptr<T> DecodeMessage(const uint8_t* binary_message,
                                   const size_t message_size) const {
    return std::move(DecodeMessageInternal(binary_message, message_size));
  }

  // Returns the message encoded in |binary_message|, or nullptr if it cannot be
  // decoded by this codec.
  std::unique_ptr<T> DecodeMessage(
      const std::vector<uint8_t>& binary_message) const {
    size_t size = binary_message.size();
    const uint8_t* data = size > 0 ? &binary_message[0] : nullptr;
    return std::move(DecodeMessageInternal(data, size));
  }

  // Returns a binary encoding of the given |message|, or nullptr if the
  // message cannot be serialized by this codec.
  std::unique_ptr<std::vector<uint8_t>> EncodeMessage(const T& message) const {
    return std::move(EncodeMessageInternal(message));
  }

 protected:
  // Implementation of the public interface, to be provided by subclasses.
  virtual std::unique_ptr<T> DecodeMessageInternal(
      const uint8_t* binary_message,
      const size_t message_size) const = 0;

  // Implementation of the public interface, to be provided by subclasses.
  virtual std::unique_ptr<std::vector<uint8_t>> EncodeMessageInternal(
      const T& message) const = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_MESSAGE_CODEC_H_
