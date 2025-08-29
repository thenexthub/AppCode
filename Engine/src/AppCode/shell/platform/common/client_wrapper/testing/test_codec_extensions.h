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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TESTING_TEST_CODEC_EXTENSIONS_H_
#define APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TESTING_TEST_CODEC_EXTENSIONS_H_

#include "appcode/shell/platform/common/client_wrapper/include/appcode/encodable_value.h"
#include "appcode/shell/platform/common/client_wrapper/include/appcode/standard_codec_serializer.h"

namespace appcode {

// A representation of a point, for custom type testing of a simple type.
class Point {
 public:
  Point(int x, int y) : x_(x), y_(y) {}
  ~Point() = default;

  int x() const { return x_; }
  int y() const { return y_; }

  bool operator==(const Point& other) const {
    return x_ == other.x_ && y_ == other.y_;
  }

 private:
  int x_;
  int y_;
};

// A typed binary data object with extra fields, for custom type testing of a
// variable-length type that includes types handled by the core standard codec.
class SomeData {
 public:
  SomeData(const std::string& label, const std::vector<uint8_t>& data)
      : label_(label), data_(data) {}
  ~SomeData() = default;

  const std::string& label() const { return label_; }
  const std::vector<uint8_t>& data() const { return data_; }

 private:
  std::string label_;
  std::vector<uint8_t> data_;
};

// Codec extension for Point.
class PointExtensionSerializer : public StandardCodecSerializer {
 public:
  PointExtensionSerializer();
  virtual ~PointExtensionSerializer();

  static const PointExtensionSerializer& GetInstance();

  // |TestCodecSerializer|
  EncodableValue ReadValueOfType(uint8_t type,
                                 ByteStreamReader* stream) const override;

  // |TestCodecSerializer|
  void WriteValue(const EncodableValue& value,
                  ByteStreamWriter* stream) const override;

 private:
  static constexpr uint8_t kPointType = 128;
};

// Codec extension for SomeData.
class SomeDataExtensionSerializer : public StandardCodecSerializer {
 public:
  SomeDataExtensionSerializer();
  virtual ~SomeDataExtensionSerializer();

  static const SomeDataExtensionSerializer& GetInstance();

  // |TestCodecSerializer|
  EncodableValue ReadValueOfType(uint8_t type,
                                 ByteStreamReader* stream) const override;

  // |TestCodecSerializer|
  void WriteValue(const EncodableValue& value,
                  ByteStreamWriter* stream) const override;

 private:
  static constexpr uint8_t kSomeDataType = 129;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_CLIENT_WRAPPER_TESTING_TEST_CODEC_EXTENSIONS_H_
