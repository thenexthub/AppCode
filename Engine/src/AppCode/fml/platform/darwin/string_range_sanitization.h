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

#ifndef APPCODE_FML_PLATFORM_DARWIN_STRING_RANGE_SANITIZATION_H_
#define APPCODE_FML_PLATFORM_DARWIN_STRING_RANGE_SANITIZATION_H_

#include <Foundation/Foundation.h>

namespace fml {

// Returns a range encompassing the grapheme cluster in which |index| is located.
//
// A nil |text| or an index greater than or equal to text.length will result in
// `NSRange(NSNotFound, 0)`.
NSRange RangeForCharacterAtIndex(NSString* text, NSUInteger index);

// Returns a range encompassing the grapheme clusters falling in |range|.
//
// This method will not alter the length of the input range, but will ensure
// that the range's location is not in the middle of a multi-byte unicode
// sequence.
//
// An invalid range will result in `NSRange(NSNotFound, 0)`.
NSRange RangeForCharactersInRange(NSString* text, NSRange range);

}  // namespace fml

#endif  // APPCODE_FML_PLATFORM_DARWIN_STRING_RANGE_SANITIZATION_H_
