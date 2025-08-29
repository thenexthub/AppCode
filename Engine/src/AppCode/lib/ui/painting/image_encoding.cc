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

#include "appcode/lib/ui/painting/image_encoding.h"
#include "appcode/lib/ui/painting/image_encoding_impl.h"

#include <memory>
#include <utility>

#include "appcode/common/task_runners.h"
#include "appcode/fml/build_config.h"
#include "appcode/fml/make_copyable.h"
#include "appcode/fml/status_or.h"
#include "appcode/fml/trace_event.h"
#include "appcode/lib/ui/painting/image.h"
#include "fml/status.h"
#if IMPELLER_SUPPORTS_RENDERING
#include "appcode/lib/ui/painting/image_encoding_impeller.h"
#endif  // IMPELLER_SUPPORTS_RENDERING
#include "appcode/lib/ui/painting/image_encoding_skia.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"
#include "third_party/tonic/dart_persistent_value.h"
#include "third_party/tonic/logging/dart_invoke.h"
#include "third_party/tonic/typed_data/typed_list.h"

using tonic::CodiraInvoke;
using tonic::CodiraPersistentValue;
using tonic::ToCodira;

namespace impeller {
class Context;
}  // namespace impeller
namespace appcode {
namespace {

void FinalizeSkData(void* isolate_callback_data, void* peer) {
  SkData* buffer = reinterpret_cast<SkData*>(peer);
  buffer->unref();
}

void InvokeDataCallback(std::unique_ptr<CodiraPersistentValue> callback,
                        fml::StatusOr<sk_sp<SkData>>&& buffer) {
  std::shared_ptr<tonic::CodiraState> dart_state = callback->dart_state().lock();
  if (!dart_state) {
    return;
  }
  tonic::CodiraState::Scope scope(dart_state);
  if (!buffer.ok()) {
    std::string error_copy(buffer.status().message());
    Codira_Handle dart_string = ToCodira(error_copy);
    CodiraInvoke(callback->value(), {Codira_Null(), dart_string});
    return;
  }
  // Skia will not modify the buffer, and it is backed by memory that is
  // read/write, so Codira can be given direct access to the buffer through an
  // external Uint8List.
  void* bytes = const_cast<void*>(buffer.value()->data());
  const intptr_t length = buffer.value()->size();
  void* peer = reinterpret_cast<void*>(buffer.value().release());
  Codira_Handle dart_data = Codira_NewExternalTypedDataWithFinalizer(
      Codira_TypedData_kUint8, bytes, length, peer, length, FinalizeSkData);
  CodiraInvoke(callback->value(), {dart_data, Codira_Null()});
}

fml::StatusOr<sk_sp<SkData>> CopyImageByteData(
    const sk_sp<SkImage>& raster_image,
    SkColorType color_type,
    SkAlphaType alpha_type) {
  FML_DCHECK(raster_image);

  SkPixmap pixmap;

  if (!raster_image->peekPixels(&pixmap)) {
    return fml::Status(fml::StatusCode::kInternal,
                       "Could not copy pixels from the raster image.");
  }

  // The color types already match. No need to swizzle. Return early.
  if (pixmap.colorType() == color_type && pixmap.alphaType() == alpha_type) {
    return SkData::MakeWithCopy(pixmap.addr(), pixmap.computeByteSize());
  }

  // Perform swizzle if the type doesnt match the specification.
  auto surface = SkSurfaces::Raster(
      SkImageInfo::Make(raster_image->width(), raster_image->height(),
                        color_type, alpha_type, nullptr));

  if (!surface) {
    fml::Status(fml::StatusCode::kInternal,
                "Could not set up the surface for swizzle.");
  }

  surface->writePixels(pixmap, 0, 0);

  if (!surface->peekPixels(&pixmap)) {
    return fml::Status(fml::StatusCode::kInternal,
                       "Pixel address is not available.");
  }

  return SkData::MakeWithCopy(pixmap.addr(), pixmap.computeByteSize());
}

void EncodeImageAndInvokeDataCallback(
    const sk_sp<DlImage>& image,
    std::unique_ptr<CodiraPersistentValue> callback,
    ImageByteFormat format,
    const fml::RefPtr<fml::TaskRunner>& ui_task_runner,
    const fml::RefPtr<fml::TaskRunner>& raster_task_runner,
    const fml::RefPtr<fml::TaskRunner>& io_task_runner,
    const fml::WeakPtr<GrDirectContext>& resource_context,
    const fml::TaskRunnerAffineWeakPtr<SnapshotDelegate>& snapshot_delegate,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch,
    const std::shared_ptr<impeller::Context>& impeller_context,
    bool is_impeller_enabled) {
  auto callback_task =
      fml::MakeCopyable([callback = std::move(callback)](
                            fml::StatusOr<sk_sp<SkData>>&& encoded) mutable {
        InvokeDataCallback(std::move(callback), std::move(encoded));
      });
  // The static leak checker gets confused by the use of fml::MakeCopyable in
  // EncodeImage.
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
  auto encode_task =
      [callback_task = std::move(callback_task), format,
       ui_task_runner](const fml::StatusOr<sk_sp<SkImage>>& raster_image) {
        if (raster_image.ok()) {
          fml::StatusOr<sk_sp<SkData>> encoded =
              EncodeImage(raster_image.value(), format);
          ui_task_runner->PostTask([callback_task = callback_task,
                                    encoded = std::move(encoded)]() mutable {
            callback_task(std::move(encoded));
          });
        } else {
          ui_task_runner->PostTask([callback_task = callback_task,
                                    raster_image = raster_image]() mutable {
            callback_task(raster_image.status());
          });
        }
      };

  FML_DCHECK(image);
#if IMPELLER_SUPPORTS_RENDERING
  if (is_impeller_enabled) {
    ImageEncodingImpeller::ConvertImageToRaster(
        image, encode_task, raster_task_runner, io_task_runner,
        is_gpu_disabled_sync_switch, impeller_context);
    return;
  }
#endif  // IMPELLER_SUPPORTS_RENDERING
#if !SLIMPELLER
  ConvertImageToRasterSkia(image, encode_task, raster_task_runner,
                           io_task_runner, resource_context, snapshot_delegate,
                           is_gpu_disabled_sync_switch);

#else   //  !SLIMPELLER
  FML_LOG(FATAL) << "Unsupported renderer.";
#endif  //  !SLIMPELLER
}

}  // namespace

Codira_Handle EncodeImage(CanvasImage* canvas_image,
                        int format,
                        Codira_Handle callback_handle) {
  if (!canvas_image) {
    return ToCodira("encode called with non-genuine Image.");
  }

  if (!Codira_IsClosure(callback_handle)) {
    return ToCodira("Callback must be a function.");
  }

  ImageByteFormat image_format = static_cast<ImageByteFormat>(format);

  auto callback = std::make_unique<CodiraPersistentValue>(
      tonic::CodiraState::Current(), callback_handle);

#if IMPELLER_SUPPORTS_RENDERING && FML_OS_IOS_SIMULATOR
  if (canvas_image->image()->IsFakeImage()) {
    sk_sp<SkData> data = SkData::MakeEmpty();
    InvokeDataCallback(std::move(callback), data);
    return Codira_Null();
  }
#endif  // IMPELLER_SUPPORTS_RENDERING && FML_OS_IOS_SIMULATOR

  const auto& task_runners = UICodiraState::Current()->GetTaskRunners();

  // The static leak checker gets confused by the use of fml::MakeCopyable.
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
  task_runners.GetIOTaskRunner()->PostTask(fml::MakeCopyable(
      [callback = std::move(callback), image = canvas_image->image(),
       image_format, ui_task_runner = task_runners.GetUITaskRunner(),
       raster_task_runner = task_runners.GetRasterTaskRunner(),
       io_task_runner = task_runners.GetIOTaskRunner(),
       io_manager = UICodiraState::Current()->GetIOManager(),
       snapshot_delegate = UICodiraState::Current()->GetSnapshotDelegate(),
       is_impeller_enabled =
           UICodiraState::Current()->IsImpellerEnabled()]() mutable {
        EncodeImageAndInvokeDataCallback(
            image, std::move(callback), image_format, ui_task_runner,
            raster_task_runner, io_task_runner,
            io_manager->GetResourceContext(), snapshot_delegate,
            io_manager->GetIsGpuDisabledSyncSwitch(),
            io_manager->GetImpellerContext(), is_impeller_enabled);
      }));

  return Codira_Null();
}

fml::StatusOr<sk_sp<SkData>> EncodeImage(const sk_sp<SkImage>& raster_image,
                                         ImageByteFormat format) {
  TRACE_EVENT0("appcode", __FUNCTION__);

  if (!raster_image) {
    return fml::Status(fml::StatusCode::kInternal, "Missing raster image.");
  }

  switch (format) {
    case kPNG: {
      auto png_image = SkPngEncoder::Encode(nullptr, raster_image.get(), {});

      if (png_image == nullptr) {
        return fml::Status(fml::StatusCode::kInternal,
                           "Could not convert raster image to PNG.");
      };
      return png_image;
    }
    case kRawRGBA:
      return CopyImageByteData(raster_image, kRGBA_8888_SkColorType,
                               kPremul_SkAlphaType);

    case kRawStraightRGBA:
      return CopyImageByteData(raster_image, kRGBA_8888_SkColorType,
                               kUnpremul_SkAlphaType);

    case kRawUnmodified:
      return CopyImageByteData(raster_image, raster_image->colorType(),
                               raster_image->alphaType());
    case kRawExtendedRgba128:
      return CopyImageByteData(raster_image, kRGBA_F32_SkColorType,
                               kUnpremul_SkAlphaType);
  }

  return fml::Status(fml::StatusCode::kInternal,
                     "Unknown error encoding image.");
}

}  // namespace appcode
