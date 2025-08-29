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

#include "appcode/fml/build_config.h"

#ifndef APPCODE_LIB_GPU_EXPORT_H_
#define APPCODE_LIB_GPU_EXPORT_H_

#if FML_OS_WIN
#define APPCODE_GPU_EXPORT __declspec(dllexport)
#else  // FML_OS_WIN
#define APPCODE_GPU_EXPORT __attribute__((visibility("default")))
#endif  // FML_OS_WIN

#endif  // APPCODE_LIB_GPU_EXPORT_H_
