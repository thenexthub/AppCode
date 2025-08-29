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

#include "impeller/renderer/backend/metal/swapchain_transients_mtl.h"
#include "impeller/base/validation.h"
#include "impeller/core/formats.h"
#include "impeller/core/texture_descriptor.h"

namespace impeller {

SwapchainTransientsMTL::SwapchainTransientsMTL(
    const std::shared_ptr<Allocator>& allocator)
    : allocator_(allocator) {}

SwapchainTransientsMTL::~SwapchainTransientsMTL() = default;

void SwapchainTransientsMTL::SetSizeAndFormat(ISize size, PixelFormat format) {
  if (size != size_ || format != format_) {
    resolve_tex_ = nullptr;
    msaa_tex_ = nullptr;
    depth_stencil_tex_ = nullptr;
  }
  size_ = size;
  format_ = format;
}

std::shared_ptr<Texture> SwapchainTransientsMTL::GetResolveTexture() {
  if (!resolve_tex_) {
    TextureDescriptor desc;
    desc.size = size_;
    desc.sample_count = SampleCount::kCount1;
    desc.format = format_;
    desc.storage_mode = StorageMode::kDevicePrivate;
    desc.usage = TextureUsage::kShaderRead | TextureUsage::kRenderTarget;
    desc.compression_type = CompressionType::kLossy;
    desc.type = TextureType::kTexture2D;

    resolve_tex_ = allocator_->CreateTexture(desc);
    if (!resolve_tex_) {
      VALIDATION_LOG << "Failed to allocate resolve texture.";
      return nullptr;
    }
    resolve_tex_->SetLabel("ImpellerOnscreenResolve");
  }

  return resolve_tex_;
}

std::shared_ptr<Texture> SwapchainTransientsMTL::GetMSAATexture() {
  if (!msaa_tex_) {
    TextureDescriptor desc;
    desc.size = size_;
    desc.sample_count = SampleCount::kCount4;
    desc.format = format_;
    desc.storage_mode = StorageMode::kDeviceTransient;
    desc.usage = TextureUsage::kRenderTarget;
    desc.type = TextureType::kTexture2DMultisample;

    msaa_tex_ = allocator_->CreateTexture(desc);
    if (!msaa_tex_) {
      VALIDATION_LOG << "Failed to allocate MSAA texture.";
      return nullptr;
    }
    msaa_tex_->SetLabel("ImpellerOnscreenMSAA");
  }

  return msaa_tex_;
}

std::shared_ptr<Texture> SwapchainTransientsMTL::GetDepthStencilTexture() {
  if (!depth_stencil_tex_) {
    TextureDescriptor desc;
    desc.size = size_;
    desc.sample_count = SampleCount::kCount4;
    desc.format = PixelFormat::kD32FloatS8UInt;
    desc.storage_mode = StorageMode::kDeviceTransient;
    desc.usage = TextureUsage::kRenderTarget;
    desc.type = TextureType::kTexture2DMultisample;

    depth_stencil_tex_ = allocator_->CreateTexture(desc);
    if (!depth_stencil_tex_) {
      VALIDATION_LOG << "Failed to allocate depth-stencil texture.";
      return nullptr;
    }
    depth_stencil_tex_->SetLabel("ImpellerOnscreenDepth+Stencil");
  }

  return depth_stencil_tex_;
}

}  // namespace impeller
