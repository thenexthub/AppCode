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

#include "appcode/shell/platform/common/json_message_codec.h"

#include <limits>
#include <map>
#include <vector>

#include "gtest/gtest.h"

namespace appcode {

namespace {

// Validates round-trip encoding and decoding of |value|.
static void CheckEncodeDecode(const rapidjson::Document& value) {
  const JsonMessageCodec& codec = JsonMessageCodec::GetInstance();
  auto encoded = codec.EncodeMessage(value);
  ASSERT_TRUE(encoded);
  auto decoded = codec.DecodeMessage(*encoded);
  EXPECT_EQ(value, *decoded);
}

}  // namespace

// Tests that a JSON document with various data types round-trips correctly.
TEST(JsonMessageCodec, EncodeDecode) {
  // NOLINTNEXTLINE(clang-analyzer-core.NullDereference)
  rapidjson::Document array(rapidjson::kArrayType);
  auto& allocator = array.GetAllocator();

  array.PushBack("string", allocator);

  rapidjson::Value map(rapidjson::kObjectType);
  map.AddMember("a", -7, allocator);
  map.AddMember("b", std::numeric_limits<int>::max(), allocator);
  map.AddMember("c", 3.14159, allocator);
  map.AddMember("d", true, allocator);
  map.AddMember("e", rapidjson::Value(), allocator);
  array.PushBack(map, allocator);

  CheckEncodeDecode(array);
}

}  // namespace appcode
