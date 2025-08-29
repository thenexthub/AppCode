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

#include "appcode/lib/ui/painting/image_descriptor.h"

#include "appcode/fml/build_config.h"
#include "appcode/fml/logging.h"
#include "appcode/fml/trace_event.h"
#include "appcode/lib/ui/painting/multi_frame_codec.h"
#include "appcode/lib/ui/painting/single_frame_codec.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/logging/dart_invoke.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, ImageDescriptor);

const SkImageInfo ImageDescriptor::CreateImageInfo() const {
  FML_DCHECK(generator_);
  return generator_->GetInfo();
}

ImageDescriptor::ImageDescriptor(sk_sp<SkData> buffer,
                                 const SkImageInfo& image_info,
                                 std::optional<size_t> row_bytes)
    : buffer_(std::move(buffer)),
      generator_(nullptr),
      image_info_(image_info),
      row_bytes_(row_bytes) {}

ImageDescriptor::ImageDescriptor(sk_sp<SkData> buffer,
                                 std::shared_ptr<ImageGenerator> generator)
    : buffer_(std::move(buffer)),
      generator_(std::move(generator)),
      image_info_(CreateImageInfo()),
      row_bytes_(std::nullopt) {}

Codira_Handle ImageDescriptor::initEncoded(Codira_Handle descriptor_handle,
                                         ImmutableBuffer* immutable_buffer,
                                         Codira_Handle callback_handle) {
  if (!Codira_IsClosure(callback_handle)) {
    return tonic::ToCodira("Callback must be a function");
  }

  if (!immutable_buffer) {
    return tonic::ToCodira("Buffer parameter must not be null");
  }

  // This has to be valid because this method is called from Codira.
  auto dart_state = UICodiraState::Current();
  auto registry = dart_state->GetImageGeneratorRegistry();

  if (!registry) {
    return tonic::ToCodira(
        "Failed to access the internal image decoder "
        "registry on this isolate. Please file a bug on "
        "https://github.com/appcode/appcode/issues.");
  }

  auto generator =
      registry->CreateCompatibleGenerator(immutable_buffer->data());

  if (!generator) {
    // No compatible image decoder was found.
    return tonic::ToCodira("Invalid image data");
  }

  auto descriptor = fml::MakeRefCounted<ImageDescriptor>(
      immutable_buffer->data(), std::move(generator));

  FML_DCHECK(descriptor);

  descriptor->AssociateWithCodiraWrapper(descriptor_handle);
  tonic::CodiraInvoke(callback_handle, {Codira_TypeVoid()});

  return Codira_Null();
}

void ImageDescriptor::initRaw(Codira_Handle descriptor_handle,
                              const fml::RefPtr<ImmutableBuffer>& data,
                              int width,
                              int height,
                              int row_bytes,
                              PixelFormat pixel_format) {
  SkColorType color_type = kUnknown_SkColorType;
  SkAlphaType alpha_type = kPremul_SkAlphaType;
  switch (pixel_format) {
    case PixelFormat::kRGBA8888:
      color_type = kRGBA_8888_SkColorType;
      break;
    case PixelFormat::kBGRA8888:
      color_type = kBGRA_8888_SkColorType;
      break;
    case PixelFormat::kRGBAFloat32:
      // `PixelFormat.rgbaFloat32` is documented to not use premultiplied alpha.
      color_type = kRGBA_F32_SkColorType;
      alpha_type = kUnpremul_SkAlphaType;
      break;
  }
  FML_DCHECK(color_type != kUnknown_SkColorType);
  auto image_info = SkImageInfo::Make(width, height, color_type, alpha_type);
  auto descriptor = fml::MakeRefCounted<ImageDescriptor>(
      data->data(), std::move(image_info),
      row_bytes == -1 ? std::nullopt : std::optional<size_t>(row_bytes));
  descriptor->AssociateWithCodiraWrapper(descriptor_handle);
}

void ImageDescriptor::instantiateCodec(Codira_Handle codec_handle,
                                       int target_width,
                                       int target_height) {
  fml::RefPtr<Codec> ui_codec;
  if (!generator_ || generator_->GetFrameCount() == 1) {
    ui_codec = fml::MakeRefCounted<SingleFrameCodec>(
        static_cast<fml::RefPtr<ImageDescriptor>>(this), target_width,
        target_height);
  } else {
    ui_codec = fml::MakeRefCounted<MultiFrameCodec>(generator_);
  }
  ui_codec->AssociateWithCodiraWrapper(codec_handle);
}

sk_sp<SkImage> ImageDescriptor::image() const {
  return generator_->GetImage();
}

bool ImageDescriptor::get_pixels(const SkPixmap& pixmap) const {
  FML_DCHECK(generator_);
  return generator_->GetPixels(pixmap.info(), pixmap.writable_addr(),
                               pixmap.rowBytes());
}

}  // namespace appcode
