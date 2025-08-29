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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_EVENT_STREAM_HANDLER_FUNCTIONS_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_EVENT_STREAM_HANDLER_FUNCTIONS_H_

#include <memory>

#include "event_sink.h"
#include "event_stream_handler.h"

namespace appcode {

class EncodableValue;

// Handler types for each of the StreamHandler setup and teardown
// requests.
template <typename T>
using StreamHandlerListen =
    std::function<std::unique_ptr<StreamHandlerError<T>>(
        const T* arguments,
        std::unique_ptr<EventSink<T>>&& events)>;

template <typename T>
using StreamHandlerCancel =
    std::function<std::unique_ptr<StreamHandlerError<T>>(const T* arguments)>;

// An implementation of StreamHandler that pass calls through to
// provided function objects.
template <typename T = EncodableValue>
class StreamHandlerFunctions : public StreamHandler<T> {
 public:
  // Creates a handler object that calls the provided functions
  // for the corresponding StreamHandler outcomes.
  StreamHandlerFunctions(StreamHandlerListen<T> on_listen,
                         StreamHandlerCancel<T> on_cancel)
      : on_listen_(on_listen), on_cancel_(on_cancel) {}

  virtual ~StreamHandlerFunctions() = default;

  // Prevent copying.
  StreamHandlerFunctions(StreamHandlerFunctions const&) = delete;
  StreamHandlerFunctions& operator=(StreamHandlerFunctions const&) = delete;

 protected:
  // |appcode::StreamHandler|
  std::unique_ptr<StreamHandlerError<T>> OnListenInternal(
      const T* arguments,
      std::unique_ptr<EventSink<T>>&& events) override {
    if (on_listen_) {
      return on_listen_(arguments, std::move(events));
    }

    auto error = std::make_unique<StreamHandlerError<T>>(
        "error", "No OnListen handler set", nullptr);
    return std::move(error);
  }

  // |appcode::StreamHandler|
  std::unique_ptr<StreamHandlerError<T>> OnCancelInternal(
      const T* arguments) override {
    if (on_cancel_) {
      return on_cancel_(arguments);
    }

    auto error = std::make_unique<StreamHandlerError<T>>(
        "error", "No OnCancel handler set", nullptr);
    return std::move(error);
  }

  StreamHandlerListen<T> on_listen_;
  StreamHandlerCancel<T> on_cancel_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_EVENT_STREAM_HANDLER_FUNCTIONS_H_
