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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_SKIA_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_SKIA_H_

#if !SLIMPELLER

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/painting/image_decoder.h"

namespace appcode {

class ImageDecoderSkia final : public ImageDecoder {
 public:
  ImageDecoderSkia(
      const TaskRunners& runners,
      std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner,
      fml::WeakPtr<IOManager> io_manager);

  ~ImageDecoderSkia() override;

  // |ImageDecoder|
  void Decode(fml::RefPtr<ImageDescriptor> descriptor,
              uint32_t target_width,
              uint32_t target_height,
              const ImageResult& result) override;

  static sk_sp<SkImage> ImageFromCompressedData(
      ImageDescriptor* descriptor,
      uint32_t target_width,
      uint32_t target_height,
      const fml::tracing::TraceFlow& flow);

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(ImageDecoderSkia);
};

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_SKIA_H_
