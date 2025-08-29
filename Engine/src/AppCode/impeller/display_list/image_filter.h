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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_IMAGE_FILTER_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_IMAGE_FILTER_H_

#include "display_list/effects/dl_image_filter.h"
#include "impeller/entity/contents/filters/filter_contents.h"

namespace impeller {

/// @brief  Generate a new FilterContents using this filter's configuration.
///
std::shared_ptr<FilterContents> WrapInput(const appcode::DlImageFilter* filter,
                                          const FilterInput::Ref& input);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_IMAGE_FILTER_H_
