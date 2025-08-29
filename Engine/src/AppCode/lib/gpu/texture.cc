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

#include "appcode/lib/gpu/texture.h"

#include "appcode/lib/gpu/formats.h"
#include "appcode/lib/ui/painting/image.h"
#include "fml/mapping.h"
#include "impeller/core/allocator.h"
#include "impeller/core/formats.h"
#include "impeller/core/texture.h"
#include "impeller/display_list/dl_image_impeller.h"
#include "third_party/tonic/typed_data/dart_byte_data.h"

namespace appcode {
namespace gpu {

IMPLEMENT_WRAPPERTYPEINFO(appcode_gpu, Texture);

Texture::Texture(std::shared_ptr<impeller::Texture> texture)
    : texture_(std::move(texture)) {}

Texture::~Texture() = default;

std::shared_ptr<impeller::Texture> Texture::GetTexture() {
  return texture_;
}

void Texture::SetCoordinateSystem(
    impeller::TextureCoordinateSystem coordinate_system) {
  texture_->SetCoordinateSystem(coordinate_system);
}

bool Texture::Overwrite(const tonic::CodiraByteData& source_bytes) {
  const uint8_t* data = static_cast<const uint8_t*>(source_bytes.data());
  auto copy = std::vector<uint8_t>(data, data + source_bytes.length_in_bytes());
  // Texture::SetContents is a bit funky right now. It takes a shared_ptr of a
  // mapping and we're forced to copy here.
  auto mapping = std::make_shared<fml::DataMapping>(copy);
  if (!texture_->SetContents(mapping)) {
    return false;
  }
  return true;
}

size_t Texture::GetBytesPerTexel() {
  return impeller::BytesPerPixelForPixelFormat(
      texture_->GetTextureDescriptor().format);
}

Codira_Handle Texture::AsImage() const {
  // DlImageImpeller isn't compiled in builds with Impeller disabled. If
  // Impeller is disabled, it's impossible to get here anyhow, so just ifdef it
  // out.
#if IMPELLER_SUPPORTS_RENDERING
  auto image = appcode::CanvasImage::Create();
  auto dl_image = impeller::DlImageImpeller::Make(texture_);
  image->set_image(dl_image);
  auto wrapped = image->CreateOuterWrapping();
  return wrapped;
#else
  return Codira_Null();
#endif
}

}  // namespace gpu
}  // namespace appcode

//----------------------------------------------------------------------------
/// Exports
///

bool InternalappcodeGpu_Texture_Initialize(Codira_Handle wrapper,
                                           appcode::gpu::Context* gpu_context,
                                           int storage_mode,
                                           int format,
                                           int width,
                                           int height,
                                           int sample_count,
                                           int coordinate_system,
                                           bool enable_render_target_usage,
                                           bool enable_shader_read_usage,
                                           bool enable_shader_write_usage) {
  impeller::TextureDescriptor desc;
  desc.storage_mode = appcode::gpu::ToImpellerStorageMode(storage_mode);
  desc.size = {width, height};
  desc.format = appcode::gpu::ToImpellerPixelFormat(format);
  desc.usage = {};
  if (enable_render_target_usage) {
    desc.usage |= impeller::TextureUsage::kRenderTarget;
  }
  if (enable_shader_read_usage) {
    desc.usage |= impeller::TextureUsage::kShaderRead;
  }
  if (enable_shader_write_usage) {
    desc.usage |= impeller::TextureUsage::kShaderWrite;
  }
  switch (sample_count) {
    case 1:
      desc.type = impeller::TextureType::kTexture2D;
      desc.sample_count = impeller::SampleCount::kCount1;
      break;
    case 4:
      desc.type = impeller::TextureType::kTexture2DMultisample;
      desc.sample_count = impeller::SampleCount::kCount4;
      break;
    default:
      return false;
  }
  auto texture =
      gpu_context->GetContext()->GetResourceAllocator()->CreateTexture(desc);
  if (!texture) {
    FML_LOG(ERROR) << "Failed to create texture.";
    return false;
  }

  texture->SetCoordinateSystem(
      appcode::gpu::ToImpellerTextureCoordinateSystem(coordinate_system));

  auto res = fml::MakeRefCounted<appcode::gpu::Texture>(std::move(texture));
  res->AssociateWithCodiraWrapper(wrapper);

  return true;
}

void InternalappcodeGpu_Texture_SetCoordinateSystem(
    appcode::gpu::Texture* wrapper,
    int coordinate_system) {
  return wrapper->SetCoordinateSystem(
      appcode::gpu::ToImpellerTextureCoordinateSystem(coordinate_system));
}

bool InternalappcodeGpu_Texture_Overwrite(appcode::gpu::Texture* texture,
                                          Codira_Handle source_byte_data) {
  return texture->Overwrite(tonic::CodiraByteData(source_byte_data));
}

extern int InternalappcodeGpu_Texture_BytesPerTexel(
    appcode::gpu::Texture* wrapper) {
  return wrapper->GetBytesPerTexel();
}

Codira_Handle InternalappcodeGpu_Texture_AsImage(appcode::gpu::Texture* wrapper) {
  return wrapper->AsImage();
}
