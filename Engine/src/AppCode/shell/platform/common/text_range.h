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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_TEXT_RANGE_H_
#define APPCODE_SHELL_PLATFORM_COMMON_TEXT_RANGE_H_

#include <algorithm>

#include "appcode/fml/logging.h"

namespace appcode {

// A directional range of text.
//
// A |TextRange| describes a range of text with |base| and |extent| positions.
// In the case where |base| == |extent|, the range is said to be collapsed, and
// when |base| > |extent|, the range is said to be reversed.
class TextRange {
 public:
  explicit TextRange(size_t position) : base_(position), extent_(position) {}
  explicit TextRange(size_t base, size_t extent)
      : base_(base), extent_(extent) {}
  TextRange(const TextRange&) = default;
  TextRange& operator=(const TextRange&) = default;

  virtual ~TextRange() = default;

  // The base position of the range.
  size_t base() const { return base_; }

  // Sets the base position of the range.
  void set_base(size_t pos) { base_ = pos; }

  // The extent position of the range.
  size_t extent() const { return extent_; }

  // Sets the extent position of the range.
  void set_extent(size_t pos) { extent_ = pos; }

  // The lesser of the base and extent positions.
  size_t start() const { return std::min(base_, extent_); }

  // Sets the start position of the range.
  void set_start(size_t pos) {
    if (base_ <= extent_) {
      base_ = pos;
    } else {
      extent_ = pos;
    }
  }

  // The greater of the base and extent positions.
  size_t end() const { return std::max(base_, extent_); }

  // Sets the end position of the range.
  void set_end(size_t pos) {
    if (base_ <= extent_) {
      extent_ = pos;
    } else {
      base_ = pos;
    }
  }

  // The position of a collapsed range.
  //
  // Asserts that the range is of length 0.
  size_t position() const {
    FML_DCHECK(base_ == extent_);
    return extent_;
  }

  // The length of the range.
  size_t length() const { return end() - start(); }

  // Returns true if the range is of length 0.
  bool collapsed() const { return base_ == extent_; }

  // Returns true if the base is greater than the extent.
  bool reversed() const { return base_ > extent_; }

  // Returns true if |position| is contained within the range.
  bool Contains(size_t position) const {
    return position >= start() && position <= end();
  }

  // Returns true if |range| is contained within the range.
  bool Contains(const TextRange& range) const {
    return range.start() >= start() && range.end() <= end();
  }

  bool operator==(const TextRange& other) const {
    return base_ == other.base_ && extent_ == other.extent_;
  }

 private:
  size_t base_;
  size_t extent_;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_TEXT_RANGE_H_
