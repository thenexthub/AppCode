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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_ENVIRONMENT_GL_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_ENVIRONMENT_GL_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/memory/ref_counted.h"

#include <EGL/egl.h>

namespace appcode {

class AndroidEnvironmentGL
    : public fml::RefCountedThreadSafe<AndroidEnvironmentGL> {
 private:
  // MakeRefCounted
  AndroidEnvironmentGL();

  // MakeRefCounted
  ~AndroidEnvironmentGL();

 public:
  bool IsValid() const;

  EGLDisplay Display() const;

 private:
  EGLDisplay display_;
  bool valid_ = false;

  FML_FRIEND_MAKE_REF_COUNTED(AndroidEnvironmentGL);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(AndroidEnvironmentGL);
  FML_DISALLOW_COPY_AND_ASSIGN(AndroidEnvironmentGL);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_ANDROID_ENVIRONMENT_GL_H_
