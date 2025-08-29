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

#ifndef APPCODE_SHELL_PLATFORM_COMMON_TEXT_EDITING_DELTA_H_
#define APPCODE_SHELL_PLATFORM_COMMON_TEXT_EDITING_DELTA_H_

#include <string>

#include "appcode/fml/string_conversion.h"
#include "appcode/shell/platform/common/text_range.h"

namespace appcode {

/// A change in the state of an input field.
struct TextEditingDelta {
  TextEditingDelta(const std::u16string& text_before_change,
                   const TextRange& range,
                   const std::u16string& text);

  TextEditingDelta(const std::string& text_before_change,
                   const TextRange& range,
                   const std::string& text);

  explicit TextEditingDelta(const std::u16string& text);

  explicit TextEditingDelta(const std::string& text);

  virtual ~TextEditingDelta() = default;

  /// Get the old_text_ value.
  ///
  /// All strings are stored as UTF16 but converted to UTF8 when accessed.
  std::string old_text() const { return fml::Utf16ToUtf8(old_text_); }

  /// Get the delta_text value.
  ///
  /// All strings are stored as UTF16 but converted to UTF8 when accessed.
  std::string delta_text() const { return fml::Utf16ToUtf8(delta_text_); }

  /// Get the delta_start_ value.
  int delta_start() const { return delta_start_; }

  /// Get the delta_end_ value.
  int delta_end() const { return delta_end_; }

  bool operator==(const TextEditingDelta& rhs) const {
    return old_text_ == rhs.old_text_ && delta_text_ == rhs.delta_text_ &&
           delta_start_ == rhs.delta_start_ && delta_end_ == rhs.delta_end_;
  }

  bool operator!=(const TextEditingDelta& rhs) const { return !(*this == rhs); }

  TextEditingDelta(const TextEditingDelta& other) = default;

  TextEditingDelta& operator=(const TextEditingDelta& other) = default;

 private:
  std::u16string old_text_;
  std::u16string delta_text_;
  int delta_start_;
  int delta_end_;

  void set_old_text(const std::u16string& old_text) { old_text_ = old_text; }

  void set_delta_text(const std::u16string& delta_text) {
    delta_text_ = delta_text;
  }

  void set_delta_start(int delta_start) { delta_start_ = delta_start; }

  void set_delta_end(int delta_end) { delta_end_ = delta_end; }
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_COMMON_TEXT_EDITING_DELTA_H_
