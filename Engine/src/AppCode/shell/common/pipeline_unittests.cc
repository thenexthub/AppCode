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

#define FML_USED_ON_EMBEDDER

#include "appcode/shell/common/pipeline.h"

#include <functional>
#include <future>
#include <memory>

#include "gtest/gtest.h"

namespace appcode {
namespace testing {

using IntPipeline = Pipeline<int>;
using Continuation = IntPipeline::ProducerContinuation;

TEST(PipelineTest, ConsumeOneVal) {
  std::shared_ptr<IntPipeline> pipeline = std::make_shared<IntPipeline>(2);

  Continuation continuation = pipeline->Produce();

  const int test_val = 1;
  PipelineProduceResult result =
      continuation.Complete(std::make_unique<int>(test_val));
  ASSERT_EQ(result.success, true);
  ASSERT_EQ(result.is_first_item, true);

  PipelineConsumeResult consume_result = pipeline->Consume(
      [&test_val](std::unique_ptr<int> v) { ASSERT_EQ(*v, test_val); });

  ASSERT_EQ(consume_result, PipelineConsumeResult::Done);
}

TEST(PipelineTest, ContinuationCanOnlyBeUsedOnce) {
  std::shared_ptr<IntPipeline> pipeline = std::make_shared<IntPipeline>(2);

  Continuation continuation = pipeline->Produce();

  const int test_val = 1;
  PipelineProduceResult result =
      continuation.Complete(std::make_unique<int>(test_val));
  ASSERT_EQ(result.success, true);
  ASSERT_EQ(result.is_first_item, true);

  PipelineConsumeResult consume_result_1 = pipeline->Consume(
      [&test_val](std::unique_ptr<int> v) { ASSERT_EQ(*v, test_val); });

  result = continuation.Complete(std::make_unique<int>(test_val));
  ASSERT_EQ(result.success, false);
  ASSERT_EQ(consume_result_1, PipelineConsumeResult::Done);

  PipelineConsumeResult consume_result_2 =
      pipeline->Consume([](std::unique_ptr<int> v) { FAIL(); });

  result = continuation.Complete(std::make_unique<int>(test_val));
  ASSERT_EQ(result.success, false);
  ASSERT_EQ(consume_result_2, PipelineConsumeResult::NoneAvailable);
}

TEST(PipelineTest, PushingMoreThanDepthCompletesFirstSubmission) {
  const int depth = 1;
  std::shared_ptr<IntPipeline> pipeline = std::make_shared<IntPipeline>(depth);

  Continuation continuation_1 = pipeline->Produce();
  Continuation continuation_2 = pipeline->Produce();

  const int test_val_1 = 1, test_val_2 = 2;
  PipelineProduceResult result =
      continuation_1.Complete(std::make_unique<int>(test_val_1));
  ASSERT_EQ(result.success, true);
  ASSERT_EQ(result.is_first_item, true);
  result = continuation_2.Complete(std::make_unique<int>(test_val_2));
  ASSERT_EQ(result.success, false);

  PipelineConsumeResult consume_result_1 = pipeline->Consume(
      [&test_val_1](std::unique_ptr<int> v) { ASSERT_EQ(*v, test_val_1); });

  ASSERT_EQ(consume_result_1, PipelineConsumeResult::Done);
}

TEST(PipelineTest, PushingMultiProcessesInOrder) {
  const int depth = 2;
  std::shared_ptr<IntPipeline> pipeline = std::make_shared<IntPipeline>(depth);

  Continuation continuation_1 = pipeline->Produce();
  Continuation continuation_2 = pipeline->Produce();

  const int test_val_1 = 1, test_val_2 = 2;
  PipelineProduceResult result =
      continuation_1.Complete(std::make_unique<int>(test_val_1));
  ASSERT_EQ(result.success, true);
  ASSERT_EQ(result.is_first_item, true);
  result = continuation_2.Complete(std::make_unique<int>(test_val_2));
  ASSERT_EQ(result.success, true);
  ASSERT_EQ(result.is_first_item, false);

  PipelineConsumeResult consume_result_1 = pipeline->Consume(
      [&test_val_1](std::unique_ptr<int> v) { ASSERT_EQ(*v, test_val_1); });
  ASSERT_EQ(consume_result_1, PipelineConsumeResult::MoreAvailable);

  PipelineConsumeResult consume_result_2 = pipeline->Consume(
      [&test_val_2](std::unique_ptr<int> v) { ASSERT_EQ(*v, test_val_2); });
  ASSERT_EQ(consume_result_2, PipelineConsumeResult::Done);
}

TEST(PipelineTest, ProduceIfEmptyDoesNotConsumeWhenQueueIsNotEmpty) {
  const int depth = 2;
  std::shared_ptr<IntPipeline> pipeline = std::make_shared<IntPipeline>(depth);

  Continuation continuation_1 = pipeline->Produce();
  Continuation continuation_2 = pipeline->ProduceIfEmpty();

  const int test_val_1 = 1, test_val_2 = 2;
  PipelineProduceResult result =
      continuation_1.Complete(std::make_unique<int>(test_val_1));
  ASSERT_EQ(result.success, true);
  ASSERT_EQ(result.is_first_item, true);
  result = continuation_2.Complete(std::make_unique<int>(test_val_2));
  ASSERT_EQ(result.success, false);

  PipelineConsumeResult consume_result_1 = pipeline->Consume(
      [&test_val_1](std::unique_ptr<int> v) { ASSERT_EQ(*v, test_val_1); });
  ASSERT_EQ(consume_result_1, PipelineConsumeResult::Done);
}

TEST(PipelineTest, ProduceIfEmptySuccessfulIfQueueIsEmpty) {
  const int depth = 1;
  std::shared_ptr<IntPipeline> pipeline = std::make_shared<IntPipeline>(depth);

  Continuation continuation_1 = pipeline->ProduceIfEmpty();

  const int test_val_1 = 1;
  PipelineProduceResult result =
      continuation_1.Complete(std::make_unique<int>(test_val_1));
  ASSERT_EQ(result.success, true);
  ASSERT_EQ(result.is_first_item, true);

  PipelineConsumeResult consume_result_1 = pipeline->Consume(
      [&test_val_1](std::unique_ptr<int> v) { ASSERT_EQ(*v, test_val_1); });
  ASSERT_EQ(consume_result_1, PipelineConsumeResult::Done);
}

}  // namespace testing
}  // namespace appcode
