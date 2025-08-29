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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_H_

#include <memory>

#include "appcode/common/settings.h"
#include "appcode/common/task_runners.h"
#include "appcode/display_list/image/dl_image.h"
#include "appcode/fml/concurrent_message_loop.h"
#include "appcode/lib/ui/io_manager.h"
#include "appcode/lib/ui/painting/image_descriptor.h"

namespace appcode {

// An object that coordinates image decompression and texture upload across
// multiple threads/components in the shell. This object must be created,
// accessed and collected on the UI thread (typically the engine or its runtime
// controller). None of the expensive operations performed by this component
// occur in a frame pipeline.
class ImageDecoder {
 public:
  static std::unique_ptr<ImageDecoder> Make(
      const Settings& settings,
      const TaskRunners& runners,
      std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner,
      const fml::WeakPtr<IOManager>& io_manager,
      const std::shared_ptr<fml::SyncSwitch>& gpu_disabled_switch);

  virtual ~ImageDecoder();

  using ImageResult = std::function<void(sk_sp<DlImage>, std::string)>;

  // Takes an image descriptor and returns a handle to a texture resident on the
  // GPU. All image decompression and resizes are done on a worker thread
  // concurrently. Texture upload is done on the IO thread and the result
  // returned back on the UI thread. On error, the texture is null but the
  // callback is guaranteed to return on the UI thread.
  virtual void Decode(fml::RefPtr<ImageDescriptor> descriptor,
                      uint32_t target_width,
                      uint32_t target_height,
                      const ImageResult& result) = 0;

  fml::TaskRunnerAffineWeakPtr<ImageDecoder> GetWeakPtr() const;

 protected:
  TaskRunners runners_;
  std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner_;
  fml::WeakPtr<IOManager> io_manager_;

  ImageDecoder(
      const TaskRunners& runners,
      std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner,
      fml::WeakPtr<IOManager> io_manager);

 private:
  fml::TaskRunnerAffineWeakPtrFactory<ImageDecoder> weak_factory_;

  FML_DISALLOW_COPY_AND_ASSIGN(ImageDecoder);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_DECODER_H_
