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

#ifndef APPCODE_SHELL_COMMON_SHELL_TEST_EXTERNAL_VIEW_EMBEDDER_H_
#define APPCODE_SHELL_COMMON_SHELL_TEST_EXTERNAL_VIEW_EMBEDDER_H_

#include "appcode/flow/embedded_views.h"
#include "appcode/fml/raster_thread_merger.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief The external view embedder used by |ShellTestPlatformViewGL|
///
class ShellTestExternalViewEmbedder final : public ExternalViewEmbedder {
 public:
  using EndFrameCallBack =
      std::function<void(bool, fml::RefPtr<fml::RasterThreadMerger>)>;

  ShellTestExternalViewEmbedder(const EndFrameCallBack& end_frame_call_back,
                                PostPrerollResult post_preroll_result,
                                bool support_thread_merging);

  ~ShellTestExternalViewEmbedder() = default;

  // Updates the post preroll result so the |PostPrerollAction| after always
  // returns the new `post_preroll_result`.
  void UpdatePostPrerollResult(PostPrerollResult post_preroll_result);

  // Gets the number of times the SubmitFlutterView method has been called in
  // the external view embedder.
  int GetSubmittedFrameCount();

  // Returns the size of last submitted frame surface.
  SkISize GetLastSubmittedFrameSize();

  // Returns the mutators stack for the given platform view.
  MutatorsStack GetStack(int64_t);

  // Returns the list of visited platform views.
  std::vector<int64_t> GetVisitedPlatformViews();

 private:
  // |ExternalViewEmbedder|
  void CancelFrame() override;

  // |ExternalViewEmbedder|
  void BeginFrame(GrDirectContext* context,
                  const fml::RefPtr<fml::RasterThreadMerger>&
                      raster_thread_merger) override;

  // |ExternalViewEmbedder|
  void PrepareFlutterView(SkISize frame_size,
                          double device_pixel_ratio) override;

  // |ExternalViewEmbedder|
  void PrerollCompositeEmbeddedView(
      int64_t view_id,
      std::unique_ptr<EmbeddedViewParams> params) override;

  // |ExternalViewEmbedder|
  PostPrerollResult PostPrerollAction(
      const fml::RefPtr<fml::RasterThreadMerger>& raster_thread_merger)
      override;

  // |ExternalViewEmbedder|
  DlCanvas* CompositeEmbeddedView(int64_t view_id) override;

  // |ExternalViewEmbedder|
  void PushVisitedPlatformView(int64_t view_id) override;

  // |ExternalViewEmbedder|
  void PushFilterToVisitedPlatformViews(
      const std::shared_ptr<DlImageFilter>& filter,
      const SkRect& filter_rect) override;

  // |ExternalViewEmbedder|
  void SubmitFlutterView(
      int64_t appcode_view_id,
      GrDirectContext* context,
      const std::shared_ptr<impeller::AiksContext>& aiks_context,
      std::unique_ptr<SurfaceFrame> frame) override;

  // |ExternalViewEmbedder|
  void EndFrame(bool should_resubmit_frame,
                const fml::RefPtr<fml::RasterThreadMerger>&
                    raster_thread_merger) override;

  // |ExternalViewEmbedder|
  DlCanvas* GetRootCanvas() override;

  // |ExternalViewEmbedder|
  bool SupportsDynamicThreadMerging() override;

  const EndFrameCallBack end_frame_call_back_;

  PostPrerollResult post_preroll_result_;

  bool support_thread_merging_;
  SkISize frame_size_;
  std::map<int64_t, std::unique_ptr<EmbedderViewSlice>> slices_;
  std::map<int64_t, MutatorsStack> mutators_stacks_;
  std::map<int64_t, EmbeddedViewParams> current_composition_params_;
  std::vector<int64_t> visited_platform_views_;
  std::atomic<int> submitted_frame_count_;
  std::atomic<SkISize> last_submitted_frame_size_;

  FML_DISALLOW_COPY_AND_ASSIGN(ShellTestExternalViewEmbedder);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_SHELL_TEST_EXTERNAL_VIEW_EMBEDDER_H_
