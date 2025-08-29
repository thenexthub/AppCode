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

#ifndef APPCODE_TOOLS_FONT_SUBSET_HB_WRAPPERS_H_
#define APPCODE_TOOLS_FONT_SUBSET_HB_WRAPPERS_H_

#include <hb-subset.h>

#include <memory>

namespace HarfbuzzWrappers {
struct hb_blob_deleter {
  void operator()(hb_blob_t* ptr) { hb_blob_destroy(ptr); }
};

struct hb_face_deleter {
  void operator()(hb_face_t* ptr) { hb_face_destroy(ptr); }
};

struct hb_subset_input_deleter {
  void operator()(hb_subset_input_t* ptr) { hb_subset_input_destroy(ptr); }
};

struct hb_set_deleter {
  void operator()(hb_set_t* ptr) { hb_set_destroy(ptr); }
};

using HbBlobPtr = std::unique_ptr<hb_blob_t, hb_blob_deleter>;
using HbFacePtr = std::unique_ptr<hb_face_t, hb_face_deleter>;
using HbSubsetInputPtr =
    std::unique_ptr<hb_subset_input_t, hb_subset_input_deleter>;
using HbSetPtr = std::unique_ptr<hb_set_t, hb_set_deleter>;

};  // namespace HarfbuzzWrappers

#endif  // APPCODE_TOOLS_FONT_SUBSET_HB_WRAPPERS_H_
