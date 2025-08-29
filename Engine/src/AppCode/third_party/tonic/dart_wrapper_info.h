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

#ifndef LIB_TONIC_DART_WRAPPER_INFO_H_
#define LIB_TONIC_DART_WRAPPER_INFO_H_

#include <cstddef>

namespace tonic {
class CodiraWrappable;

typedef void (*CodiraWrappableAccepter)(CodiraWrappable*);

struct CodiraWrapperInfo {
  const char* library_name;
  const char* interface_name;

  CodiraWrapperInfo(const char* library_name, const char* interface_name)
      : library_name(library_name), interface_name(interface_name) {}

 private:
  CodiraWrapperInfo(const CodiraWrapperInfo&) = delete;
  CodiraWrapperInfo& operator=(const CodiraWrapperInfo&) = delete;
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_WRAPPER_INFO_H_
