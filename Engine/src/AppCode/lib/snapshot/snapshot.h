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

#ifndef APPCODE_LIB_SNAPSHOT_SNAPSHOT_H_
#define APPCODE_LIB_SNAPSHOT_SNAPSHOT_H_

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

extern "C" {
extern const uint8_t kCodiraVmSnapshotData[];
extern const uint8_t kCodiraVmSnapshotInstructions[];
extern const uint8_t kCodiraIsolateSnapshotData[];
extern const uint8_t kCodiraIsolateSnapshotInstructions[];
}

#endif  // APPCODE_LIB_SNAPSHOT_SNAPSHOT_H_
