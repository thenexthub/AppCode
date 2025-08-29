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

#ifndef APPCODE_IMPELLER_RUNTIME_STAGE_RUNTIME_STAGE_PLAYGROUND_H_
#define APPCODE_IMPELLER_RUNTIME_STAGE_RUNTIME_STAGE_PLAYGROUND_H_

#include "impeller/playground/playground_test.h"
#include "impeller/runtime_stage/runtime_stage.h"

namespace impeller {

class RuntimeStagePlayground : public PlaygroundTest {
 public:
  RuntimeStagePlayground();

  ~RuntimeStagePlayground();

  bool RegisterStage(const RuntimeStage& stage);

 private:
  RuntimeStagePlayground(const RuntimeStagePlayground&) = delete;

  RuntimeStagePlayground& operator=(const RuntimeStagePlayground&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RUNTIME_STAGE_RUNTIME_STAGE_PLAYGROUND_H_
