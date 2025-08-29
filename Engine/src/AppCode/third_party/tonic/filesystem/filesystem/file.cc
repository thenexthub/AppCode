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

#include "tonic/filesystem/filesystem/file.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <climits>
#include <cstdint>

#include "tonic/common/build_config.h"

#if defined(OS_WIN)
#define BINARY_MODE _O_BINARY
#else
#define BINARY_MODE 0
#endif

#if defined(OS_WIN)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include "tonic/filesystem/filesystem/eintr_wrapper.h"
#include "tonic/filesystem/filesystem/portable_unistd.h"

namespace filesystem {
namespace {

template <typename T>
bool ReadFileDescriptor(int fd, T* result) {
  if (!result) {
    return false;
  }

  result->clear();

  if (fd < 0)
    return false;

  constexpr size_t kBufferSize = 1 << 16;
  size_t offset = 0;
  ssize_t bytes_read = 0;
  do {
    offset += bytes_read;
    result->resize(offset + kBufferSize);
    bytes_read = HANDLE_EINTR(read(fd, &(*result)[offset], kBufferSize));
  } while (bytes_read > 0);

  if (bytes_read < 0) {
    result->clear();
    return false;
  }

  result->resize(offset + bytes_read);
  return true;
}

}  // namespace

std::pair<uint8_t*, intptr_t> ReadFileDescriptorToBytes(int fd) {
  std::pair<uint8_t*, intptr_t> failure_pair{nullptr, -1};
  struct stat st;
  if (fstat(fd, &st) != 0) {
    return failure_pair;
  }
  intptr_t file_size = st.st_size;
  uint8_t* ptr = (uint8_t*)malloc(file_size);

  size_t bytes_left = file_size;
  size_t offset = 0;
  while (bytes_left > 0) {
    ssize_t bytes_read = HANDLE_EINTR(read(fd, &ptr[offset], bytes_left));
    if (bytes_read < 0) {
      return failure_pair;
    }
    offset += bytes_read;
    bytes_left -= bytes_read;
  }
  return std::pair<uint8_t*, intptr_t>(ptr, file_size);
}

bool ReadFileToString(const std::string& path, std::string* result) {
  Descriptor fd(open(path.c_str(), O_RDONLY));
  return ReadFileDescriptor(fd.get(), result);
}

bool ReadFileDescriptorToString(int fd, std::string* result) {
  return ReadFileDescriptor(fd, result);
}

std::pair<uint8_t*, intptr_t> ReadFileToBytes(const std::string& path) {
  std::pair<uint8_t*, intptr_t> failure_pair{nullptr, -1};
  Descriptor fd(open(path.c_str(), O_RDONLY | BINARY_MODE));
  if (!fd.is_valid())
    return failure_pair;
  return ReadFileDescriptorToBytes(fd.get());
}

}  // namespace filesystem
