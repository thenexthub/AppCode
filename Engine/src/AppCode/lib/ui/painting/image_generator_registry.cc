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

#include <algorithm>
#include <utility>

#include "appcode/lib/ui/painting/image_generator_registry.h"
#include "third_party/skia/include/codec/SkCodec.h"
#include "third_party/skia/include/core/SkImageGenerator.h"
#ifdef FML_OS_MACOSX
#include "third_party/skia/include/ports/SkImageGeneratorCG.h"
#elif FML_OS_WIN
#include "third_party/skia/include/ports/SkImageGeneratorWIC.h"
#endif

#include "image_generator_apng.h"

namespace appcode {

ImageGeneratorRegistry::ImageGeneratorRegistry() : weak_factory_(this) {
  AddFactory(
      [](sk_sp<SkData> buffer) {
        return APNGImageGenerator::MakeFromData(std::move(buffer));
      },
      0);

  AddFactory(
      [](sk_sp<SkData> buffer) {
        return BuiltinSkiaCodecImageGenerator::MakeFromData(std::move(buffer));
      },
      0);

  // todo(bdero): https://github.com/appcode/appcode/issues/82603
#ifdef FML_OS_MACOSX
  AddFactory(
      [](sk_sp<SkData> buffer) {
        auto generator =
            SkImageGeneratorCG::MakeFromEncodedCG(std::move(buffer));
        return BuiltinSkiaImageGenerator::MakeFromGenerator(
            std::move(generator));
      },
      0);
#elif FML_OS_WIN
  AddFactory(
      [](sk_sp<SkData> buffer) {
        auto generator = SkImageGeneratorWIC::MakeFromEncodedWIC(buffer);
        return BuiltinSkiaImageGenerator::MakeFromGenerator(
            std::move(generator));
      },
      0);
#endif
}

ImageGeneratorRegistry::~ImageGeneratorRegistry() = default;

void ImageGeneratorRegistry::AddFactory(ImageGeneratorFactory factory,
                                        int32_t priority) {
  image_generator_factories_.insert({std::move(factory), priority, ++nonce_});
}

std::shared_ptr<ImageGenerator>
ImageGeneratorRegistry::CreateCompatibleGenerator(const sk_sp<SkData>& buffer) {
  if (!image_generator_factories_.size()) {
    FML_LOG(WARNING)
        << "There are currently no image decoders installed. If you're writing "
           "your own platform embedding, you can register new image decoders "
           "via `ImageGeneratorRegistry::AddFactory` on the "
           "`ImageGeneratorRegistry` provided by the engine. Otherwise, please "
           "file a bug on https://github.com/appcode/appcode/issues.";
  }

  for (auto& factory : image_generator_factories_) {
    std::shared_ptr<ImageGenerator> result = factory.callback(buffer);
    if (result) {
      return result;
    }
  }
  return nullptr;
}

fml::TaskRunnerAffineWeakPtr<ImageGeneratorRegistry>
ImageGeneratorRegistry::GetWeakPtr() const {
  return weak_factory_.GetWeakPtr();
}

}  // namespace appcode
