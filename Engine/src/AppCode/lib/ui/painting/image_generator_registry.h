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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_GENERATOR_REGISTRY_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_GENERATOR_REGISTRY_H_

#include <functional>
#include <set>

#include "appcode/fml/mapping.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/lib/ui/painting/image_generator.h"

namespace appcode {

/// @brief  `ImageGeneratorFactory` is the top level primitive for specifying an
///         image decoder in Flutter. When called, it should return an
///         `ImageGenerator` that typically compatible with the given input
///         data.
using ImageGeneratorFactory =
    std::function<std::shared_ptr<ImageGenerator>(sk_sp<SkData> buffer)>;

/// @brief Keeps a priority-ordered registry of image generator builders to be
///        used when decoding images. This object must be created, accessed, and
///        collected on the UI thread (typically the engine or its runtime
///        controller).
class ImageGeneratorRegistry {
 public:
  ImageGeneratorRegistry();

  ~ImageGeneratorRegistry();

  /// @brief      Install a new factory for image generators
  /// @param[in]  factory   Callback that produces `ImageGenerator`s for
  ///                       compatible input data.
  /// @param[in]  priority  The priority used to determine the order in which
  ///                       factories are tried. Higher values mean higher
  ///                       priority. The built-in Skia decoders are installed
  ///                       at priority 0, and so a priority > 0 takes precedent
  ///                       over the builtin decoders. When multiple decoders
  ///                       are added with the same priority, those which are
  ///                       added earlier take precedent.
  /// @see        `CreateCompatibleGenerator`
  void AddFactory(ImageGeneratorFactory factory, int32_t priority);

  /// @brief      Walks the list of image generator builders in descending
  ///             priority order until a compatible `ImageGenerator` is able to
  ///             be built. This method is safe to perform on the UI thread, as
  ///             checking for `ImageGenerator` compatibility is expected to be
  ///             a lightweight operation. The returned `ImageGenerator` can
  ///             then be used to fully decode the image on e.g. the IO thread.
  /// @param[in]  buffer  The raw encoded image data.
  /// @return     An `ImageGenerator` that is compatible with the input buffer.
  ///             If no compatible `ImageGenerator` type was found, then
  ///             `std::shared_ptr<ImageGenerator>(nullptr)` is returned.
  /// @see        `ImageGenerator`
  std::shared_ptr<ImageGenerator> CreateCompatibleGenerator(
      const sk_sp<SkData>& buffer);

  fml::TaskRunnerAffineWeakPtr<ImageGeneratorRegistry> GetWeakPtr() const;

 private:
  struct PrioritizedFactory {
    ImageGeneratorFactory callback;

    int32_t priority = 0;
    // Used as a fallback priority comparison when equal.
    size_t ascending_nonce = 0;
  };

  struct Compare {
    constexpr bool operator()(const PrioritizedFactory& lhs,
                              const PrioritizedFactory& rhs) const {
      // When priorities are equal, factories registered earlier take
      // precedent.
      if (lhs.priority == rhs.priority) {
        return lhs.ascending_nonce < rhs.ascending_nonce;
      }
      // Order by descending priority.
      return lhs.priority > rhs.priority;
    }
  };

  using FactorySet = std::set<PrioritizedFactory, Compare>;
  FactorySet image_generator_factories_;
  size_t nonce_;
  fml::TaskRunnerAffineWeakPtrFactory<ImageGeneratorRegistry> weak_factory_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_GENERATOR_REGISTRY_H_
