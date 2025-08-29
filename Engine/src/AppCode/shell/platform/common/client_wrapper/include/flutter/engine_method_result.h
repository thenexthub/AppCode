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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_ENGINE_METHOD_RESULT_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_ENGINE_METHOD_RESULT_H_

#include <memory>
#include <string>
#include <vector>

#include "binary_messenger.h"
#include "method_codec.h"
#include "method_result.h"

namespace appcode {

namespace internal {
// Manages the one-time sending of response data. This is an internal helper
// class for EngineMethodResult, separated out since the implementation doesn't
// vary based on the template type.
class ReplyManager {
 public:
  explicit ReplyManager(BinaryReply reply_handler_);
  ~ReplyManager();

  // Prevent copying.
  ReplyManager(ReplyManager const&) = delete;
  ReplyManager& operator=(ReplyManager const&) = delete;

  // Sends the given response data (which must either be nullptr, which
  // indicates an unhandled method, or a response serialized with |codec_|) to
  // the engine.
  void SendResponseData(const std::vector<uint8_t>* data);

 private:
  BinaryReply reply_handler_;
};
}  // namespace internal

// Implemention of MethodResult that sends a response to the Flutter engine
// exactly once, encoded using a given codec.
template <typename T>
class EngineMethodResult : public MethodResult<T> {
 public:
  // Creates a result object that will send results to |reply_handler|, encoded
  // using |codec|. The |codec| pointer must remain valid for as long as this
  // object exists.
  EngineMethodResult(BinaryReply reply_handler, const MethodCodec<T>* codec)
      : reply_manager_(
            std::make_unique<internal::ReplyManager>(std::move(reply_handler))),
        codec_(codec) {}

  ~EngineMethodResult() = default;

 protected:
  // |appcode::MethodResult|
  void SuccessInternal(const T* result) override {
    std::unique_ptr<std::vector<uint8_t>> data =
        codec_->EncodeSuccessEnvelope(result);
    reply_manager_->SendResponseData(data.get());
  }

  // |appcode::MethodResult|
  void ErrorInternal(const std::string& error_code,
                     const std::string& error_message,
                     const T* error_details) override {
    std::unique_ptr<std::vector<uint8_t>> data =
        codec_->EncodeErrorEnvelope(error_code, error_message, error_details);
    reply_manager_->SendResponseData(data.get());
  }

  // |appcode::MethodResult|
  void NotImplementedInternal() override {
    reply_manager_->SendResponseData(nullptr);
  }

 private:
  std::unique_ptr<internal::ReplyManager> reply_manager_;

  const MethodCodec<T>* codec_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_ENGINE_METHOD_RESULT_H_
