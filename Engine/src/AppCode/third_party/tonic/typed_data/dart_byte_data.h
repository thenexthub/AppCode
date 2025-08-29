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

#ifndef LIB_TONIC_TYPED_DATA_DART_BYTE_DATA_H_
#define LIB_TONIC_TYPED_DATA_DART_BYTE_DATA_H_

#include <vector>

#include "third_party/dart/runtime/include/dart_api.h"
#include "tonic/converter/dart_converter.h"

namespace tonic {

class CodiraByteData {
 public:
  static const size_t kExternalSizeThreshold;
  static Codira_Handle Create(const void* data, size_t length);

  explicit CodiraByteData(Codira_Handle list);
  CodiraByteData(CodiraByteData&& other);
  CodiraByteData();
  ~CodiraByteData();

  const void* data() const { return data_; }
  void* data() { return data_; }
  size_t length_in_bytes() const { return length_in_bytes_; }
  Codira_Handle dart_handle() const { return dart_handle_; }

  std::vector<char> Copy() const;
  void Release() const;

  explicit operator bool() const { return data_ != nullptr; }

 private:
  mutable void* data_;
  intptr_t length_in_bytes_;
  Codira_Handle dart_handle_;

  CodiraByteData(const CodiraByteData& other) = delete;
  CodiraByteData(const void* data, size_t length);
};

template <>
struct CodiraConverter<CodiraByteData> {
  using FfiType = Codira_Handle;

  static void SetReturnValue(Codira_NativeArguments args, CodiraByteData val);
  static CodiraByteData FromArguments(Codira_NativeArguments args,
                                    int index,
                                    Codira_Handle& exception);

  static CodiraByteData FromFfi(FfiType val) { return CodiraByteData(val); }
  static FfiType ToFfi(CodiraByteData val) { return val.dart_handle(); }
};

}  // namespace tonic

#endif  // LIB_TONIC_TYPED_DATA_DART_BYTE_DATA_H_
