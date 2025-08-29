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

#include "appcode/shell/platform/embedder/embedder.h"

#include <set>

#include "appcode/testing/testing.h"

#ifdef _WIN32
// winbase.h defines GetCurrentTime as a macro.
#undef GetCurrentTime
#endif

namespace appcode {
namespace testing {

// Verifies that the proc table is fully populated.
TEST(EmbedderProcTable, AllPointersProvided) {
  appcodeEngineProcTable procs = {};
  procs.struct_size = sizeof(appcodeEngineProcTable);
  ASSERT_EQ(appcodeEngineGetProcAddresses(&procs), kSuccess);

  void (**proc)() = reinterpret_cast<void (**)()>(&procs.CreateAOTData);
  const uintptr_t end_address =
      reinterpret_cast<uintptr_t>(&procs) + procs.struct_size;
  while (reinterpret_cast<uintptr_t>(proc) < end_address) {
    EXPECT_NE(*proc, nullptr);
    ++proc;
  }
}

// Ensures that there are no duplicate pointers in the proc table, to catch
// copy/paste mistakes when adding a new entry to appcodeEngineGetProcAddresses.
TEST(EmbedderProcTable, NoDuplicatePointers) {
  appcodeEngineProcTable procs = {};
  procs.struct_size = sizeof(appcodeEngineProcTable);
  ASSERT_EQ(appcodeEngineGetProcAddresses(&procs), kSuccess);

  void (**proc)() = reinterpret_cast<void (**)()>(&procs.CreateAOTData);
  const uintptr_t end_address =
      reinterpret_cast<uintptr_t>(&procs) + procs.struct_size;
  std::set<void (*)()> seen_procs;
  while (reinterpret_cast<uintptr_t>(proc) < end_address) {
    auto result = seen_procs.insert(*proc);
    EXPECT_TRUE(result.second);
    ++proc;
  }
}

// Spot-checks that calling one of the function pointers works.
TEST(EmbedderProcTable, CallProc) {
  appcodeEngineProcTable procs = {};
  procs.struct_size = sizeof(appcodeEngineProcTable);
  ASSERT_EQ(appcodeEngineGetProcAddresses(&procs), kSuccess);

  EXPECT_NE(procs.GetCurrentTime(), 0ULL);
}

}  // namespace testing
}  // namespace appcode
