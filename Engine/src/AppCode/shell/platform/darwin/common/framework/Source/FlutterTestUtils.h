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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODETESTUTILS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODETESTUTILS_H_

#import <Foundation/Foundation.h>

/// Returns YES if the block throws an exception.
BOOL FLTThrowsObjcException(dispatch_block_t block);

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_COMMON_FRAMEWORK_SOURCE_APPCODETESTUTILS_H_
