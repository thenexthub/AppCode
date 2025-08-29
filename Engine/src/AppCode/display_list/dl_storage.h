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

#ifndef APPCODE_DISPLAY_LIST_DL_STORAGE_H_
#define APPCODE_DISPLAY_LIST_DL_STORAGE_H_

#include <memory>

#include "appcode/fml/logging.h"

namespace appcode {

// Manages a buffer allocated with malloc.
class DisplayListStorage {
 public:
  static const constexpr size_t kDLPageSize = 4096u;

  DisplayListStorage() = default;
  DisplayListStorage(DisplayListStorage&&);

  /// Returns a pointer to the base of the storage.
  uint8_t* base() { return ptr_.get(); }
  const uint8_t* base() const { return ptr_.get(); }

  /// Returns the currently allocated size
  size_t size() const { return used_; }

  /// Returns the maximum currently allocated space
  size_t capacity() const { return allocated_; }

  /// Ensures the indicated number of bytes are available and returns
  /// a pointer to that memory within the storage while also invalidating
  /// any other outstanding pointers into the storage.
  uint8_t* allocate(size_t needed);

  /// Trims the storage to the currently allocated size and invalidates
  /// any outstanding pointers into the storage.
  void trim() { realloc(used_); }

  /// Resets the storage and allocation of the object to an empty state
  void reset();

  DisplayListStorage& operator=(DisplayListStorage&& other);

 private:
  void realloc(size_t count);

  struct FreeDeleter {
    void operator()(uint8_t* p) { std::free(p); }
  };
  std::unique_ptr<uint8_t, FreeDeleter> ptr_;

  size_t used_ = 0u;
  size_t allocated_ = 0u;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_DL_STORAGE_H_
