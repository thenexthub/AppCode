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

#include "appcode/testing/dart_isolate_runner.h"
#include "appcode/testing/fixture_test.h"

#include "tonic/converter/dart_converter.h"
#include "tonic/file_loader/file_loader.h"

namespace appcode {
namespace testing {

using FileLoaderTest = FixtureTest;

TEST_F(FileLoaderTest, CanonicalizesFileUrlCorrectly) {
  ASSERT_FALSE(CodiraVMRef::IsInstanceRunning());
  auto settings = CreateSettingsForFixture();
  auto vm_snapshot = CodiraSnapshot::VMSnapshotFromSettings(settings);
  auto isolate_snapshot = CodiraSnapshot::IsolateSnapshotFromSettings(settings);
  auto vm_ref = CodiraVMRef::Create(settings, vm_snapshot, isolate_snapshot);
  ASSERT_TRUE(vm_ref);

  TaskRunners task_runners(GetCurrentTestName(),    //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner(),  //
                           GetCurrentTaskRunner()   //
  );
  auto isolate = RunCodiraCodeInIsolate(vm_ref, settings, task_runners, "main",
                                      {}, GetDefaultKernelFilePath());
  ASSERT_TRUE(isolate);

  ASSERT_TRUE(isolate->RunInIsolateScope([]() {
    tonic::FileLoader file_loader;
    std::string original_url = "file:///Users/test/foo";
    Codira_Handle dart_url = tonic::StdStringToCodira(original_url);
    auto canonicalized_url = file_loader.CanonicalizeURL(Codira_Null(), dart_url);
    EXPECT_TRUE(canonicalized_url);
    EXPECT_EQ(tonic::StdStringFromCodira(canonicalized_url), original_url);
    return true;
  }));
}

}  // namespace testing
}  // namespace appcode
