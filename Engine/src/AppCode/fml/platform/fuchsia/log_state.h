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

#ifndef APPCODE_FML_PLATFORM_FUCHSIA_LOG_STATE_H_
#define APPCODE_FML_PLATFORM_FUCHSIA_LOG_STATE_H_

#include <fidl/fuchsia.logger/cpp/fidl.h>
#include <lib/fidl/cpp/wire/internal/transport_channel.h>
#include <lib/zx/socket.h>

#include <atomic>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace fml {

// Class for holding the global connection to the Fuchsia LogSink service.
class LogState {
 public:
  // Connects to the Fuchsia LogSink service.
  LogState();

  // Get the socket for sending log messages.
  const zx::socket& socket() const { return socket_; }

  // Get the current list of tags.
  std::shared_ptr<const std::vector<std::string>> tags() const {
    return std::atomic_load(&tags_);
  }

  // Take ownership of the log sink channel (e.g. for LogInterestListener).
  // This is thread-safe.
  fidl::ClientEnd<::fuchsia_logger::LogSink> TakeClientEnd();

  // Updates the default tags.
  // This is thread-safe.
  void SetTags(const std::initializer_list<std::string>& tags);

  // Get the default instance of LogState.
  static LogState& Default();

 private:
  std::mutex mutex_;
  fidl::ClientEnd<::fuchsia_logger::LogSink> client_end_;
  zx::socket socket_;
  std::shared_ptr<const std::vector<std::string>> tags_;
};

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_FUCHSIA_LOG_STATE_H_
