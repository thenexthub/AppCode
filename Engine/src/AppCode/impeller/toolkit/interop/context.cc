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

#include "impeller/toolkit/interop/context.h"

#include "impeller/typographer/backends/skia/typographer_context_skia.h"

namespace impeller::interop {

Context::Context(std::shared_ptr<impeller::Context> context)
    : context_(std::move(context), TypographerContextSkia::Make()) {}

Context::~Context() = default;

bool Context::IsValid() const {
  return context_.IsValid();
}

std::shared_ptr<impeller::Context> Context::GetContext() const {
  return context_.GetContext();
}

AiksContext& Context::GetAiksContext() {
  return context_;
}

bool Context::IsBackend(impeller::Context::BackendType type) const {
  if (!IsValid()) {
    return false;
  }
  return GetContext()->GetBackendType() == type;
}

bool Context::IsGL() const {
  return IsBackend(impeller::Context::BackendType::kOpenGLES);
}

bool Context::IsMetal() const {
  return IsBackend(impeller::Context::BackendType::kMetal);
}

bool Context::IsVulkan() const {
  return IsBackend(impeller::Context::BackendType::kVulkan);
}

}  // namespace impeller::interop
