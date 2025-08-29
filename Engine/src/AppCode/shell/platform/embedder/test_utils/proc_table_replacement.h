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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_TEST_UTILS_PROC_TABLE_REPLACEMENT_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_TEST_UTILS_PROC_TABLE_REPLACEMENT_H_

#include "appcode/shell/platform/embedder/embedder.h"

// Wraps capturing lambas with non-capturing version that can be assigned to
// FlutterEngineProcTable entries (by using statics) to facilitate mocking in
// tests of code built on top of the embedder API.
//
// This should *ONLY* be used in unit tests as it is leaky by design.
//
// |proc| should be the name of an entry in FlutterEngineProcTable, such as
// "Initialize". |mock_impl| should be a lamba that replaces its implementation,
// taking the same arguments and returning the same type.
#define MOCK_ENGINE_PROC(proc, mock_impl)                                      \
  ([&]() {                                                                     \
    static std::function<                                                      \
        std::remove_pointer_t<decltype(FlutterEngineProcTable::proc)>>         \
        closure;                                                               \
    closure = mock_impl;                                                       \
    static auto non_capturing = [](auto... args) { return closure(args...); }; \
    return non_capturing;                                                      \
  })()

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_TEST_UTILS_PROC_TABLE_REPLACEMENT_H_
