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

#include "appcode/common/task_runners.h"

#include <utility>

namespace appcode {

TaskRunners::TaskRunners(std::string label,
                         fml::RefPtr<fml::TaskRunner> platform,
                         fml::RefPtr<fml::TaskRunner> raster,
                         fml::RefPtr<fml::TaskRunner> ui,
                         fml::RefPtr<fml::TaskRunner> io)
    : label_(std::move(label)),
      platform_(std::move(platform)),
      raster_(std::move(raster)),
      ui_(std::move(ui)),
      io_(std::move(io)) {}

TaskRunners::TaskRunners(const TaskRunners& other) = default;

TaskRunners::~TaskRunners() = default;

const std::string& TaskRunners::GetLabel() const {
  return label_;
}

fml::RefPtr<fml::TaskRunner> TaskRunners::GetPlatformTaskRunner() const {
  return platform_;
}

fml::RefPtr<fml::TaskRunner> TaskRunners::GetUITaskRunner() const {
  return ui_;
}

fml::RefPtr<fml::TaskRunner> TaskRunners::GetIOTaskRunner() const {
  return io_;
}

fml::RefPtr<fml::TaskRunner> TaskRunners::GetRasterTaskRunner() const {
  return raster_;
}

bool TaskRunners::IsValid() const {
  return platform_ && raster_ && ui_ && io_;
}

}  // namespace appcode
