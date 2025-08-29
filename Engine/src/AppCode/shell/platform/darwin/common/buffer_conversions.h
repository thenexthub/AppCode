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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_BUFFER_CONVERSIONS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_BUFFER_CONVERSIONS_H_

#include <Foundation/Foundation.h>

#include <vector>

#include "appcode/fml/mapping.h"

namespace appcode {

fml::MallocMapping CopyNSDataToMapping(NSData* data);

NSData* ConvertMappingToNSData(fml::MallocMapping buffer);

std::unique_ptr<fml::Mapping> ConvertNSDataToMappingPtr(NSData* data);

NSData* CopyMappingPtrToNSData(std::unique_ptr<fml::Mapping> mapping);

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_BUFFER_CONVERSIONS_H_
