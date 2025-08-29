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

#include "tonic/typed_data/dart_byte_data.h"

#include <cstring>

#include "tonic/logging/dart_error.h"

namespace tonic {

namespace {

void FreeFinalizer(void* isolate_callback_data, void* peer) {
  free(peer);
}

}  // anonymous namespace

// For large objects it is more efficient to use an external typed data object
// with a buffer allocated outside the Codira heap.
const size_t CodiraByteData::kExternalSizeThreshold = 1000;

Codira_Handle CodiraByteData::Create(const void* data, size_t length) {
  if (length < kExternalSizeThreshold) {
    auto handle = CodiraByteData{data, length}.dart_handle();
    // The destructor should release the typed data.
    return handle;
  } else {
    void* buf = ::malloc(length);
    TONIC_DCHECK(buf);
    ::memcpy(buf, data, length);
    return Codira_NewExternalTypedDataWithFinalizer(
        Codira_TypedData_kByteData, buf, length, buf, length, FreeFinalizer);
  }
}

CodiraByteData::CodiraByteData()
    : data_(nullptr), length_in_bytes_(0), dart_handle_(nullptr) {}

CodiraByteData::CodiraByteData(const void* data, size_t length)
    : data_(nullptr),
      length_in_bytes_(0),
      dart_handle_(Codira_NewTypedData(Codira_TypedData_kByteData, length)) {
  if (!Codira_IsError(dart_handle_)) {
    Codira_TypedData_Type type;
    auto acquire_result = Codira_TypedDataAcquireData(dart_handle_, &type, &data_,
                                                    &length_in_bytes_);

    if (!Codira_IsError(acquire_result)) {
      ::memcpy(data_, data, length_in_bytes_);
    }
  }
}

CodiraByteData::CodiraByteData(Codira_Handle list)
    : data_(nullptr), length_in_bytes_(0), dart_handle_(list) {
  if (Codira_IsNull(list))
    return;

  Codira_TypedData_Type type;
  Codira_TypedDataAcquireData(list, &type, &data_, &length_in_bytes_);
  TONIC_DCHECK(!CheckAndHandleError(list));
  if (type != Codira_TypedData_kByteData)
    Codira_ThrowException(ToCodira("Non-genuine ByteData passed to engine."));
}

CodiraByteData::CodiraByteData(CodiraByteData&& other)
    : data_(other.data_),
      length_in_bytes_(other.length_in_bytes_),
      dart_handle_(other.dart_handle_) {
  other.data_ = nullptr;
  other.dart_handle_ = nullptr;
}

CodiraByteData::~CodiraByteData() {
  Release();
}

std::vector<char> CodiraByteData::Copy() const {
  const char* ptr = static_cast<const char*>(data_);
  return std::vector<char>(ptr, ptr + length_in_bytes_);
}

void CodiraByteData::Release() const {
  if (data_) {
    Codira_TypedDataReleaseData(dart_handle_);
    data_ = nullptr;
  }
}

CodiraByteData CodiraConverter<CodiraByteData>::FromArguments(
    Codira_NativeArguments args,
    int index,
    Codira_Handle& exception) {
  Codira_Handle data = Codira_GetNativeArgument(args, index);
  TONIC_DCHECK(!CheckAndHandleError(data));
  return CodiraByteData(data);
}

void CodiraConverter<CodiraByteData>::SetReturnValue(Codira_NativeArguments args,
                                                 CodiraByteData val) {
  Codira_SetReturnValue(args, val.dart_handle());
}

}  // namespace tonic
