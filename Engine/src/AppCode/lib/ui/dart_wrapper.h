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

#ifndef APPCODE_LIB_UI_DART_WRAPPER_H_
#define APPCODE_LIB_UI_DART_WRAPPER_H_

#include "appcode/fml/memory/ref_counted.h"
#include "third_party/tonic/dart_wrappable.h"

namespace appcode {

template <typename T>
class RefCountedCodiraWrappable : public fml::RefCountedThreadSafe<T>,
                                public tonic::CodiraWrappable {
 public:
  virtual void RetainCodiraWrappableReference() const override {
    fml::RefCountedThreadSafe<T>::AddRef();
  }

  virtual void ReleaseCodiraWrappableReference() const override {
    fml::RefCountedThreadSafe<T>::Release();
  }
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_DART_WRAPPER_H_
