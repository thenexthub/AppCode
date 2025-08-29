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

#ifndef APPCODE_IMPELLER_ENTITY_ENTITY_PASS_CLIP_STACK_H_
#define APPCODE_IMPELLER_ENTITY_ENTITY_PASS_CLIP_STACK_H_

#include "impeller/entity/contents/clip_contents.h"
#include "impeller/geometry/rect.h"

namespace impeller {

struct ClipCoverageLayer {
  std::optional<Rect> coverage;
  size_t clip_height = 0;
};

/// @brief A class that tracks all clips that have been recorded in the current
///        entity pass stencil.
///
///        These clips are replayed when restoring the backdrop so that the
///        stencil buffer is left in an identical state.
class EntityPassClipStack {
 public:
  struct ReplayResult {
    ClipContents clip_contents;
    Matrix transform;
    std::optional<Rect> clip_coverage;
    uint32_t clip_depth = 0;
  };

  struct ClipStateResult {
    /// Whether or not the Entity should be rendered. If false, the Entity may
    /// be safely skipped.
    bool should_render = false;
    /// Whether or not the current clip coverage changed during the call to
    /// `ApplyClipState`.
    bool clip_did_change = false;
  };

  /// Create a new [EntityPassClipStack] with an initialized coverage rect.
  explicit EntityPassClipStack(const Rect& initial_coverage_rect);

  ~EntityPassClipStack() = default;

  std::optional<Rect> CurrentClipCoverage() const;

  void PushSubpass(std::optional<Rect> subpass_coverage, size_t clip_height);

  void PopSubpass();

  bool HasCoverage() const;

  ClipStateResult RecordClip(const ClipContents& clip_contents,
                             Matrix transform,
                             Point global_pass_position,
                             uint32_t clip_depth,
                             size_t clip_height_floor,
                             bool is_aa);

  ReplayResult& GetLastReplayResult() {
    return GetCurrentSubpassState().rendered_clip_entities.back();
  }

  ClipStateResult RecordRestore(Point global_pass_position,
                                size_t restore_height);

  const std::vector<ReplayResult>& GetReplayEntities() const;

  // Visible for testing.
  const std::vector<ClipCoverageLayer> GetClipCoverageLayers() const;

 private:
  struct SubpassState {
    std::vector<ReplayResult> rendered_clip_entities;
    std::vector<ClipCoverageLayer> clip_coverage;
  };

  SubpassState& GetCurrentSubpassState();

  std::vector<SubpassState> subpass_state_;
  size_t next_replay_index_ = 0;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_ENTITY_PASS_CLIP_STACK_H_
