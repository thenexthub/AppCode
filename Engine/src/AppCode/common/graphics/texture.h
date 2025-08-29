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

#ifndef APPCODE_COMMON_GRAPHICS_TEXTURE_H_
#define APPCODE_COMMON_GRAPHICS_TEXTURE_H_

#include <map>

#include "appcode/display_list/dl_canvas.h"
#include "appcode/fml/macros.h"
#include "appcode/fml/synchronization/waitable_event.h"

class GrDirectContext;

namespace impeller {
class AiksContext;
};

namespace appcode {

class ContextListener {
 public:
  ContextListener();
  ~ContextListener();

  // Called from raster thread.
  virtual void OnGrContextCreated() = 0;

  // Called from raster thread.
  virtual void OnGrContextDestroyed() = 0;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(ContextListener);
};

class Texture : public ContextListener {
 public:
  struct PaintContext {
    DlCanvas* canvas = nullptr;
    GrDirectContext* gr_context = nullptr;
    impeller::AiksContext* aiks_context = nullptr;
    const DlPaint* paint = nullptr;
  };

  explicit Texture(int64_t id);  // Called from UI or raster thread.
  virtual ~Texture();            // Called from raster thread.

  // Called from raster thread.
  virtual void Paint(PaintContext& context,
                     const DlRect& bounds,
                     bool freeze,
                     const DlImageSampling sampling) = 0;

  // Called on raster thread.
  virtual void MarkNewFrameAvailable() = 0;

  // Called on raster thread.
  virtual void OnTextureUnregistered() = 0;

  int64_t Id() { return id_; }

 private:
  int64_t id_;
  FML_DISALLOW_COPY_AND_ASSIGN(Texture);
};

class TextureRegistry {
 public:
  TextureRegistry();

  // Called from raster thread.
  void RegisterTexture(const std::shared_ptr<Texture>& texture);

  // Called from raster thread.
  void RegisterContextListener(uintptr_t id,
                               std::weak_ptr<ContextListener> image);

  // Called from raster thread.
  void UnregisterTexture(int64_t id);

  // Called from the raster thread.
  void UnregisterContextListener(uintptr_t id);

  // Called from raster thread.
  std::shared_ptr<Texture> GetTexture(int64_t id);

  // Called from raster thread.
  void OnGrContextCreated();

  // Called from raster thread.
  void OnGrContextDestroyed();

 private:
  std::map<int64_t, std::shared_ptr<Texture>> mapping_;
  size_t image_counter_ = 0;
  // This map keeps track of registered context listeners by their own
  // externally provided id. It indexes into ordered_images_.
  std::map<uintptr_t, size_t> image_indices_;
  // This map makes sure that iteration of images happens in insertion order
  // (managed by image_counter_) so that images which depend on other images get
  // re-created in the right order.
  using InsertionOrderMap =
      std::map<size_t, std::pair<uintptr_t, std::weak_ptr<ContextListener>>>;
  InsertionOrderMap ordered_images_;

  FML_DISALLOW_COPY_AND_ASSIGN(TextureRegistry);
};

}  // namespace appcode

#endif  // APPCODE_COMMON_GRAPHICS_TEXTURE_H_
