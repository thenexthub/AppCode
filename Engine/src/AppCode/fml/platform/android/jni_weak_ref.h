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

#ifndef APPCODE_FML_PLATFORM_ANDROID_JNI_WEAK_REF_H_
#define APPCODE_FML_PLATFORM_ANDROID_JNI_WEAK_REF_H_

#include <jni.h>

#include "appcode/fml/platform/android/scoped_java_ref.h"

namespace fml {
namespace jni {

// Manages WeakGlobalRef lifecycle.
// This class is not thread-safe w.r.t. get() and reset(). Multiple threads may
// safely use get() concurrently, but if the user calls reset() (or of course,
// calls the destructor) they'll need to provide their own synchronization.
class JavaObjectWeakGlobalRef {
 public:
  JavaObjectWeakGlobalRef();

  JavaObjectWeakGlobalRef(const JavaObjectWeakGlobalRef& orig);

  JavaObjectWeakGlobalRef(JNIEnv* env, jobject obj);

  virtual ~JavaObjectWeakGlobalRef();

  void operator=(const JavaObjectWeakGlobalRef& rhs);

  ScopedJavaLocalRef<jobject> get(JNIEnv* env) const;

  bool is_empty() const { return obj_ == NULL; }

  void reset();

 private:
  void Assign(const JavaObjectWeakGlobalRef& rhs);

  jweak obj_;
};

// Get the real object stored in the weak reference returned as a
// ScopedJavaLocalRef.
ScopedJavaLocalRef<jobject> GetRealObject(JNIEnv* env, jweak obj);

}  // namespace jni
}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_ANDROID_JNI_WEAK_REF_H_
