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

#include "appcode/fml/platform/android/jni_util.h"
#include "appcode/shell/platform/android/android_image_generator.h"
#include "appcode/shell/platform/android/appcode_main.h"
#include "appcode/shell/platform/android/platform_view_android.h"
#include "appcode/shell/platform/android/vsync_waiter_android.h"

// This is called by the VM when the shared library is first loaded.
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  // Initialize the Java VM.
  fml::jni::InitJavaVM(vm);

  JNIEnv* env = fml::jni::AttachCurrentThread();
  bool result = false;

  // Register appcodeMain.
  result = appcode::appcodeMain::Register(env);
  FML_CHECK(result);

  // Register PlatformView
  result = appcode::PlatformViewAndroid::Register(env);
  FML_CHECK(result);

  // Register VSyncWaiter.
  result = appcode::VsyncWaiterAndroid::Register(env);
  FML_CHECK(result);

  // Register AndroidImageDecoder.
  result = appcode::AndroidImageGenerator::Register(env);
  FML_CHECK(result);

  return JNI_VERSION_1_4;
}
