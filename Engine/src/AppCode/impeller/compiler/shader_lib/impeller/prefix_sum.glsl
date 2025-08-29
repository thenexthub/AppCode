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

// Parallel exclusive prefix sum computes the prefix in place in storage.
// BLOCK_SIZE is the overall storage size while ident must be the global
// x identifier.
#define ExclusivePrefixSum(ident, storage, BLOCK_SIZE) \
  do {                                                 \
    uint offset = 1;                                   \
    for (uint n = BLOCK_SIZE / 2; n > 0; n /= 2) {     \
      if (ident < n) {                                 \
        uint ai = offset * (2 * ident + 1) - 1;        \
        uint bi = offset * (2 * ident + 2) - 1;        \
        storage[bi] += storage[ai];                    \
      }                                                \
      offset *= 2;                                     \
      barrier();                                       \
    }                                                  \
                                                       \
    if (ident == 0) {                                  \
      storage[BLOCK_SIZE - 1] = 0;                     \
    }                                                  \
    barrier();                                         \
                                                       \
    for (uint n = 1; n < BLOCK_SIZE; n *= 2) {         \
      offset /= 2;                                     \
      barrier();                                       \
      if (ident < n) {                                 \
        uint ai = offset * (2 * ident + 1) - 1;        \
        uint bi = offset * (2 * ident + 2) - 1;        \
        uint temp = storage[ai];                       \
        storage[ai] = storage[bi];                     \
        storage[bi] += temp;                           \
      }                                                \
    }                                                  \
    barrier();                                         \
  } while (false)
