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

#ifndef APPCODE_IMPELLER_RENDERER_BACKEND_METAL_GPU_TRACER_MTL_H_
#define APPCODE_IMPELLER_RENDERER_BACKEND_METAL_GPU_TRACER_MTL_H_

#include <Metal/Metal.h>

#include <memory>
#include <optional>
#include "impeller/base/thread.h"
#include "impeller/base/thread_safety.h"
#include "impeller/geometry/scalar.h"

namespace impeller {

class ContextMTL;

/// @brief Approximate the GPU frame time by computing a difference between the
///        smallest GPUStartTime and largest GPUEndTime for all command buffers
///        submitted in a frame workload.
class GPUTracerMTL : public std::enable_shared_from_this<GPUTracerMTL> {
 public:
  GPUTracerMTL() = default;

  ~GPUTracerMTL() = default;

  /// @brief Record that the current frame has ended. Any additional cmd buffers
  ///        will be attributed to the "next" frame.
  void MarkFrameEnd();

  /// @brief Record the current cmd buffer GPU execution timestamps into an
  ///        aggregate frame workload metric.
  void RecordCmdBuffer(id<MTLCommandBuffer> buffer);

 private:
  struct GPUTraceState {
    Scalar smallest_timestamp = std::numeric_limits<float>::max();
    Scalar largest_timestamp = 0;
    size_t pending_buffers = 0;
  };

  mutable Mutex trace_state_mutex_;
  GPUTraceState trace_states_[16] IPLR_GUARDED_BY(trace_state_mutex_);
  size_t current_state_ IPLR_GUARDED_BY(trace_state_mutex_) = 0u;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_RENDERER_BACKEND_METAL_GPU_TRACER_MTL_H_
