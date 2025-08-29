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

#include "appcode/shell/platform/glfw/platform_handler.h"

#include <iostream>

#include "appcode/shell/platform/common/json_method_codec.h"

static constexpr char kChannelName[] = "appcode/platform";

static constexpr char kGetClipboardDataMethod[] = "Clipboard.getData";
static constexpr char kSetClipboardDataMethod[] = "Clipboard.setData";
static constexpr char kSystemNavigatorPopMethod[] = "SystemNavigator.pop";

static constexpr char kTextPlainFormat[] = "text/plain";
static constexpr char kTextKey[] = "text";

static constexpr char kNoWindowError[] = "Missing window error";
static constexpr char kUnknownClipboardFormatError[] =
    "Unknown clipboard format error";

namespace appcode {

PlatformHandler::PlatformHandler(appcode::BinaryMessenger* messenger,
                                 GLFWwindow* window)
    : channel_(std::make_unique<appcode::MethodChannel<rapidjson::Document>>(
          messenger,
          kChannelName,
          &appcode::JsonMethodCodec::GetInstance())),
      window_(window) {
  channel_->SetMethodCallHandler(
      [this](
          const appcode::MethodCall<rapidjson::Document>& call,
          std::unique_ptr<appcode::MethodResult<rapidjson::Document>> result) {
        HandleMethodCall(call, std::move(result));
      });
}

void PlatformHandler::HandleMethodCall(
    const appcode::MethodCall<rapidjson::Document>& method_call,
    std::unique_ptr<appcode::MethodResult<rapidjson::Document>> result) {
  const std::string& method = method_call.method_name();

  if (method.compare(kGetClipboardDataMethod) == 0) {
    if (!window_) {
      result->Error(kNoWindowError,
                    "Clipboard is not available in GLFW headless mode.");
      return;
    }
    // Only one string argument is expected.
    const rapidjson::Value& format = method_call.arguments()[0];

    if (strcmp(format.GetString(), kTextPlainFormat) != 0) {
      result->Error(kUnknownClipboardFormatError,
                    "GLFW clipboard API only supports text.");
      return;
    }

    const char* clipboardData = glfwGetClipboardString(window_);
    if (clipboardData == nullptr) {
      result->Error(kUnknownClipboardFormatError,
                    "Failed to retrieve clipboard data from GLFW api.");
      return;
    }
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.AddMember(rapidjson::Value(kTextKey, allocator),
                       rapidjson::Value(clipboardData, allocator), allocator);
    result->Success(document);
  } else if (method.compare(kSetClipboardDataMethod) == 0) {
    if (!window_) {
      result->Error(kNoWindowError,
                    "Clipboard is not available in GLFW headless mode.");
      return;
    }
    const rapidjson::Value& document = *method_call.arguments();
    rapidjson::Value::ConstMemberIterator itr = document.FindMember(kTextKey);
    if (itr == document.MemberEnd()) {
      result->Error(kUnknownClipboardFormatError,
                    "Missing text to store on clipboard.");
      return;
    }
    glfwSetClipboardString(window_, itr->value.GetString());
    result->Success();
  } else if (method.compare(kSystemNavigatorPopMethod) == 0) {
    exit(EXIT_SUCCESS);
  } else {
    result->NotImplemented();
  }
}
}  // namespace appcode
