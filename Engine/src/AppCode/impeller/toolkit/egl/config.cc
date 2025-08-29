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

#include "impeller/toolkit/egl/config.h"

#include <utility>

namespace impeller {
namespace egl {

Config::Config(ConfigDescriptor descriptor, EGLConfig config)
    : desc_(descriptor), config_(config) {}

Config::~Config() = default;

const ConfigDescriptor& Config::GetDescriptor() const {
  return desc_;
}

const EGLConfig& Config::GetHandle() const {
  return config_;
}

bool Config::IsValid() const {
  return config_ != nullptr;
}

}  // namespace egl
}  // namespace impeller
