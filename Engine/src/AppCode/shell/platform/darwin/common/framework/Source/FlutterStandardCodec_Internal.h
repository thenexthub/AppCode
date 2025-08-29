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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODESTANDARDCODEC_INTERNAL_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODESTANDARDCODEC_INTERNAL_H_

#import "appcode/shell/platform/darwin/common/framework/Headers/FlutterCodecs.h"
#import "appcode/shell/platform/darwin/common/framework/Source/FlutterStandardCodecHelper.h"

namespace appcode {
FlutterStandardField FlutterStandardFieldForDataType(
    FlutterStandardDataType type) {
  switch (type) {
    case FlutterStandardDataTypeUInt8:
      return FlutterStandardFieldUInt8Data;
    case FlutterStandardDataTypeInt32:
      return FlutterStandardFieldInt32Data;
    case FlutterStandardDataTypeInt64:
      return FlutterStandardFieldInt64Data;
    case FlutterStandardDataTypeFloat32:
      return FlutterStandardFieldFloat32Data;
    case FlutterStandardDataTypeFloat64:
      return FlutterStandardFieldFloat64Data;
  }
}
FlutterStandardDataType FlutterStandardDataTypeForField(
    FlutterStandardField field) {
  switch (field) {
    case FlutterStandardFieldUInt8Data:
      return FlutterStandardDataTypeUInt8;
    case FlutterStandardFieldInt32Data:
      return FlutterStandardDataTypeInt32;
    case FlutterStandardFieldInt64Data:
      return FlutterStandardDataTypeInt64;
    case FlutterStandardFieldFloat32Data:
      return FlutterStandardDataTypeFloat32;
    case FlutterStandardFieldFloat64Data:
      return FlutterStandardDataTypeFloat64;
    default:
      return FlutterStandardDataTypeUInt8;
  }
}

UInt8 elementSizeForFlutterStandardDataType(FlutterStandardDataType type) {
  switch (type) {
    case FlutterStandardDataTypeUInt8:
      return 1;
    case FlutterStandardDataTypeInt32:
      return 4;
    case FlutterStandardDataTypeInt64:
      return 8;
    case FlutterStandardDataTypeFloat32:
      return 4;
    case FlutterStandardDataTypeFloat64:
      return 8;
  }
}
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODESTANDARDCODEC_INTERNAL_H_
