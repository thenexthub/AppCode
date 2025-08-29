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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_TOUCH_SOURCE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_TOUCH_SOURCE_H_

#include <fuchsia/ui/pointer/cpp/fidl.h>

#include <optional>
#include <vector>

#include "appcode/fml/logging.h"

namespace appcode_runner::testing {

// A test stub to act as the protocol server. A test can control what is sent
// back by this server implementation, via the ScheduleCallback call.
class FakeTouchSource : public fuchsia::ui::pointer::TouchSource {
 public:
  // |fuchsia.ui.pointer.TouchSource|
  void Watch(std::vector<fuchsia::ui::pointer::TouchResponse> responses,
             TouchSource::WatchCallback callback) override {
    responses_ = std::move(responses);
    callback_ = std::move(callback);
  }

  // Have the server issue events to the client's hanging-get Watch call.
  void ScheduleCallback(std::vector<fuchsia::ui::pointer::TouchEvent> events) {
    FML_CHECK(callback_) << "require a valid WatchCallback";
    callback_(std::move(events));
  }

  // Allow the test to observe what the client uploaded on the next Watch call.
  std::optional<std::vector<fuchsia::ui::pointer::TouchResponse>>
  UploadedResponses() {
    auto responses = std::move(responses_);
    responses_.reset();
    return responses;
  }

 private:
  // |fuchsia.ui.pointer.TouchSource|
  void UpdateResponse(fuchsia::ui::pointer::TouchInteractionId ixn,
                      fuchsia::ui::pointer::TouchResponse response,
                      TouchSource::UpdateResponseCallback callback) override {
    FML_UNREACHABLE();
  }

  // Client uploads responses to server.
  std::optional<std::vector<fuchsia::ui::pointer::TouchResponse>> responses_;

  // Client-side logic to invoke on Watch() call's return. A test triggers it
  // with ScheduleCallback().
  TouchSource::WatchCallback callback_;
};

}  // namespace appcode_runner::testing

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_TOUCH_SOURCE_H_
