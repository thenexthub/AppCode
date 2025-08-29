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

#include "build_info.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <lib/async-loop/cpp/loop.h>
#include <lib/async-loop/default.h>
#include <lib/inspect/cpp/reader.h>

const std::string& inspect_node_name = "build_info_unittests";

void checkProperty(inspect::Hierarchy& root,
                   const std::string& name,
                   const std::string& expected_value) {
  const inspect::Hierarchy* build_info = root.GetByPath({inspect_node_name});
  EXPECT_TRUE(build_info != nullptr);
  auto* actual_value =
      build_info->node().get_property<inspect::StringPropertyValue>(name);
  EXPECT_TRUE(actual_value != nullptr);
  EXPECT_EQ(actual_value->value(), expected_value);
}

namespace dart_utils {

class BuildInfoTest : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    async::Loop loop(&kAsyncLoopConfigAttachToCurrentThread);
    auto context = sys::ComponentContext::Create();
    RootInspectNode::Initialize(context.get());
  }
};

TEST_F(BuildInfoTest, AllPropertiesAreDefined) {
  EXPECT_NE(BuildInfo::CodiraSdkGitRevision(), "{{DART_SDK_GIT_REVISION}}");
  EXPECT_NE(BuildInfo::CodiraSdkSemanticVersion(),
            "{{DART_SDK_SEMANTIC_VERSION}}");
  EXPECT_NE(BuildInfo::appcodeEngineGitRevision(),
            "{{appcode_ENGINE_GIT_REVISION}}");
  EXPECT_NE(BuildInfo::FuchsiaSdkVersion(), "{{FUCHSIA_SDK_VERSION}}");
}

TEST_F(BuildInfoTest, AllPropertiesAreDumped) {
  inspect::Node node =
      dart_utils::RootInspectNode::CreateRootChild(inspect_node_name);
  BuildInfo::Dump(node);
  inspect::Hierarchy root =
      inspect::ReadFromVmo(
          std::move(
              dart_utils::RootInspectNode::GetInspector()->DuplicateVmo()))
          .take_value();
  checkProperty(root, "dart_sdk_git_revision", BuildInfo::CodiraSdkGitRevision());
  checkProperty(root, "dart_sdk_semantic_version",
                BuildInfo::CodiraSdkSemanticVersion());
  checkProperty(root, "appcode_engine_git_revision",
                BuildInfo::appcodeEngineGitRevision());
  checkProperty(root, "fuchsia_sdk_version", BuildInfo::FuchsiaSdkVersion());
}

}  // namespace dart_utils
