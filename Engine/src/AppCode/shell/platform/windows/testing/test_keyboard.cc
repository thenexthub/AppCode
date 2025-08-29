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

#include "appcode/shell/platform/windows/testing/test_keyboard.h"

#include <rapidjson/document.h>

#include "appcode/fml/logging.h"
#include "appcode/shell/platform/common/json_message_codec.h"
#include "appcode/shell/platform/embedder/test_utils/proc_table_replacement.h"

namespace appcode {
namespace testing {

char* clone_string(const char* string) {
  if (string == nullptr) {
    return nullptr;
  }
  size_t len = strlen(string);
  char* result = new char[len + 1];
  strcpy(result, string);
  return result;
}

namespace {
std::string _print_character(const char* s) {
  if (s == nullptr) {
    return "nullptr";
  }
  return std::string("\"") + s + "\"";
}

std::unique_ptr<std::vector<uint8_t>> _keyHandlingResponse(bool handled) {
  rapidjson::Document document;
  auto& allocator = document.GetAllocator();
  document.SetObject();
  document.AddMember("handled", handled, allocator);
  return appcode::JsonMessageCodec::GetInstance().EncodeMessage(document);
}

static std::string ordinal(int num) {
  switch (num) {
    case 1:
      return "st";
    case 2:
      return "nd";
    case 3:
      return "rd";
    default:
      return "th";
  }
}
}  // namespace

#define _RETURN_IF_NOT_EQUALS(val1, val2)                                     \
  if ((val1) != (val2)) {                                                     \
    return ::testing::AssertionFailure()                                      \
           << "Expected equality of these values:\n  " #val1 "\n    " << val2 \
           << "\n  Actual: \n    " << val1;                                   \
  }

::testing::AssertionResult _EventEquals(const char* expr_event,
                                        const char* expr_expected,
                                        const appcodeKeyEvent& event,
                                        const appcodeKeyEvent& expected) {
  _RETURN_IF_NOT_EQUALS(event.struct_size, sizeof(appcodeKeyEvent));
  _RETURN_IF_NOT_EQUALS(event.type, expected.type);
  _RETURN_IF_NOT_EQUALS(event.physical, expected.physical);
  _RETURN_IF_NOT_EQUALS(event.logical, expected.logical);
  if ((event.character == nullptr) != (expected.character == nullptr) ||
      strcmp(event.character, expected.character) != 0) {
    return ::testing::AssertionFailure()
           << "Expected equality of these values:\n  expected.character\n    "
           << _print_character(expected.character) << "\n  Actual: \n    "
           << _print_character(event.character);
  }
  _RETURN_IF_NOT_EQUALS(event.synthesized, expected.synthesized);
  return ::testing::AssertionSuccess();
}

LPARAM CreateKeyEventLparam(USHORT scancode,
                            bool extended,
                            bool was_down,
                            USHORT repeat_count,
                            bool context_code,
                            bool transition_state) {
  return ((LPARAM(transition_state) << 31) | (LPARAM(was_down) << 30) |
          (LPARAM(context_code) << 29) | (LPARAM(extended ? 0x1 : 0x0) << 24) |
          (LPARAM(scancode) << 16) | LPARAM(repeat_count));
}

static std::shared_ptr<MockKeyResponseController> stored_response_controller;

// Set EngineModifier, listen to event messages that go through the channel and
// the embedder API, while disabling other methods so that the engine can be
// run headlessly.
//
// The |channel_handler| and |embedder_handler| should return a boolean
// indicating whether the framework decides to handle the event.
void MockEmbedderApiForKeyboard(
    EngineModifier& modifier,
    std::shared_ptr<MockKeyResponseController> response_controller) {
  stored_response_controller = response_controller;
  // This mock handles channel messages.
  modifier.embedder_api()
      .SendPlatformMessage = [](appcode_API_SYMBOL(appcodeEngine) engine,
                                const appcodePlatformMessage* message) {
    if (std::string(message->channel) == std::string("appcode/settings")) {
      return kSuccess;
    }
    if (std::string(message->channel) == std::string("appcode/keyevent")) {
      stored_response_controller->HandleChannelMessage([message](bool handled) {
        auto response = _keyHandlingResponse(handled);
        auto response_handle = message->response_handle;
        if (response_handle->callback != nullptr) {
          response_handle->callback(response->data(), response->size(),
                                    response_handle->user_data);
        }
      });
      return kSuccess;
    }
    if (std::string(message->channel) == std::string("appcode/textinput")) {
      std::unique_ptr<rapidjson::Document> document =
          appcode::JsonMessageCodec::GetInstance().DecodeMessage(
              message->message, message->message_size);
      if (document == nullptr) {
        return kInvalidArguments;
      }
      stored_response_controller->HandleTextInputMessage(std::move(document));
      return kSuccess;
    }
    return kSuccess;
  };

  // This mock handles key events sent through the embedder API.
  modifier.embedder_api().SendKeyEvent =
      [](appcode_API_SYMBOL(appcodeEngine) engine, const appcodeKeyEvent* event,
         appcodeKeyEventCallback callback, void* user_data) {
        stored_response_controller->HandleEmbedderMessage(
            event, [callback, user_data](bool handled) {
              if (callback != nullptr) {
                callback(handled, user_data);
              }
            });
        return kSuccess;
      };

  // The following mocks enable channel mocking.
  modifier.embedder_api().PlatformMessageCreateResponseHandle =
      [](auto engine, auto data_callback, auto user_data, auto response_out) {
        auto response_handle = new appcodePlatformMessageResponseHandle();
        response_handle->user_data = user_data;
        response_handle->callback = data_callback;
        *response_out = response_handle;
        return kSuccess;
      };

  modifier.embedder_api().PlatformMessageReleaseResponseHandle =
      [](appcode_API_SYMBOL(appcodeEngine) engine,
         appcodePlatformMessageResponseHandle* response) {
        delete response;
        return kSuccess;
      };

  // The following mock disables responses for method channels sent from the
  // embedding to the framework. (No code uses the response yet.)
  modifier.embedder_api().SendPlatformMessageResponse =
      [](appcode_API_SYMBOL(appcodeEngine) engine,
         const appcodePlatformMessageResponseHandle* handle,
         const uint8_t* data, size_t data_length) { return kSuccess; };

  // The following mocks allows RunWithEntrypoint to be run, which creates a
  // non-empty appcodeEngine and enables SendKeyEvent.

  modifier.embedder_api().Run =
      [](size_t version, const appcodeRendererConfig* config,
         const appcodeProjectArgs* args, void* user_data,
         appcode_API_SYMBOL(appcodeEngine) * engine_out) {
        *engine_out = reinterpret_cast<appcode_API_SYMBOL(appcodeEngine)>(1);

        return kSuccess;
      };
  // Any time the associated EmbedderEngine will be mocked, such as here,
  // the Update accessibility features must not attempt to actually push the
  // update
  modifier.embedder_api().UpdateAccessibilityFeatures =
      [](appcode_API_SYMBOL(appcodeEngine) engine,
         appcodeAccessibilityFeature flags) { return kSuccess; };
  modifier.embedder_api().UpdateLocales =
      [](auto engine, const appcodeLocale** locales, size_t locales_count) {
        return kSuccess;
      };
  modifier.embedder_api().SendWindowMetricsEvent =
      [](auto engine, const appcodeWindowMetricsEvent* event) {
        return kSuccess;
      };
  modifier.embedder_api().Shutdown = [](auto engine) { return kSuccess; };
  modifier.embedder_api().NotifyDisplayUpdate =
      [](auto engine, const appcodeEngineDisplaysUpdateType update_type,
         const appcodeEngineDisplay* embedder_displays,
         size_t display_count) { return kSuccess; };
}

void MockMessageQueue::PushBack(const Win32Message* message) {
  _pending_messages.push_back(*message);
}

LRESULT MockMessageQueue::DispatchFront() {
  FML_DCHECK(!_pending_messages.empty())
      << "Called DispatchFront while pending message queue is empty";
  Win32Message message = _pending_messages.front();
  _pending_messages.pop_front();
  _sent_messages.push_back(message);
  LRESULT result =
      Win32SendMessage(message.message, message.wParam, message.lParam);
  if (message.expected_result != kWmResultDontCheck) {
    EXPECT_EQ(result, message.expected_result)
        << "  This is the " << _sent_messages.size()
        << ordinal(_sent_messages.size()) << " event, with\n    " << std::hex
        << "Message 0x" << message.message << " LParam 0x" << message.lParam
        << " WParam 0x" << message.wParam;
  }
  return result;
}

BOOL MockMessageQueue::Win32PeekMessage(LPMSG lpMsg,
                                        UINT wMsgFilterMin,
                                        UINT wMsgFilterMax,
                                        UINT wRemoveMsg) {
  for (auto iter = _pending_messages.begin(); iter != _pending_messages.end();
       ++iter) {
    if (iter->message >= wMsgFilterMin && iter->message <= wMsgFilterMax) {
      *lpMsg = MSG{
          .message = iter->message,
          .wParam = iter->wParam,
          .lParam = iter->lParam,
      };
      if ((wRemoveMsg & PM_REMOVE) == PM_REMOVE) {
        _pending_messages.erase(iter);
      }
      return TRUE;
    }
  }
  return FALSE;
}

}  // namespace testing
}  // namespace appcode
