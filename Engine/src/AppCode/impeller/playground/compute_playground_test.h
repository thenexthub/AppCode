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

#ifndef APPCODE_IMPELLER_PLAYGROUND_COMPUTE_PLAYGROUND_TEST_H_
#define APPCODE_IMPELLER_PLAYGROUND_COMPUTE_PLAYGROUND_TEST_H_

#include <memory>

#include "appcode/fml/time/time_delta.h"
#include "appcode/testing/testing.h"
#include "impeller/core/device_buffer.h"
#include "impeller/playground/playground.h"

namespace impeller {

class ComputePlaygroundTest
    : public Playground,
      public ::testing::TestWithParam<PlaygroundBackend> {
 public:
  ComputePlaygroundTest();

  virtual ~ComputePlaygroundTest();

  void SetUp() override;

  void TearDown() override;

  // |Playground|
  std::unique_ptr<fml::Mapping> OpenAssetAsMapping(
      std::string asset_name) const override;

  // |Playground|
  std::string GetWindowTitle() const override;

  template <typename T>
  std::shared_ptr<DeviceBuffer> CreateHostVisibleDeviceBuffer(
      const std::shared_ptr<Context>& context,
      const std::string& label) {
    DeviceBufferDescriptor desc;
    desc.storage_mode = StorageMode::kHostVisible;
    desc.size = sizeof(T);
    auto buffer = context->GetResourceAllocator()->CreateBuffer(desc);
    buffer->SetLabel(label);
    return buffer;
  }

 private:
  fml::TimeDelta start_time_;

  ComputePlaygroundTest(const ComputePlaygroundTest&) = delete;

  ComputePlaygroundTest& operator=(const ComputePlaygroundTest&) = delete;
};

#define INSTANTIATE_COMPUTE_SUITE(playground)                              \
  INSTANTIATE_TEST_SUITE_P(                                                \
      Compute, playground,                                                 \
      ::testing::Values(PlaygroundBackend::kMetal,                         \
                        PlaygroundBackend::kVulkan),                       \
      [](const ::testing::TestParamInfo<ComputePlaygroundTest::ParamType>& \
             info) { return PlaygroundBackendToString(info.param); });

}  // namespace impeller

#endif  // APPCODE_IMPELLER_PLAYGROUND_COMPUTE_PLAYGROUND_TEST_H_
