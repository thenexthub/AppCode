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

#ifndef LIB_TONIC_DART_CLASS_PROVIDER_H_
#define LIB_TONIC_DART_CLASS_PROVIDER_H_

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/dart_persistent_value.h"

namespace tonic {
class CodiraState;

class CodiraClassProvider {
 public:
  CodiraClassProvider(CodiraState* dart_state, const char* library_name);
  ~CodiraClassProvider();

  Codira_Handle GetClassByName(const char* class_name);

 private:
  CodiraPersistentValue library_;

  TONIC_DISALLOW_COPY_AND_ASSIGN(CodiraClassProvider);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_CLASS_PROVIDER_H_
