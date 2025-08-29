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

#ifndef APPCODE_LIB_UI_PAINTING_DISPLAY_LIST_DEFERRED_IMAGE_GPU_IMPELLER_H_
#define APPCODE_LIB_UI_PAINTING_DISPLAY_LIST_DEFERRED_IMAGE_GPU_IMPELLER_H_

#include "appcode/common/graphics/texture.h"
#include "appcode/display_list/image/dl_image.h"
#include "appcode/flow/layers/layer_tree.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/fml/task_runner.h"
#include "appcode/lib/ui/snapshot_delegate.h"
#include "impeller/core/texture.h"

namespace appcode {

class DlDeferredImageGPUImpeller final : public DlImage {
 public:
  static sk_sp<DlDeferredImageGPUImpeller> Make(
      std::unique_ptr<LayerTree> layer_tree,
      fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate,
      fml::RefPtr<fml::TaskRunner> raster_task_runner);

  static sk_sp<DlDeferredImageGPUImpeller> Make(
      sk_sp<DisplayList> display_list,
      const SkISize& size,
      fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate,
      fml::RefPtr<fml::TaskRunner> raster_task_runner);

  // |DlImage|
  ~DlDeferredImageGPUImpeller() override;

  // |DlImage|
  sk_sp<SkImage> skia_image() const override;

  // |DlImage|
  std::shared_ptr<impeller::Texture> impeller_texture() const override;

  // |DlImage|
  bool isOpaque() const override;

  // |DlImage|
  bool isTextureBacked() const override;

  // |DlImage|
  bool isUIThreadSafe() const override;

  // |DlImage|
  DlISize GetSize() const override;

  // |DlImage|
  size_t GetApproximateByteSize() const override;

  // |DlImage|
  OwningContext owning_context() const override {
    return OwningContext::kRaster;
  }

 private:
  class ImageWrapper final : public std::enable_shared_from_this<ImageWrapper>,
                             public ContextListener {
   public:
    ~ImageWrapper();

    static std::shared_ptr<ImageWrapper> Make(
        sk_sp<DisplayList> display_list,
        const SkISize& size,
        fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate,
        fml::RefPtr<fml::TaskRunner> raster_task_runner);

    static std::shared_ptr<ImageWrapper> Make(
        std::unique_ptr<LayerTree> layer_tree,
        fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate,
        fml::RefPtr<fml::TaskRunner> raster_task_runner);

    bool isTextureBacked() const;

    const std::shared_ptr<impeller::Texture> texture() const {
      return texture_;
    }

    const SkISize size() const { return size_; }

    std::optional<std::string> get_error();

   private:
    SkISize size_;
    sk_sp<DisplayList> display_list_;
    std::shared_ptr<impeller::Texture> texture_;
    fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate_;
    fml::RefPtr<fml::TaskRunner> raster_task_runner_;
    std::shared_ptr<TextureRegistry> texture_registry_;

    mutable std::mutex error_mutex_;
    std::optional<std::string> error_;

    ImageWrapper(
        sk_sp<DisplayList> display_list,
        const SkISize& size,
        fml::TaskRunnerAffineWeakPtr<SnapshotDelegate> snapshot_delegate,
        fml::RefPtr<fml::TaskRunner> raster_task_runner);

    // If a layer tree is provided, it will be flattened during the raster
    // thread task spawned by this method. After being flattened into a display
    // list, the image wrapper will be updated to hold this display list and the
    // layer tree can be dropped.
    void SnapshotDisplayList(std::unique_ptr<LayerTree> layer_tree = nullptr);

    // |ContextListener|
    void OnGrContextCreated() override;

    // |ContextListener|
    void OnGrContextDestroyed() override;

    FML_DISALLOW_COPY_AND_ASSIGN(ImageWrapper);
  };

  const std::shared_ptr<ImageWrapper> wrapper_;

  explicit DlDeferredImageGPUImpeller(std::shared_ptr<ImageWrapper> wrapper);

  FML_DISALLOW_COPY_AND_ASSIGN(DlDeferredImageGPUImpeller);
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_DISPLAY_LIST_DEFERRED_IMAGE_GPU_IMPELLER_H_
