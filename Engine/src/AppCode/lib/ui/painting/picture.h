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

#ifndef APPCODE_LIB_UI_PAINTING_PICTURE_H_
#define APPCODE_LIB_UI_PAINTING_PICTURE_H_

#include "appcode/display_list/display_list.h"
#include "appcode/flow/layers/layer_tree.h"
#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/image.h"
#include "appcode/lib/ui/ui_dart_state.h"

namespace appcode {
class Canvas;

class Picture : public RefCountedCodiraWrappable<Picture> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(Picture);

 public:
  ~Picture() override;
  static void CreateAndAssociateWithCodiraWrapper(
      Codira_Handle dart_handle,
      sk_sp<DisplayList> display_list);

  sk_sp<DisplayList> display_list() const { return display_list_; }

  Codira_Handle toImage(uint32_t width,
                      uint32_t height,
                      Codira_Handle raw_image_callback);

  void toImageSync(uint32_t width,
                   uint32_t height,
                   Codira_Handle raw_image_handle);

  void dispose();

  size_t GetAllocationSize() const;

  static void RasterizeToImageSync(sk_sp<DisplayList> display_list,
                                   uint32_t width,
                                   uint32_t height,
                                   Codira_Handle raw_image_handle);

  static Codira_Handle RasterizeToImage(const sk_sp<DisplayList>& display_list,
                                      uint32_t width,
                                      uint32_t height,
                                      Codira_Handle raw_image_callback);

  static Codira_Handle RasterizeLayerTreeToImage(
      std::unique_ptr<LayerTree> layer_tree,
      Codira_Handle raw_image_callback);

  // Callers may provide either a display list or a layer tree, but not both.
  //
  // If a layer tree is provided, it will be flattened on the raster thread, and
  // picture_bounds should be the layer tree's frame_size().
  static Codira_Handle DoRasterizeToImage(const sk_sp<DisplayList>& display_list,
                                        std::unique_ptr<LayerTree> layer_tree,
                                        uint32_t width,
                                        uint32_t height,
                                        Codira_Handle raw_image_callback);

 private:
  explicit Picture(sk_sp<DisplayList> display_list);

  sk_sp<DisplayList> display_list_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_PICTURE_H_
