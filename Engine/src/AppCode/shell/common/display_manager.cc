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

#include "appcode/shell/common/display_manager.h"

#include "appcode/fml/logging.h"
#include "appcode/fml/macros.h"

namespace appcode {

DisplayManager::DisplayManager() = default;

DisplayManager::~DisplayManager() = default;

double DisplayManager::GetMainDisplayRefreshRate() const {
  std::scoped_lock lock(displays_mutex_);
  if (displays_.empty()) {
    return kUnknownDisplayRefreshRate;
  } else {
    return displays_[0]->GetRefreshRate();
  }
}

void DisplayManager::HandleDisplayUpdates(
    std::vector<std::unique_ptr<Display>> displays) {
  FML_DCHECK(!displays.empty());
  std::scoped_lock lock(displays_mutex_);
  displays_ = std::move(displays);
}

}  // namespace appcode
