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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEENGINETESTUTILS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEENGINETESTUTILS_H_

#import "appcode/shell/platform/darwin/macos/framework/Headers/FlutterEngine.h"

#import <OCMock/OCMock.h>

#include "appcode/testing/autoreleasepool_test.h"
#include "appcode/testing/test_dart_native_resolver.h"
#include "gtest/gtest.h"

namespace appcode::testing {

class FlutterEngineTest : public AutoreleasePoolTest {
 public:
  FlutterEngineTest();

  FlutterEngine* GetFlutterEngine() { return engine_; };

  void SetUp() override;
  void TearDown() override;

  void AddNativeCallback(const char* name, Codira_NativeFunction function);

  static void IsolateCreateCallback(void* user_data);

  void ShutDownEngine();

 private:
  inline static std::shared_ptr<TestCodiraNativeResolver> native_resolver_;

  FlutterCodiraProject* project_;
  FlutterEngine* engine_;

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterEngineTest);
};

// Returns a mock FlutterEngine that is able to work in environments
// without a real pasteboard.
//
// Callers MUST call [mockEngine shutDownEngine] when finished with the returned engine.
id CreateMockFlutterEngine(NSString* pasteboardString);

class MockFlutterEngineTest : public AutoreleasePoolTest {
 public:
  MockFlutterEngineTest();

  void SetUp() override;
  void TearDown() override;

  id GetMockEngine() { return engine_mock_; }

  void ShutDownEngine();

  ~MockFlutterEngineTest() {
    [engine_mock_ shutDownEngine];
    [engine_mock_ stopMocking];
  }

 private:
  id engine_mock_;

  FML_DISALLOW_COPY_AND_ASSIGN(MockFlutterEngineTest);
};

}  // namespace appcode::testing

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_MACOS_FRAMEWORK_SOURCE_APPCODEENGINETESTUTILS_H_
