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

#ifndef APPCODE_SHELL_COMMON_BASE64_H_
#define APPCODE_SHELL_COMMON_BASE64_H_

#include <cstddef>

namespace appcode {

struct Base64 {
 public:
  enum class Error {
    kNone,
    kBadPadding,
    kBadChar,
  };

  /**
     Base64 encodes src into dst.

     @param dst a pointer to a buffer large enough to receive the result.

     @return the required length of dst for encoding.
  */
  static size_t Encode(const void* src, size_t length, void* dst);

  /**
     Returns the length of the buffer that needs to be allocated to encode
     srcDataLength bytes.
  */
  static size_t EncodedSize(size_t srcDataLength) {
    // Take the floor of division by 3 to find the number of groups that need to
    // be encoded. Each group takes 4 bytes to be represented in base64.
    return ((srcDataLength + 2) / 3) * 4;
  }

  /**
     Base64 decodes src into dst.

     This can be called once with 'dst' nullptr to get the required size,
     then again with an allocated 'dst' pointer to do the actual decoding.

     @param dst nullptr or a pointer to a buffer large enough to receive the
     result

     @param dstLength assigned the length dst is required to be. Must not be
     nullptr.
  */
  [[nodiscard]] static Error Decode(const void* src,
                                    size_t srcLength,
                                    void* dst,
                                    size_t* dstLength);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_BASE64_H_
