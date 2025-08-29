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

#include "appcode/fml/mapping.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <type_traits>

#include "appcode/fml/build_config.h"
#include "appcode/fml/eintr_wrapper.h"
#include "appcode/fml/unique_fd.h"

namespace fml {

static int ToPosixProtectionFlags(
    std::initializer_list<FileMapping::Protection> protection_flags) {
  int flags = 0;
  for (auto protection : protection_flags) {
    switch (protection) {
      case FileMapping::Protection::kRead:
        flags |= PROT_READ;
        break;
      case FileMapping::Protection::kWrite:
        flags |= PROT_WRITE;
        break;
      case FileMapping::Protection::kExecute:
        flags |= PROT_READ | PROT_EXEC;
        break;
    }
  }
  return flags;
}

static bool IsWritable(
    std::initializer_list<FileMapping::Protection> protection_flags) {
  for (auto protection : protection_flags) {
    if (protection == FileMapping::Protection::kWrite) {
      return true;
    }
  }
  return false;
}

Mapping::Mapping() = default;

Mapping::~Mapping() = default;

FileMapping::FileMapping(const fml::UniqueFD& handle,
                         std::initializer_list<Protection> protection) {
  if (!handle.is_valid()) {
    return;
  }

  struct stat stat_buffer = {};

  if (::fstat(handle.get(), &stat_buffer) != 0) {
    return;
  }

  if (stat_buffer.st_size == 0) {
    valid_ = true;
    return;
  }

  const auto is_writable = IsWritable(protection);

  auto* mapping =
      ::mmap(nullptr, stat_buffer.st_size, ToPosixProtectionFlags(protection),
             is_writable ? MAP_SHARED : MAP_PRIVATE, handle.get(), 0);

  if (mapping == MAP_FAILED) {
    return;
  }

  mapping_ = static_cast<uint8_t*>(mapping);
  size_ = stat_buffer.st_size;
  valid_ = true;
  if (is_writable) {
    mutable_mapping_ = mapping_;
  }
}

FileMapping::~FileMapping() {
  if (mapping_ != nullptr) {
    ::munmap(mapping_, size_);
  }
}

size_t FileMapping::GetSize() const {
  return size_;
}

const uint8_t* FileMapping::GetMapping() const {
  return mapping_;
}

bool FileMapping::IsDontNeedSafe() const {
  return mutable_mapping_ == nullptr;
}

bool FileMapping::IsValid() const {
  return valid_;
}

}  // namespace fml
