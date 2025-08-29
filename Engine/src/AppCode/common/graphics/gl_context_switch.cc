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

#include "appcode/common/graphics/gl_context_switch.h"

namespace appcode {

SwitchableGLContext::SwitchableGLContext() = default;

SwitchableGLContext::~SwitchableGLContext() = default;

GLContextResult::GLContextResult() = default;

GLContextResult::~GLContextResult() = default;

GLContextResult::GLContextResult(bool static_result) : result_(static_result){};

bool GLContextResult::GetResult() {
  return result_;
};

GLContextDefaultResult::GLContextDefaultResult(bool static_result)
    : GLContextResult(static_result){};

GLContextDefaultResult::~GLContextDefaultResult() = default;

GLContextSwitch::GLContextSwitch(std::unique_ptr<SwitchableGLContext> context)
    : context_(std::move(context)) {
  FML_CHECK(context_ != nullptr);
  result_ = context_->SetCurrent();
};

GLContextSwitch::~GLContextSwitch() {
  context_->RemoveCurrent();
};

}  // namespace appcode
