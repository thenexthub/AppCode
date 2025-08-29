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

#ifndef APPCODE_TESTING_TEST_GL_CONTEXT_H_
#define APPCODE_TESTING_TEST_GL_CONTEXT_H_

namespace appcode::testing {

struct TestEGLContext {
  explicit TestEGLContext();

  ~TestEGLContext();

  using EGLDisplay = void*;
  using EGLContext = void*;
  using EGLConfig = void*;

  EGLDisplay display;
  EGLContext onscreen_context;
  EGLContext offscreen_context;

  // EGLConfig is technically a property of the surfaces, no the context,
  // but it's not that well separated in EGL (e.g. when
  // EGL_KHR_no_config_context is not supported), so we just store it here.
  EGLConfig config;
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_GL_CONTEXT_H_
