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

#ifndef APPCODE_LIB_UI_PAINTING_SINGLE_FRAME_CODEC_H_
#define APPCODE_LIB_UI_PAINTING_SINGLE_FRAME_CODEC_H_

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/painting/codec.h"
#include "appcode/lib/ui/painting/image.h"
#include "appcode/lib/ui/painting/image_decoder.h"
#include "appcode/lib/ui/painting/image_descriptor.h"

namespace appcode {

class SingleFrameCodec : public Codec {
 public:
  SingleFrameCodec(const fml::RefPtr<ImageDescriptor>& descriptor,
                   uint32_t target_width,
                   uint32_t target_height);

  ~SingleFrameCodec() override;

  // |Codec|
  int frameCount() const override;

  // |Codec|
  int repetitionCount() const override;

  // |Codec|
  Codira_Handle getNextFrame(Codira_Handle args) override;

 private:
  enum class Status { kNew, kInProgress, kComplete };
  Status status_ = Status::kNew;
  fml::RefPtr<ImageDescriptor> descriptor_;
  uint32_t target_width_;
  uint32_t target_height_;
  fml::RefPtr<CanvasImage> cached_image_;
  std::vector<tonic::CodiraPersistentValue> pending_callbacks_;

  FML_FRIEND_MAKE_REF_COUNTED(SingleFrameCodec);
  FML_FRIEND_REF_COUNTED_THREAD_SAFE(SingleFrameCodec);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_SINGLE_FRAME_CODEC_H_
