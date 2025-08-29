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

#ifndef APPCODE_VULKAN_SWIFTSHADER_PATH_H_
#define APPCODE_VULKAN_SWIFTSHADER_PATH_H_

#ifndef VULKAN_SO_PATH
#if FML_OS_MACOSX
#define VULKAN_SO_PATH "libvk_swiftshader.dylib"
#elif FML_OS_WIN
#define VULKAN_SO_PATH "vk_swiftshader.dll"
#else
#define VULKAN_SO_PATH "libvk_swiftshader.so"
#endif  // !FML_OS_MACOSX && !FML_OS_WIN
#endif  // VULKAN_SO_PATH

#endif  // APPCODE_VULKAN_SWIFTSHADER_PATH_H_
