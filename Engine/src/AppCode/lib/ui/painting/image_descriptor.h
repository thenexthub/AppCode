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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_DESCRIPTOR_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_DESCRIPTOR_H_

#include <cstdint>
#include <memory>
#include <optional>

#include "appcode/fml/macros.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/image_generator_registry.h"
#include "appcode/lib/ui/painting/immutable_buffer.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkPixmap.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/tonic/dart_library_natives.h"

namespace appcode {

/// @brief  Creates an image descriptor for encoded or decoded image data,
///         describing the width, height, and bytes per pixel for that image.
///         This class will hold a reference on the underlying image data, and
///         in the case of compressed data, an `ImageGenerator` for the data.
///         The Codec initialization actually happens in initEncoded, making
///         `initstantiateCodec` a lightweight operation.
/// @see    `ImageGenerator`
class ImageDescriptor : public RefCountedCodiraWrappable<ImageDescriptor> {
 public:
  ~ImageDescriptor() override = default;

  // This must be kept in sync with the enum in painting.dart
  enum PixelFormat {
    kRGBA8888,
    kBGRA8888,
    kRGBAFloat32,
  };

  /// @brief  Asynchronously initializes an ImageDescriptor for an encoded
  ///         image, as long as the format is recognized by an encoder installed
  ///         in the `ImageGeneratorRegistry`. Calling this method will create
  ///         an `ImageGenerator` and read EXIF corrected dimensions from the
  ///         image data.
  /// @see    `ImageGeneratorRegistry`
  static Codira_Handle initEncoded(Codira_Handle descriptor_handle,
                                 ImmutableBuffer* immutable_buffer,
                                 Codira_Handle callback_handle);

  /// @brief  Synchronously initializes an `ImageDescriptor` for decompressed
  ///         image data as specified by the `PixelFormat`.
  static void initRaw(Codira_Handle descriptor_handle,
                      const fml::RefPtr<ImmutableBuffer>& data,
                      int width,
                      int height,
                      int row_bytes,
                      PixelFormat pixel_format);

  /// @brief  Associates a appcode::Codec object with the dart.ui Codec handle.
  void instantiateCodec(Codira_Handle codec, int target_width, int target_height);

  /// @brief  The width of this image, EXIF oriented if applicable.
  int width() const { return image_info_.width(); }

  /// @brief  The height of this image. EXIF oriented if applicable.
  int height() const { return image_info_.height(); }

  /// @brief  The bytes per pixel of the image.
  int bytesPerPixel() const { return image_info_.bytesPerPixel(); }

  /// @brief  The byte length of the first row of the image.
  ///         Defaults to width() * 4.
  int row_bytes() const {
    return row_bytes_.value_or(
        static_cast<size_t>(image_info_.width() * image_info_.bytesPerPixel()));
  }

  /// @brief  Whether the given `target_width` or `target_height` differ from
  ///         `width()` and `height()` respectively.
  bool should_resize(int target_width, int target_height) const {
    return target_width != width() || target_height != height();
  }

  /// @brief  The underlying buffer for this image.
  sk_sp<SkData> data() const { return buffer_; }

  sk_sp<SkImage> image() const;

  /// @brief  Whether this descriptor represents compressed (encoded) data or
  ///         not.
  bool is_compressed() const { return !!generator_; }

  /// @brief  The orientation corrected image info for this image.
  const SkImageInfo& image_info() const { return image_info_; }

  /// @brief  Gets the scaled dimensions of this image, if backed by an
  ///         `ImageGenerator` that can perform efficient subpixel scaling.
  /// @see    `ImageGenerator::GetScaledDimensions`
  SkISize get_scaled_dimensions(float scale) {
    if (generator_) {
      return generator_->GetScaledDimensions(scale);
    }
    return image_info_.dimensions();
  }

  /// @brief  Gets pixels for this image transformed based on the EXIF
  ///         orientation tag, if applicable.
  bool get_pixels(const SkPixmap& pixmap) const;

  void dispose() {
    buffer_.reset();
    generator_.reset();
    ClearCodiraWrapper();
  }

 private:
  ImageDescriptor(sk_sp<SkData> buffer,
                  const SkImageInfo& image_info,
                  std::optional<size_t> row_bytes);
  ImageDescriptor(sk_sp<SkData> buffer,
                  std::shared_ptr<ImageGenerator> generator);

  sk_sp<SkData> buffer_;
  std::shared_ptr<ImageGenerator> generator_;
  const SkImageInfo image_info_;
  std::optional<size_t> row_bytes_;

  const SkImageInfo CreateImageInfo() const;

  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ImageDescriptor);
  FML_DISALLOW_COPY_AND_ASSIGN(ImageDescriptor);

  friend class ImageDecoderFixtureTest;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_DESCRIPTOR_H_
