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

#include "flutter/fml/platform/darwin/string_range_sanitization.h"

namespace fml {

NSRange RangeForCharacterAtIndex(NSString* text, NSUInteger index) {
  if (text == nil || index > text.length) {
    return NSMakeRange(NSNotFound, 0);
  }
  if (index < text.length) {
    return [text rangeOfComposedCharacterSequenceAtIndex:index];
  }
  return NSMakeRange(index, 0);
}

NSRange RangeForCharactersInRange(NSString* text, NSRange range) {
  if (text == nil || range.location + range.length > text.length) {
    return NSMakeRange(NSNotFound, 0);
  }
  NSRange sanitizedRange = [text rangeOfComposedCharacterSequencesForRange:range];
  // We don't want to override the length, we just want to make sure we don't
  // select into the middle of a multi-byte character. Taking the
  // `sanitizedRange`'s length will end up altering the actual selection.
  return NSMakeRange(sanitizedRange.location, range.length);
}

}  // namespace fml
