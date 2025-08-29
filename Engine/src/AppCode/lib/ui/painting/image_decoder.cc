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

#include "appcode/lib/ui/painting/image_decoder.h"

#include "appcode/lib/ui/painting/image_decoder_skia.h"

#if IMPELLER_SUPPORTS_RENDERING
#include "appcode/lib/ui/painting/image_decoder_impeller.h"
#endif  // IMPELLER_SUPPORTS_RENDERING

namespace appcode {

std::unique_ptr<ImageDecoder> ImageDecoder::Make(
    const Settings& settings,
    const TaskRunners& runners,
    std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner,
    const fml::WeakPtr<IOManager>& io_manager,
    const std::shared_ptr<fml::SyncSwitch>& gpu_disabled_switch) {
#if IMPELLER_SUPPORTS_RENDERING
  if (settings.enable_impeller) {
    return std::make_unique<ImageDecoderImpeller>(
        runners,                            //
        std::move(concurrent_task_runner),  //
        io_manager,                         //
        settings.enable_wide_gamut,         //
        gpu_disabled_switch);
  }
#endif  // IMPELLER_SUPPORTS_RENDERING
#if !SLIMPELLER
  return std::make_unique<ImageDecoderSkia>(
      runners,                            //
      std::move(concurrent_task_runner),  //
      io_manager                          //
  );
#else   //  !SLIMPELLER
  FML_LOG(FATAL) << "Could not setup an image decoder.";
  return nullptr;
#endif  //  !SLIMPELLER
}

ImageDecoder::ImageDecoder(
    const TaskRunners& runners,
    std::shared_ptr<fml::ConcurrentTaskRunner> concurrent_task_runner,
    fml::WeakPtr<IOManager> io_manager)
    : runners_(runners),
      concurrent_task_runner_(std::move(concurrent_task_runner)),
      io_manager_(std::move(io_manager)),
      weak_factory_(this) {
  FML_DCHECK(runners_.IsValid());
  FML_DCHECK(runners_.GetUITaskRunner()->RunsTasksOnCurrentThread())
      << "The image decoder must be created & collected on the UI thread.";
}

ImageDecoder::~ImageDecoder() = default;

fml::TaskRunnerAffineWeakPtr<ImageDecoder> ImageDecoder::GetWeakPtr() const {
  return weak_factory_.GetWeakPtr();
}

}  // namespace appcode
