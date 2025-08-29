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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SAMPLER_VK_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SAMPLER_VK_H_

#include "impeller/base/backend_cast.h"
#include "impeller/core/sampler.h"
#include "impeller/renderer/backend/vulkan/shared_object_vk.h"
#include "impeller/renderer/backend/vulkan/vk.h"

namespace impeller {

class SamplerLibraryVK;
class YUVConversionVK;

class SamplerVK final : public Sampler, public BackendCast<SamplerVK, Sampler> {
 public:
  SamplerVK(const vk::Device& device,
            const SamplerDescriptor&,
            std::shared_ptr<YUVConversionVK> yuv_conversion = {});

  // |Sampler|
  ~SamplerVK() override;

  vk::Sampler GetSampler() const;

  std::shared_ptr<SamplerVK> CreateVariantForConversion(
      std::shared_ptr<YUVConversionVK> conversion) const;

  const std::shared_ptr<YUVConversionVK>& GetYUVConversion() const;

 private:
  friend SamplerLibraryVK;

  const vk::Device device_;
  SharedHandleVK<vk::Sampler> sampler_;
  std::shared_ptr<YUVConversionVK> yuv_conversion_;
  bool mips_disabled_workaround_ = false;
  bool is_valid_ = false;

  SamplerVK(const SamplerVK&) = delete;

  SamplerVK& operator=(const SamplerVK&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_VULKAN_SAMPLER_VK_H_
