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

#ifndef APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_VIEW_H_
#define APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_VIEW_H_

#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "appcode/flow/embedded_views.h"
#include "appcode/fml/hash_combine.h"
#include "appcode/fml/macros.h"
#include "appcode/shell/platform/embedder/embedder_render_target.h"

namespace appcode {

class EmbedderExternalView {
 public:
  using PlatformViewID = int64_t;
  struct ViewIdentifier {
    std::optional<PlatformViewID> platform_view_id;

    ViewIdentifier() {}

    explicit ViewIdentifier(PlatformViewID view_id)
        : platform_view_id(view_id) {}

    struct Hash {
      constexpr std::size_t operator()(const ViewIdentifier& desc) const {
        if (!desc.platform_view_id.has_value()) {
          return fml::HashCombine();
        }

        return fml::HashCombine(desc.platform_view_id.value());
      }
    };

    struct Equal {
      constexpr bool operator()(const ViewIdentifier& lhs,
                                const ViewIdentifier& rhs) const {
        return lhs.platform_view_id == rhs.platform_view_id;
      }
    };
  };

  struct RenderTargetDescriptor {
    SkISize surface_size;

    explicit RenderTargetDescriptor(const SkISize& p_surface_size)
        : surface_size(p_surface_size) {}

    struct Hash {
      constexpr std::size_t operator()(
          const RenderTargetDescriptor& desc) const {
        return fml::HashCombine(desc.surface_size.width(),
                                desc.surface_size.height());
      }
    };

    struct Equal {
      bool operator()(const RenderTargetDescriptor& lhs,
                      const RenderTargetDescriptor& rhs) const {
        return lhs.surface_size == rhs.surface_size;
      }
    };
  };

  using ViewIdentifierSet = std::unordered_set<ViewIdentifier,
                                               ViewIdentifier::Hash,
                                               ViewIdentifier::Equal>;

  using PendingViews = std::unordered_map<ViewIdentifier,
                                          std::unique_ptr<EmbedderExternalView>,
                                          ViewIdentifier::Hash,
                                          ViewIdentifier::Equal>;

  EmbedderExternalView(const SkISize& frame_size,
                       const SkMatrix& surface_transformation);

  EmbedderExternalView(const SkISize& frame_size,
                       const SkMatrix& surface_transformation,
                       ViewIdentifier view_identifier,
                       std::unique_ptr<EmbeddedViewParams> params);

  ~EmbedderExternalView();

  bool IsRootView() const;

  bool HasPlatformView() const;

  bool HasEngineRenderedContents();

  ViewIdentifier GetViewIdentifier() const;

  const EmbeddedViewParams* GetEmbeddedViewParams() const;

  RenderTargetDescriptor CreateRenderTargetDescriptor() const;

  DlCanvas* GetCanvas();

  SkISize GetRenderSurfaceSize() const;

  bool Render(const EmbedderRenderTarget& render_target,
              bool clear_surface = true);

  const DlRegion& GetDlRegion() const;

 private:
  // End the recording of the slice.
  // Noop if the slice's recording has already ended.
  void TryEndRecording() const;

  const SkISize render_surface_size_;
  const SkMatrix surface_transformation_;
  ViewIdentifier view_identifier_;
  std::unique_ptr<EmbeddedViewParams> embedded_view_params_;
  std::unique_ptr<DisplayListEmbedderViewSlice> slice_;
  std::optional<bool> has_engine_rendered_contents_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderExternalView);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_EMBEDDER_EMBEDDER_EXTERNAL_VIEW_H_
