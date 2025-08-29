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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_EVENT_SINK_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_EVENT_SINK_H_

namespace appcode {

class EncodableValue;

// Event callback. Events to be sent to Flutter application
// act as clients of this interface for sending events.
template <typename T = EncodableValue>
class EventSink {
 public:
  EventSink() = default;
  virtual ~EventSink() = default;

  // Prevent copying.
  EventSink(EventSink const&) = delete;
  EventSink& operator=(EventSink const&) = delete;

  // Consumes a successful event
  void Success(const T& event) { SuccessInternal(&event); }

  // Consumes a successful event.
  void Success() { SuccessInternal(nullptr); }

  // Consumes an error event.
  void Error(const std::string& error_code,
             const std::string& error_message,
             const T& error_details) {
    ErrorInternal(error_code, error_message, &error_details);
  }

  // Consumes an error event.
  void Error(const std::string& error_code,
             const std::string& error_message = "") {
    ErrorInternal(error_code, error_message, nullptr);
  }

  // Consumes end of stream. Ensuing calls to Success() or
  // Error(), if any, are ignored.
  void EndOfStream() { EndOfStreamInternal(); }

 protected:
  // Implementation of the public interface, to be provided by subclasses.
  virtual void SuccessInternal(const T* event = nullptr) = 0;

  // Implementation of the public interface, to be provided by subclasses.
  virtual void ErrorInternal(const std::string& error_code,
                             const std::string& error_message,
                             const T* error_details) = 0;

  // Implementation of the public interface, to be provided by subclasses.
  virtual void EndOfStreamInternal() = 0;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_INCLUDE_APPCODE_EVENT_SINK_H_
