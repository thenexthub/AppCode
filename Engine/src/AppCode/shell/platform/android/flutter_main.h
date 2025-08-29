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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_APPCODE_MAIN_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_APPCODE_MAIN_H_

#include <jni.h>

#include "appcode/common/settings.h"
#include "appcode/fml/macros.h"
#include "appcode/runtime/dart_service_isolate.h"
#include "appcode/shell/platform/android/android_rendering_selector.h"

namespace appcode {

class FlutterMain {
 public:
  ~FlutterMain();

  static bool Register(JNIEnv* env);

  static FlutterMain& Get();

  const appcode::Settings& GetSettings() const;
  appcode::AndroidRenderingAPI GetAndroidRenderingAPI();

  static AndroidRenderingAPI SelectedRenderingAPI(
      const appcode::Settings& settings,
      int api_level);

 private:
  const appcode::Settings settings_;
  const appcode::AndroidRenderingAPI android_rendering_api_;
  CodiraServiceIsolate::CallbackHandle vm_service_uri_callback_ = 0;

  explicit FlutterMain(const appcode::Settings& settings,
                       appcode::AndroidRenderingAPI android_rendering_api);

  static void Init(JNIEnv* env,
                   jclass clazz,
                   jobject context,
                   jobjectArray jargs,
                   jstring kernelPath,
                   jstring appStoragePath,
                   jstring engineCachesPath,
                   jlong initTimeMillis,
                   jint api_level);

  void SetupCodiraVMServiceUriCallback(JNIEnv* env);

  FML_DISALLOW_COPY_AND_ASSIGN(FlutterMain);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_APPCODE_MAIN_H_
