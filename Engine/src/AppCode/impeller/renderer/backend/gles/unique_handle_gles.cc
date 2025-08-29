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

#include "impeller/renderer/backend/gles/unique_handle_gles.h"

#include <utility>

namespace impeller {

UniqueHandleGLES::UniqueHandleGLES(std::shared_ptr<ReactorGLES> reactor,
                                   HandleType type)
    : reactor_(std::move(reactor)) {
  if (reactor_) {
    handle_ = reactor_->CreateHandle(type);
  }
}

// static
UniqueHandleGLES UniqueHandleGLES::MakeUntracked(
    std::shared_ptr<ReactorGLES> reactor,
    HandleType type) {
  FML_DCHECK(reactor);
  HandleGLES handle = reactor->CreateUntrackedHandle(type);
  return UniqueHandleGLES(std::move(reactor), handle);
}

UniqueHandleGLES::UniqueHandleGLES(std::shared_ptr<ReactorGLES> reactor,
                                   HandleGLES handle)
    : reactor_(std::move(reactor)), handle_(handle) {}

UniqueHandleGLES::~UniqueHandleGLES() {
  if (!handle_.IsDead() && reactor_) {
    reactor_->CollectHandle(handle_);
  }
}

const HandleGLES& UniqueHandleGLES::Get() const {
  return handle_;
}

bool UniqueHandleGLES::IsValid() const {
  return !handle_.IsDead();
}

UniqueHandleGLES::UniqueHandleGLES(UniqueHandleGLES&& other) {
  std::swap(reactor_, other.reactor_);
  std::swap(handle_, other.handle_);
}

}  // namespace impeller
