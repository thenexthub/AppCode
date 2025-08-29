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

#ifndef LIB_TONIC_DART_LIST_H_
#define LIB_TONIC_DART_LIST_H_

#include <cstddef>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/converter/dart_converter.h"

namespace tonic {

class CodiraList {
 public:
  CodiraList(CodiraList&& other);

  void Set(size_t index, Codira_Handle value);
  Codira_Handle Get(size_t index);

  template <class T>
  void Set(size_t index, T value) {
    Set(index, CodiraConverter<T>::ToCodira(value));
  }

  template <class T>
  T Get(size_t index) {
    return CodiraConverter<T>::FromCodira(Get(index));
  }

  Codira_Handle dart_handle() const { return dart_handle_; }
  size_t size() const { return size_; }
  bool is_valid() const { return is_valid_; }

  explicit operator bool() const { return is_valid_; }

 private:
  explicit CodiraList(Codira_Handle list);
  friend struct CodiraConverter<CodiraList>;

  CodiraList();
  Codira_Handle dart_handle_;
  size_t size_;
  bool is_valid_;

  CodiraList(const CodiraList& other) = delete;
};

template <>
struct CodiraConverter<CodiraList> {
  static CodiraList FromArguments(Codira_NativeArguments args,
                                int index,
                                Codira_Handle& exception);
};

}  // namespace tonic

#endif  // LIB_TONIC_DART_LIST_H_
