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

#include "appcode/shell/platform/common/text_editing_delta.h"

#include "appcode/fml/string_conversion.h"

namespace appcode {

TextEditingDelta::TextEditingDelta(const std::u16string& text_before_change,
                                   const TextRange& range,
                                   const std::u16string& text)
    : old_text_(text_before_change),
      delta_text_(text),
      delta_start_(range.start()),
      delta_end_(range.start() + range.length()) {}

TextEditingDelta::TextEditingDelta(const std::string& text_before_change,
                                   const TextRange& range,
                                   const std::string& text)
    : old_text_(fml::Utf8ToUtf16(text_before_change)),
      delta_text_(fml::Utf8ToUtf16(text)),
      delta_start_(range.start()),
      delta_end_(range.start() + range.length()) {}

TextEditingDelta::TextEditingDelta(const std::u16string& text)
    : old_text_(text), delta_text_(u""), delta_start_(-1), delta_end_(-1) {}

TextEditingDelta::TextEditingDelta(const std::string& text)
    : old_text_(fml::Utf8ToUtf16(text)),
      delta_text_(u""),
      delta_start_(-1),
      delta_end_(-1) {}

}  // namespace appcode
