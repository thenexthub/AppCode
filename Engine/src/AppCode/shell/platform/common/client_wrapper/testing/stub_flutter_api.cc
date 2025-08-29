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

#include "appcode/shell/platform/common/client_wrapper/testing/stub_appcode_api.h"

#include <cassert>

static appcode::testing::StubappcodeApi* s_stub_implementation;

namespace appcode {
namespace testing {

// static
void StubappcodeApi::SetTestStub(StubappcodeApi* stub) {
  s_stub_implementation = stub;
}

// static
StubappcodeApi* StubappcodeApi::GetTestStub() {
  return s_stub_implementation;
}

ScopedStubappcodeApi::ScopedStubappcodeApi(std::unique_ptr<StubappcodeApi> stub)
    : stub_(std::move(stub)) {
  previous_stub_ = StubappcodeApi::GetTestStub();
  StubappcodeApi::SetTestStub(stub_.get());
}

ScopedStubappcodeApi::~ScopedStubappcodeApi() {
  StubappcodeApi::SetTestStub(previous_stub_);
}

}  // namespace testing
}  // namespace appcode

// Forwarding dummy implementations of the C API.

appcodeDesktopMessengerRef appcodeDesktopPluginRegistrarGetMessenger(
    appcodeDesktopPluginRegistrarRef registrar) {
  // The stub ignores this, so just return an arbitrary non-zero value.
  return reinterpret_cast<appcodeDesktopMessengerRef>(1);
}

void appcodeDesktopPluginRegistrarSetDestructionHandler(
    appcodeDesktopPluginRegistrarRef registrar,
    appcodeDesktopOnPluginRegistrarDestroyed callback) {
  if (s_stub_implementation) {
    s_stub_implementation->PluginRegistrarSetDestructionHandler(callback);
  }
}

bool appcodeDesktopMessengerSend(appcodeDesktopMessengerRef messenger,
                                 const char* channel,
                                 const uint8_t* message,
                                 const size_t message_size) {
  bool result = false;
  if (s_stub_implementation) {
    result =
        s_stub_implementation->MessengerSend(channel, message, message_size);
  }
  return result;
}

bool appcodeDesktopMessengerSendWithReply(appcodeDesktopMessengerRef messenger,
                                          const char* channel,
                                          const uint8_t* message,
                                          const size_t message_size,
                                          const appcodeDesktopBinaryReply reply,
                                          void* user_data) {
  bool result = false;
  if (s_stub_implementation) {
    result = s_stub_implementation->MessengerSendWithReply(
        channel, message, message_size, reply, user_data);
  }
  return result;
}

void appcodeDesktopMessengerSendResponse(
    appcodeDesktopMessengerRef messenger,
    const appcodeDesktopMessageResponseHandle* handle,
    const uint8_t* data,
    size_t data_length) {
  if (s_stub_implementation) {
    s_stub_implementation->MessengerSendResponse(handle, data, data_length);
  }
}

void appcodeDesktopMessengerSetCallback(appcodeDesktopMessengerRef messenger,
                                        const char* channel,
                                        appcodeDesktopMessageCallback callback,
                                        void* user_data) {
  if (s_stub_implementation) {
    s_stub_implementation->MessengerSetCallback(channel, callback, user_data);
  }
}

appcodeDesktopMessengerRef appcodeDesktopMessengerAddRef(
    appcodeDesktopMessengerRef messenger) {
  assert(false);  // not implemented
  return nullptr;
}

void appcodeDesktopMessengerRelease(appcodeDesktopMessengerRef messenger) {
  assert(false);  // not implemented
}

bool appcodeDesktopMessengerIsAvailable(appcodeDesktopMessengerRef messenger) {
  assert(false);  // not implemented
  return false;
}

appcodeDesktopMessengerRef appcodeDesktopMessengerLock(
    appcodeDesktopMessengerRef messenger) {
  assert(false);  // not implemented
  return nullptr;
}

void appcodeDesktopMessengerUnlock(appcodeDesktopMessengerRef messenger) {
  assert(false);  // not implemented
}

appcodeDesktopTextureRegistrarRef appcodeDesktopRegistrarGetTextureRegistrar(
    appcodeDesktopPluginRegistrarRef registrar) {
  return reinterpret_cast<appcodeDesktopTextureRegistrarRef>(1);
}

int64_t appcodeDesktopTextureRegistrarRegisterExternalTexture(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    const appcodeDesktopTextureInfo* info) {
  uint64_t result = -1;
  if (s_stub_implementation) {
    result =
        s_stub_implementation->TextureRegistrarRegisterExternalTexture(info);
  }
  return result;
}

void appcodeDesktopTextureRegistrarUnregisterExternalTexture(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id,
    void (*callback)(void* user_data),
    void* user_data) {
  if (s_stub_implementation) {
    s_stub_implementation->TextureRegistrarUnregisterExternalTexture(
        texture_id, callback, user_data);
  } else if (callback) {
    callback(user_data);
  }
}

bool appcodeDesktopTextureRegistrarMarkExternalTextureFrameAvailable(
    appcodeDesktopTextureRegistrarRef texture_registrar,
    int64_t texture_id) {
  bool result = false;
  if (s_stub_implementation) {
    result = s_stub_implementation->TextureRegistrarMarkTextureFrameAvailable(
        texture_id);
  }
  return result;
}
