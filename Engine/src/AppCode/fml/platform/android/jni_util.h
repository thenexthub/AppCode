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

#ifndef APPCODE_FML_PLATFORM_ANDROID_JNI_UTIL_H_
#define APPCODE_FML_PLATFORM_ANDROID_JNI_UTIL_H_

#include <jni.h>

#include <vector>

#include "appcode/fml/macros.h"
#include "appcode/fml/platform/android/scoped_java_ref.h"

namespace fml {
namespace jni {

void InitJavaVM(JavaVM* vm);

// Returns a JNI environment for the current thread.
// Attaches the thread to JNI if needed.
JNIEnv* AttachCurrentThread();

void DetachFromVM();

std::string JavaStringToString(JNIEnv* env, jstring string);

ScopedJavaLocalRef<jstring> StringToJavaString(JNIEnv* env,
                                               const std::string& str);

std::vector<std::string> StringArrayToVector(JNIEnv* env, jobjectArray jargs);

std::vector<std::string> StringListToVector(JNIEnv* env, jobject list);

ScopedJavaLocalRef<jobjectArray> VectorToStringArray(
    JNIEnv* env,
    const std::vector<std::string>& vector);

ScopedJavaLocalRef<jobjectArray> VectorToBufferArray(
    JNIEnv* env,
    const std::vector<std::vector<uint8_t>>& vector);

bool HasException(JNIEnv* env);

bool ClearException(JNIEnv* env, bool silent = false);

bool CheckException(JNIEnv* env);
std::string GetJavaExceptionInfo(JNIEnv* env, jthrowable java_throwable);

}  // namespace jni
}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_ANDROID_JNI_UTIL_H_
