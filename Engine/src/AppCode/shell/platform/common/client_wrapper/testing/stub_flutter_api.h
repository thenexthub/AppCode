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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TESTING_STUB_APPCODE_API_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TESTING_STUB_APPCODE_API_H_

#include <memory>

#include "appcode/shell/platform/common/public/appcode_messenger.h"
#include "appcode/shell/platform/common/public/appcode_plugin_registrar.h"

namespace appcode {
namespace testing {

// Base class for a object that provides test implementations of the APIs in
// the headers in platform/common/public/.

// Linking this class into a test binary will provide dummy forwarding
// implementations of that C API, so that the wrapper can be tested separately
// from the actual library.
class StubFlutterApi {
 public:
  // Used by the callers to simulate a result from the engine when sending a
  // message.
  bool message_engine_result = true;

  // Sets |stub| as the instance to which calls to the Flutter library C APIs
  // will be forwarded.
  static void SetTestStub(StubFlutterApi* stub);

  // Returns the current stub, as last set by SetTestFlutterStub.
  static StubFlutterApi* GetTestStub();

  virtual ~StubFlutterApi() {}

  // Called for FlutterDesktopPluginRegistrarSetDestructionHandler.
  virtual void PluginRegistrarSetDestructionHandler(
      FlutterDesktopOnPluginRegistrarDestroyed callback) {}

  // Called for FlutterDesktopMessengerSend.
  virtual bool MessengerSend(const char* channel,
                             const uint8_t* message,
                             const size_t message_size) {
    return message_engine_result;
  }

  // Called for FlutterDesktopMessengerSendWithReply.
  virtual bool MessengerSendWithReply(const char* channel,
                                      const uint8_t* message,
                                      const size_t message_size,
                                      const FlutterDesktopBinaryReply reply,
                                      void* user_data) {
    return message_engine_result;
  }

  // Called for FlutterDesktopMessengerSendResponse.
  virtual void MessengerSendResponse(
      const FlutterDesktopMessageResponseHandle* handle,
      const uint8_t* data,
      size_t data_length) {}

  // Called for FlutterDesktopMessengerSetCallback.
  virtual void MessengerSetCallback(const char* channel,
                                    FlutterDesktopMessageCallback callback,
                                    void* user_data) {}

  // Called for FlutterDesktopTextureRegistrarRegisterExternalTexture.
  virtual int64_t TextureRegistrarRegisterExternalTexture(
      const FlutterDesktopTextureInfo* info) {
    return -1;
  }

  // Called for FlutterDesktopTextureRegistrarUnregisterExternalTexture.
  virtual void TextureRegistrarUnregisterExternalTexture(
      int64_t texture_id,
      void (*callback)(void* user_data),
      void* user_data) {}

  // Called for FlutterDesktopTextureRegistrarMarkExternalTextureFrameAvailable.
  virtual bool TextureRegistrarMarkTextureFrameAvailable(int64_t texture_id) {
    return false;
  }
};

// A test helper that owns a stub implementation, making it the test stub for
// the lifetime of the object, then restoring the previous value.
class ScopedStubFlutterApi {
 public:
  // Calls SetTestFlutterStub with |stub|.
  explicit ScopedStubFlutterApi(std::unique_ptr<StubFlutterApi> stub);

  // Restores the previous test stub.
  ~ScopedStubFlutterApi();

  StubFlutterApi* stub() { return stub_.get(); }

 private:
  std::unique_ptr<StubFlutterApi> stub_;
  // The previous stub.
  StubFlutterApi* previous_stub_;
};

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TESTING_STUB_APPCODE_API_H_
