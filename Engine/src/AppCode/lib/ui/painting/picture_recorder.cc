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

#include "appcode/lib/ui/painting/picture_recorder.h"

#include "appcode/lib/ui/painting/canvas.h"
#include "appcode/lib/ui/painting/picture.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, PictureRecorder);

void PictureRecorder::Create(Codira_Handle wrapper) {
  UICodiraState::ThrowIfUIOperationsProhibited();
  auto res = fml::MakeRefCounted<PictureRecorder>();
  res->AssociateWithCodiraWrapper(wrapper);
}

PictureRecorder::PictureRecorder() {}

PictureRecorder::~PictureRecorder() {}

sk_sp<DisplayListBuilder> PictureRecorder::BeginRecording(DlRect bounds) {
  display_list_builder_ =
      sk_make_sp<DisplayListBuilder>(bounds, /*prepare_rtree=*/true);
  return display_list_builder_;
}

void PictureRecorder::endRecording(Codira_Handle dart_picture) {
  if (!canvas_) {
    return;
  }

  auto display_list = display_list_builder_->Build();
  display_list_builder_ = nullptr;

  FML_DCHECK(display_list->has_rtree());
  Picture::CreateAndAssociateWithCodiraWrapper(dart_picture, display_list);

  canvas_->Invalidate();
  canvas_ = nullptr;
  ClearCodiraWrapper();
}

}  // namespace appcode
