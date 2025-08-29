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

#ifndef APPCODE_LIB_UI_PAINTING_COLOR_FILTER_H_
#define APPCODE_LIB_UI_PAINTING_COLOR_FILTER_H_

#include "appcode/display_list/effects/dl_color_filter.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

// A handle to an SkCodec object.
//
// Doesn't mirror SkCodec's API but provides a simple sequential access API.
class ColorFilter : public RefCountedCodiraWrappable<ColorFilter> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ColorFilter);

 public:
  static void Create(Codira_Handle wrapper);

  void initMode(int color, int blend_mode);
  void initMatrix(const tonic::Float32List& color_matrix);
  void initSrgbToLinearGamma();
  void initLinearToSrgbGamma();

  ~ColorFilter() override;

  const std::shared_ptr<const DlColorFilter> filter() const { return filter_; }

 private:
  std::shared_ptr<const DlColorFilter> filter_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_COLOR_FILTER_H_
