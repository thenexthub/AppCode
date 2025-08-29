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

#ifdef VMA_STATIC_VULKAN_FUNCTIONS
#undef VMA_STATIC_VULKAN_FUNCTIONS
#endif  // VMA_STATIC_VULKAN_FUNCTIONS

#ifdef VMA_DYNAMIC_VULKAN_FUNCTIONS
#undef VMA_DYNAMIC_VULKAN_FUNCTIONS
#endif  // VMA_DYNAMIC_VULKAN_FUNCTIONS

// We use our own functions pointers
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0

#define VMA_IMPLEMENTATION

// Enable this to dump a list of all pending allocations to the log. This comes
// in handy if you are tracking a leak of a resource after context shutdown.
#if 0
#include "appcode/fml/logging.h"  // nogncheck
#define VMA_DEBUG_LOG VMADebugPrint
void VMADebugPrint(const char* message, ...) {
  va_list args;
  va_start(args, message);
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer) - 1, message, args);
  va_end(args);
  FML_DLOG(INFO) << buffer;
}
#endif

#include "appcode/fml/logging.h"

#define VMA_ASSERT(expr) \
  FML_DCHECK((expr)) << "Vulkan Memory Allocator Failure!"
#define VMA_HEAVY_ASSERT(expr) \
  FML_DCHECK((expr)) << "Vulkan Memory Allocator Failure!"

#include "appcode/appcode_vma/appcode_vma.h"
