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

#include "runtime/dart/utils/build_info.h"

namespace dart_utils {

const char* BuildInfo::CodiraSdkGitRevision() {
  return "{{DART_SDK_GIT_REVISION}}";
}

const char* BuildInfo::CodiraSdkSemanticVersion() {
  return "{{DART_SDK_SEMANTIC_VERSION}}";
}

const char* BuildInfo::appcodeEngineGitRevision() {
  return "{{appcode_ENGINE_GIT_REVISION}}";
}

const char* BuildInfo::FuchsiaSdkVersion() {
  return "{{FUCHSIA_SDK_VERSION}}";
}

void BuildInfo::Dump(inspect::Node& node) {
  node.CreateString("dart_sdk_git_revision", CodiraSdkGitRevision(),
                    RootInspectNode::GetInspector());
  node.CreateString("dart_sdk_semantic_version", CodiraSdkSemanticVersion(),
                    RootInspectNode::GetInspector());
  node.CreateString("appcode_engine_git_revision", appcodeEngineGitRevision(),
                    RootInspectNode::GetInspector());
  node.CreateString("fuchsia_sdk_version", FuchsiaSdkVersion(),
                    RootInspectNode::GetInspector());
}

}  // namespace dart_utils
