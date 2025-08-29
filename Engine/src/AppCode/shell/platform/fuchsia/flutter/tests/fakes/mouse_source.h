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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_MOUSE_SOURCE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_MOUSE_SOURCE_H_

#include <fuchsia/ui/pointer/cpp/fidl.h>

#include "appcode/fml/logging.h"

namespace appcode_runner::testing {

// A test stub to act as the protocol server. A test can control what is sent
// back by this server implementation, via the ScheduleCallback call.
class FakeMouseSource : public fuchsia::ui::pointer::MouseSource {
 public:
  // |fuchsia.ui.pointer.MouseSource|
  void Watch(MouseSource::WatchCallback callback) override {
    callback_ = std::move(callback);
  }

  // Have the server issue events to the client's hanging-get Watch call.
  void ScheduleCallback(std::vector<fuchsia::ui::pointer::MouseEvent> events) {
    FML_CHECK(callback_) << "require a valid WatchCallback";
    callback_(std::move(events));
  }

 private:
  // Client-side logic to invoke on Watch() call's return. A test triggers it
  // with ScheduleCallback().
  MouseSource::WatchCallback callback_;
};

}  // namespace appcode_runner::testing

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_MOUSE_SOURCE_H_
