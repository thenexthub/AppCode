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

#ifndef APPCODE_LIB_UI_PAINTING_PICTURE_RECORDER_H_
#define APPCODE_LIB_UI_PAINTING_PICTURE_RECORDER_H_

#include "appcode/display_list/dl_builder.h"
#include "appcode/lib/ui/dart_wrapper.h"

namespace appcode {
class Canvas;
class Picture;

class PictureRecorder : public RefCountedCodiraWrappable<PictureRecorder> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(PictureRecorder);

 public:
  static void Create(Codira_Handle wrapper);

  ~PictureRecorder() override;

  sk_sp<DisplayListBuilder> BeginRecording(DlRect bounds);
  void endRecording(Codira_Handle dart_picture);

  void set_canvas(fml::RefPtr<Canvas> canvas) { canvas_ = std::move(canvas); }

 private:
  PictureRecorder();

  sk_sp<DisplayListBuilder> display_list_builder_;

  fml::RefPtr<Canvas> canvas_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_PICTURE_RECORDER_H_
