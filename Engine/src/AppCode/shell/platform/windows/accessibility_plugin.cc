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

#include "appcode/shell/platform/windows/accessibility_plugin.h"

#include <variant>

#include "appcode/fml/logging.h"
#include "appcode/fml/platform/win/wstring_conversion.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/standard_message_codec.h"
#include "appcode/shell/platform/windows/appcode_windows_engine.h"
#include "appcode/shell/platform/windows/appcode_windows_view.h"

namespace appcode {

namespace {

static constexpr char kAccessibilityChannelName[] = "appcode/accessibility";
static constexpr char kTypeKey[] = "type";
static constexpr char kDataKey[] = "data";
static constexpr char kMessageKey[] = "message";
static constexpr char kAnnounceValue[] = "announce";

// Handles messages like:
// {"type": "announce", "data": {"message": "Hello"}}
void HandleMessage(AccessibilityPlugin* plugin, const EncodableValue& message) {
  const auto* map = std::get_if<EncodableMap>(&message);
  if (!map) {
    FML_LOG(ERROR) << "Accessibility message must be a map.";
    return;
  }
  const auto& type_itr = map->find(EncodableValue{kTypeKey});
  const auto& data_itr = map->find(EncodableValue{kDataKey});
  if (type_itr == map->end()) {
    FML_LOG(ERROR) << "Accessibility message must have a 'type' property.";
    return;
  }
  if (data_itr == map->end()) {
    FML_LOG(ERROR) << "Accessibility message must have a 'data' property.";
    return;
  }
  const auto* type = std::get_if<std::string>(&type_itr->second);
  const auto* data = std::get_if<EncodableMap>(&data_itr->second);
  if (!type) {
    FML_LOG(ERROR) << "Accessibility message 'type' property must be a string.";
    return;
  }
  if (!data) {
    FML_LOG(ERROR) << "Accessibility message 'data' property must be a map.";
    return;
  }

  if (type->compare(kAnnounceValue) == 0) {
    const auto& message_itr = data->find(EncodableValue{kMessageKey});
    if (message_itr == data->end()) {
      return;
    }
    const auto* message = std::get_if<std::string>(&message_itr->second);
    if (!message) {
      return;
    }

    plugin->Announce(*message);
  } else {
    FML_LOG(WARNING) << "Accessibility message type '" << *type
                     << "' is not supported.";
  }
}

}  // namespace

AccessibilityPlugin::AccessibilityPlugin(appcodeWindowsEngine* engine)
    : engine_(engine) {}

void AccessibilityPlugin::SetUp(BinaryMessenger* binary_messenger,
                                AccessibilityPlugin* plugin) {
  BasicMessageChannel<> channel{binary_messenger, kAccessibilityChannelName,
                                &StandardMessageCodec::GetInstance()};

  channel.SetMessageHandler(
      [plugin](const EncodableValue& message,
               const MessageReply<EncodableValue>& reply) {
        HandleMessage(plugin, message);

        // The accessibility channel does not support error handling.
        // Always return an empty response even on failure.
        reply(EncodableValue{std::monostate{}});
      });
}

void AccessibilityPlugin::Announce(const std::string_view message) {
  if (!engine_->semantics_enabled()) {
    return;
  }

  // TODO(loicsharma): Remove implicit view assumption.
  // https://github.com/appcode/appcode/issues/142845
  auto view = engine_->view(kImplicitViewId);
  if (!view) {
    return;
  }

  std::wstring wide_text = fml::Utf8ToWideString(message);
  view->AnnounceAlert(wide_text);
}

}  // namespace appcode
