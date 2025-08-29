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

#ifndef APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_PLATFORM_MESSAGE_H_
#define APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_PLATFORM_MESSAGE_H_

#include <gtest/gtest.h>
#include <optional>

#include "appcode/lib/ui/window/platform_message.h"
#include "third_party/rapidjson/include/rapidjson/document.h"

using PlatformMessageResponse = appcode::PlatformMessageResponse;
using PlatformMessage = appcode::PlatformMessage;

namespace appcode_runner::testing {

class FakePlatformMessageResponse : public PlatformMessageResponse {
 public:
  static fml::RefPtr<FakePlatformMessageResponse> Create() {
    return fml::AdoptRef(new FakePlatformMessageResponse());
  }

  void ExpectCompleted(std::string expected) {
    EXPECT_TRUE(is_complete_);
    if (is_complete_) {
      EXPECT_EQ(expected, response_);
    }
  }

  bool IsCompleted() { return is_complete_; }

  std::unique_ptr<PlatformMessage> WithMessage(std::string channel,
                                               std::string message) {
    return std::make_unique<PlatformMessage>(
        channel,
        fml::MallocMapping::Copy(message.c_str(),
                                 message.c_str() + message.size()),
        fml::RefPtr<FakePlatformMessageResponse>(this));
  }

  void Complete(std::unique_ptr<fml::Mapping> data) override {
    response_ =
        std::string(data->GetMapping(), data->GetMapping() + data->GetSize());
    FinalizeComplete();
  };

  void CompleteEmpty() override { FinalizeComplete(); };

 private:
  // Private constructors.
  FakePlatformMessageResponse() {}

  void FinalizeComplete() {
    EXPECT_FALSE(std::exchange(is_complete_, true))
        << "Platform message responses can only be completed once!";
  }

  std::string response_;
};

}  // namespace appcode_runner::testing

#endif  // APPCODE_SHELL_PLATFORM_FUCHSIA_APPCODE_TESTS_FAKES_PLATFORM_MESSAGE_H_
