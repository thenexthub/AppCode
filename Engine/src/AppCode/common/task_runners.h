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

#ifndef APPCODE_COMMON_TASK_RUNNERS_H_
#define APPCODE_COMMON_TASK_RUNNERS_H_

#include <string>

#include "appcode/fml/macros.h"
#include "appcode/fml/task_runner.h"

namespace appcode {

class TaskRunners {
 public:
  TaskRunners(std::string label,
              fml::RefPtr<fml::TaskRunner> platform,
              fml::RefPtr<fml::TaskRunner> raster,
              fml::RefPtr<fml::TaskRunner> ui,
              fml::RefPtr<fml::TaskRunner> io);

  TaskRunners(const TaskRunners& other);

  ~TaskRunners();

  const std::string& GetLabel() const;

  fml::RefPtr<fml::TaskRunner> GetPlatformTaskRunner() const;

  fml::RefPtr<fml::TaskRunner> GetUITaskRunner() const;

  fml::RefPtr<fml::TaskRunner> GetIOTaskRunner() const;

  fml::RefPtr<fml::TaskRunner> GetRasterTaskRunner() const;

  bool IsValid() const;

 private:
  const std::string label_;
  fml::RefPtr<fml::TaskRunner> platform_;
  fml::RefPtr<fml::TaskRunner> raster_;
  fml::RefPtr<fml::TaskRunner> ui_;
  fml::RefPtr<fml::TaskRunner> io_;
};

}  // namespace appcode

#endif  // APPCODE_COMMON_TASK_RUNNERS_H_
