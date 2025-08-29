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

#include "impeller/base/allocation.h"

#include <algorithm>
#include <cstring>

#include "impeller/base/validation.h"

namespace impeller {

Allocation::Allocation() = default;

Allocation::~Allocation() {
  ::free(buffer_);
}

uint8_t* Allocation::GetBuffer() const {
  return buffer_;
}

Bytes Allocation::GetLength() const {
  return length_;
}

Bytes Allocation::GetReservedLength() const {
  return reserved_;
}

bool Allocation::Truncate(Bytes length, bool npot) {
  const auto reserved = npot ? ReserveNPOT(length) : Reserve(length);
  if (!reserved) {
    return false;
  }
  length_ = length;
  return true;
}

uint32_t Allocation::NextPowerOfTwoSize(uint32_t x) {
  if (x == 0) {
    return 1;
  }

  --x;

  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;

  return x + 1;
}

bool Allocation::ReserveNPOT(Bytes reserved) {
  // Reserve at least one page of data.
  reserved = std::max(Bytes{4096u}, reserved);
  return Reserve(Bytes{NextPowerOfTwoSize(reserved.GetByteSize())});
}

bool Allocation::Reserve(Bytes reserved) {
  if (reserved <= reserved_) {
    return true;
  }

  auto new_allocation = ::realloc(buffer_, reserved.GetByteSize());
  if (!new_allocation) {
    // If new length is zero, a minimum non-zero sized allocation is returned.
    // So this check will not trip and this routine will indicate success as
    // expected.
    VALIDATION_LOG << "Allocation failed. Out of host memory.";
    return false;
  }

  buffer_ = static_cast<uint8_t*>(new_allocation);
  reserved_ = reserved;

  return true;
}

std::shared_ptr<fml::Mapping> CreateMappingWithCopy(const uint8_t* contents,
                                                    Bytes length) {
  if (contents == nullptr) {
    return nullptr;
  }

  auto allocation = std::make_shared<Allocation>();
  if (!allocation->Truncate(length)) {
    return nullptr;
  }

  std::memmove(allocation->GetBuffer(), contents, length.GetByteSize());

  return CreateMappingFromAllocation(allocation);
}

std::shared_ptr<fml::Mapping> CreateMappingFromAllocation(
    const std::shared_ptr<Allocation>& allocation) {
  if (!allocation) {
    return nullptr;
  }
  return std::make_shared<fml::NonOwnedMapping>(
      reinterpret_cast<const uint8_t*>(allocation->GetBuffer()),  //
      allocation->GetLength().GetByteSize(),                      //
      [allocation](auto, auto) {}                                 //
  );
}

std::shared_ptr<fml::Mapping> CreateMappingWithString(std::string string) {
  auto buffer = std::make_shared<std::string>(std::move(string));
  return std::make_unique<fml::NonOwnedMapping>(
      reinterpret_cast<const uint8_t*>(buffer->c_str()), buffer->length(),
      [buffer](auto, auto) {});
}

}  // namespace impeller
